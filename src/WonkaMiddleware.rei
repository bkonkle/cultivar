type result =
  | Response(Express.Response.StatusCode.t, Js.Json.t)
  | Next;

type handler =
  (Express.Request.t, Express.Response.t) => Wonka_types.sourceT(result);

let middleware: handler => Express.Middleware.t;
