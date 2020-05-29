/**
 * The result of handling a request can either be a response, or a signal to move on to the next
 * Express middleware in the stack.
 */
type result =
  | Response(Express.Response.StatusCode.t, Js.Json.t)
  | Next;

/**
 * A handler takes an Express Request and Response, and returns a Wonka source that yields a result.
 */
type handler =
  (Express.Request.t, Express.Response.t) => Wonka_types.sourceT(result);

/**
 * Creates Express middleware from a handler.
 */
let middleware: handler => Express.Middleware.t;
