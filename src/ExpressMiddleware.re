open Express;
open Wonka;
open Wonka_types;

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

module Exchange = OperationExchange.Make(HttpOperation);

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
 * Applies one operator or the other to a source based on a predicate that returns an either.
 */
let either =
    (
      ~test: 'event => Either.t,
      ~left: operatorT('event, 'result),
      ~right: operatorT('event, 'result),
      source: sourceT('event),
      sink: sinkT('result),
    ) =>
  source((. signal) => {
    switch (signal) {
    | Start(tb) => sink(. Start(tb))
    | Push(event) =>
      let handler =
        switch (test(event)) {
        | Left => left
        | Right => right
        };
      handler(fromValue(event), sink);
    | End => sink(. End)
    }
  });

/**
 * Creates Express middleware from a root exchange.
 */
[@genType]
let middleware = (exchange: Exchange.t) =>
  HttpOperation.(
    from((next, req, res) => {
      fromValue({
        http: {
          req,
          res,
        },
      })
      |> exchange
      |> map((. result) =>
           switch (result) {
           | Respond(statusCode, data) =>
             res |> Response.status(statusCode) |> Response.sendJson(data)
           | Next =>
             try(res |> next(Next.route)) {
             | e => res |> next(Next.error(e))
             }
           | HttpOperation.Error(e) => res |> next(Next.error(e))
           }
         )
    })
  );
