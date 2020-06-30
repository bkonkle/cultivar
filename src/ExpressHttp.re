open Cultivar;
open Express;
open Wonka;

/**
 * An ExpressHttp event is composed of an Express Request and Response.
 */
[@genType]
type event = {
  req: Request.t,
  res: Response.t,
};

/**
 * The operation contains the event details.
 */
[@genType]
type operation = {http: event};

/**
 * The result of handling an event can be a signal to respond with json, to move on to
 * the next Express middleware in the stack, or to handle an error.
 */
[@genType]
type result =
  | Forward
  | Respond(Response.StatusCode.t, Js.Json.t)
  | Reject(exn);

module Exchange = {
  [@genType]
  type input('context) = Exchange.input(operation, result, 'context);

  [@genType]
  type t('context) = Exchange.t(operation, result, 'context);
};

[@genType]
let notFound = (~message: Js.Option.t(string)=?, _input) =>
  Js.Json.(
    map((. _event) =>
      Respond(
        Response.StatusCode.NotFound,
        JsonUtils.toJson([
          ("success", boolean(false)),
          (
            "message",
            string(message |> Js.Option.getWithDefault("Not found")),
          ),
        ]),
      )
    )
  );

/**
 * Turn a string based on the StatusCode enum to a Response.StatusCode.t value.
 *
 *   WARNING: Unsafe - will throw an exception if not given a value from the enum.
 */
[@genType]
let statusCode = (statusCode: string): Response.StatusCode.t =>
  switch (statusCode) {
  | "Ok" => Ok
  | "Created" => Created
  | "Accepted" => Accepted
  | "NonAuthoritativeInformation" => NonAuthoritativeInformation
  | "NoContent" => NoContent
  | "ResetContent" => ResetContent
  | "PartialContent" => PartialContent
  | "MultiStatus" => MultiStatus
  | "AleadyReported" => AleadyReported
  | "IMUsed" => IMUsed
  | "MultipleChoices" => MultipleChoices
  | "MovedPermanently" => MovedPermanently
  | "Found" => Found
  | "SeeOther" => SeeOther
  | "NotModified" => NotModified
  | "UseProxy" => UseProxy
  | "SwitchProxy" => SwitchProxy
  | "TemporaryRedirect" => TemporaryRedirect
  | "PermanentRedirect" => PermanentRedirect
  | "BadRequest" => BadRequest
  | "Unauthorized" => Unauthorized
  | "PaymentRequired" => PaymentRequired
  | "Forbidden" => Forbidden
  | "NotFound" => NotFound
  | "MethodNotAllowed" => MethodNotAllowed
  | "NotAcceptable" => NotAcceptable
  | "ProxyAuthenticationRequired" => ProxyAuthenticationRequired
  | "RequestTimeout" => RequestTimeout
  | "Conflict" => Conflict
  | "Gone" => Gone
  | "LengthRequired" => LengthRequired
  | "PreconditionFailed" => PreconditionFailed
  | "PayloadTooLarge" => PayloadTooLarge
  | "UriTooLong" => UriTooLong
  | "UnsupportedMediaType" => UnsupportedMediaType
  | "RangeNotSatisfiable" => RangeNotSatisfiable
  | "ExpectationFailed" => ExpectationFailed
  | "ImATeapot" => ImATeapot
  | "MisdirectedRequest" => MisdirectedRequest
  | "UnprocessableEntity" => UnprocessableEntity
  | "Locked" => Locked
  | "FailedDependency" => FailedDependency
  | "UpgradeRequired" => UpgradeRequired
  | "PreconditionRequired" => PreconditionRequired
  | "TooManyRequests" => TooManyRequests
  | "RequestHeaderFieldsTooLarge" => RequestHeaderFieldsTooLarge
  | "UnavailableForLegalReasons" => UnavailableForLegalReasons
  | "InternalServerError" => InternalServerError
  | "NotImplemented" => NotImplemented
  | "BadGateway" => BadGateway
  | "ServiceUnavailable" => ServiceUnavailable
  | "GatewayTimeout" => GatewayTimeout
  | "HttpVersionNotSupported" => HttpVersionNotSupported
  | "VariantAlsoNegotiates" => VariantAlsoNegotiates
  | "InsufficientStorage" => InsufficientStorage
  | "LoopDetected" => LoopDetected
  | "NotExtended" => NotExtended
  | "NetworkAuthenticationRequired" => NetworkAuthenticationRequired
  | _ => raise(Not_found)
  };

[@genType]
let forward = () => fromValue(Forward);

[@genType]
let respond = (status: string, json: Js.Json.t) =>
  Respond(statusCode(status), json) |> fromValue;

[@genType]
let reject = (error: exn) => Reject(error) |> fromValue;
