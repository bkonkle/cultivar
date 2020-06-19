open Cultivar;
open Express;

/**
 * An ExpressHttp event is composed of an Express Request and Response.
 */
type event = {
  req: Request.t,
  res: Response.t,
};

/**
 * The operation contains the event details.
 */
type operation = {http: event};

/**
 * The result of handling an event can be a signal to respond with json, to move on to
 * the next Express middleware in the stack, or to handle an error.
 */
type result =
  | Forward
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Reject(exn);

module Exchange = {
  type input('context) = Exchange.input(operation, result, 'context);

  type t('context) = Exchange.t(operation, result, 'context);
};
