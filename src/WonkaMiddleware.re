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
 * The result of handling an event can either be a signal to respond with json, or to move on to
 * the next Express middleware in the stack.
 */
type jsonResult =
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Next;

/**
 * A handler transforms an httpEvent into a jsonResult.
 */
type handler = operatorT(httpEvent, jsonResult);

include Middleware.Make({
  type f = (Middleware.next, Request.t, Response.t) => sourceT(complete);

  type errorF =
    (Middleware.next, Error.t, Request.t, Response.t) => sourceT(complete);

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
         }
       )
  );
};
