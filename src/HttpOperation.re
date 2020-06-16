open Express;

/**
 * An HttpOperation is composed of an Express Request and Response.
 */
type t = {
  req: Request.t,
  res: Response.t,
};

/**
 * The event contains the Operation details.
 */
type event = {http: t};

/**
 * The result of handling an event can be a signal to respond with json, to move on to
 * the next Express middleware in the stack, or to handle an error.
 */
type result =
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Next
  | Error(exn);
