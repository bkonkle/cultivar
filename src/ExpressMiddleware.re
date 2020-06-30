open Express;
open ExpressHttp;
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

include Middleware.Make({
  type f = (Middleware.next, Request.t, Response.t) => sourceT(complete);

  type errorF =
    (Middleware.next, Error.t, Request.t, Response.t) => sourceT(complete);

  let apply = apply;
  let applyWithError = applyWithError;
});

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

let getEmptyContext = (_req: Request.t): Js.nullable('context) => Js.Nullable.null;

/**
 * Creates Express middleware from a root exchange and optional context.
 */
[@genType]
let middleware =
    (~getContext=getEmptyContext, exchange: Exchange.t('context)) =>
  from((next, req, res) => {
    fromValue({
      http: {
        req,
        res,
      },
    })
    |> exchange({
         forward: map((. _) => Forward),
         context: getContext(req) |> Js.Nullable.toOption,
       })
    |> map((. result) =>
         switch (result) {
         | Respond(statusCode, data) =>
           // The type in bs-express is incorrect - this actually returns `undefined`
           res |> Response.status(statusCode) |> ignore;

           res |> Response.sendJson(data);
         | Forward =>
           try(res |> next(Next.route)) {
           | e => res |> next(Next.error(e))
           }
         | Reject(e) => res |> next(Next.error(e))
         }
       )
  });
