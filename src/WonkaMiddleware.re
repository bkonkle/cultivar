open Express;
open Wonka;
open Wonka_types;

/**
 * An httpEvent is composed of an Express Request and Response.
 */
type httpEvent = {
  req: Request.t,
  res: Response.t,
};

/**
 * The result of handling an event can be a signal to respond with json, to move on to
 * the next Express middleware in the stack, or to handle an error.
 */
type jsonResult =
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Next
  | Error(exn);

let apply = (f, next, req, res) =>
  (
    try(f(next, req, res)) {
    | e => next(Next.error(e), res) |> fromValue
    }
  )
  |> publish
  |> ignore;

let applyWithError = (f, next, err, req, res) => {
  (
    try(f(next, err, req, res)) {
    | e => next(Next.error(e), res) |> fromValue
    }
  )
  |> publish
  |> ignore;
};

/**
 * A handler transforms an httpEvent into a jsonResult.
 */
type handler = operatorT(httpEvent, jsonResult);

include Middleware.Make({
  type f = (Middleware.next, Request.t, Response.t) => sourceT(complete);

  type errorF =
    (Middleware.next, Error.t, Request.t, Response.t) => sourceT(complete);

  let apply = apply;
  let applyWithError = applyWithError;
});

/**
 * Turn a list of keys and values into a JSON object.
 */
let toJson = (list: list((Js.Dict.key, 'a))) =>
  Js.Dict.fromList(list) |> Js.Json.object_;

/**
 * Creates Express middleware from a handler.
 */
[@genType]
let middleware = (handler: handler) => {
  from((next, req, res) =>
    fromValue({req, res})
    |> handler
    |> map((. result) =>
         switch (result) {
         | Respond(statusCode, data) =>
           res |> Response.status(statusCode) |> Response.sendJson(data)
         | Next =>
           try(res |> next(Next.route)) {
           | e => res |> next(Next.error(e))
           }
         | Error(e) => res |> next(Next.error(e))
         }
       )
  );
};

type either =
  | Right
  | Left;

/**
 * Applies one operator or the other to a source based on a predicate that returns an either.
 */
let either =
    (
      ~test: 'event => either,
      ~right: operatorT('newEvent, 'result),
      ~left: operatorT('event, 'result),
      source: sourceT('event),
      sink: sinkT('result),
    ) =>
  source((. signal) => {
    switch (signal) {
    | Start(tb) => sink(. Start(tb))
    | Push(event) =>
      let handler =
        switch (test(event)) {
        | Right => right
        | Left => left
        };
      handler(fromValue(event), sink);
    | End => sink(. End)
    }
  });
