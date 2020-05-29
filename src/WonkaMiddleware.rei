open Express;
open Wonka_types;

/**
 * An event is composed of an Express Request and Response.
 */
type event = (Request.t, Response.t);

/**
 * The result of handling an event can either be a signal to respond, or to move on to the next
 * Express middleware in the stack.
 */
type result =
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Next;

/**
 * A handler takes an event and returns a Wonka source that yields a result.
 */
type handler = operatorT(event, result);

/**
 * Creates Express middleware from a handler.
 */
let middleware: handler => Middleware.t;
