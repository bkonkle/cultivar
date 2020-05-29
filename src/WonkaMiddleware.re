open Express;
open Wonka;
open Wonka_types;

type result =
  | Response(Response.StatusCode.t, Js.Json.t)
  | Next;

type handler = (Request.t, Response.t) => sourceT(result);

let takeFirst = (arr: array('a)) => arr[0];

[@genType]
let middleware = (handler: handler) => {
  Middleware.from((next, req, res) =>
    handler(req, res)
    |> map((. result) =>
         switch (result) {
         | Response(statusCode, data) =>
           res |> Response.status(statusCode) |> Response.sendJson(data)
         | Next =>
           try(res |> next(Next.middleware)) {
           | e => res |> next(Next.error(e))
           }
         }
       )
    |> take(1)
    |> toArray
    |> takeFirst
  );
};
