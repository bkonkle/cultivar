open Express;
open Wonka_types;

/**
 * An event is composed of an Express Request and Response.
 */
type event = {
  req: Request.t,
  res: Response.t,
};

/**
 * The result of handling an event can either be a signal to respond, or to move on to the next
 * Express middleware in the stack.
 */
type result =
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Next;

/**
 * A handler transforms an event into a result.
 */
type handler = operatorT(event, result);

/**
 * Turn a list of keys and values into a JSON object.
 */
let toJson: list((Js.Dict.key, Js.Json.t)) => Js.Json.t;

/**
 * Creates Express middleware from a handler.
 */
let middleware: handler => Middleware.t;
