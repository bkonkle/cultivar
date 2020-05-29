open Express;
open Wonka;
open Wonka_types;

type event = (Express.Request.t, Express.Response.t);

type result =
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Next;

type handler = operatorT(event, result);

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

let toJson = (list: list((Js.Dict.key, 'a))) =>
  Js.Dict.fromList(list) |> Js.Json.object_;

[@genType]
let middleware = (handler: handler) => {
  from((next, req, res) =>
    fromValue((req, res))
    |> handler
    |> take(1)
    |> map((. result) =>
         switch (result) {
         | Respond(statusCode, data) =>
           res |> Response.status(statusCode) |> Response.sendJson(data)
         | Next =>
           try(res |> next(Next.middleware)) {
           | e => res |> next(Next.error(e))
           }
         }
       )
  );
};
