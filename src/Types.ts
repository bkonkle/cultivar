import {Exchange_t, Exchange_input} from 'Cultivar.gen'

export enum StatusCode {
  Ok = 'Ok',
  Created = 'Created',
  Accepted = 'Accepted',
  NonAuthoritativeInformation = 'NonAuthoritativeInformation',
  NoContent = 'NoContent',
  ResetContent = 'ResetContent',
  PartialContent = 'PartialContent',
  MultiStatus = 'MultiStatus',
  AleadyReported = 'AleadyReported',
  IMUsed = 'IMUsed',
  MultipleChoices = 'MultipleChoices',
  MovedPermanently = 'MovedPermanently',
  Found = 'Found',
  SeeOther = 'SeeOther',
  NotModified = 'NotModified',
  UseProxy = 'UseProxy',
  SwitchProxy = 'SwitchProxy',
  TemporaryRedirect = 'TemporaryRedirect',
  PermanentRedirect = 'PermanentRedirect',
  BadRequest = 'BadRequest',
  Unauthorized = 'Unauthorized',
  PaymentRequired = 'PaymentRequired',
  Forbidden = 'Forbidden',
  NotFound = 'NotFound',
  MethodNotAllowed = 'MethodNotAllowed',
  NotAcceptable = 'NotAcceptable',
  ProxyAuthenticationRequired = 'ProxyAuthenticationRequired',
  RequestTimeout = 'RequestTimeout',
  Conflict = 'Conflict',
  Gone = 'Gone',
  LengthRequired = 'LengthRequired',
  PreconditionFailed = 'PreconditionFailed',
  PayloadTooLarge = 'PayloadTooLarge',
  UriTooLong = 'UriTooLong',
  UnsupportedMediaType = 'UnsupportedMediaType',
  RangeNotSatisfiable = 'RangeNotSatisfiable',
  ExpectationFailed = 'ExpectationFailed',
  ImATeapot = 'ImATeapot',
  MisdirectedRequest = 'MisdirectedRequest',
  UnprocessableEntity = 'UnprocessableEntity',
  Locked = 'Locked',
  FailedDependency = 'FailedDependency',
  UpgradeRequired = 'UpgradeRequired',
  PreconditionRequired = 'PreconditionRequired',
  TooManyRequests = 'TooManyRequests',
  RequestHeaderFieldsTooLarge = 'RequestHeaderFieldsTooLarge',
  UnavailableForLegalReasons = 'UnavailableForLegalReasons',
  InternalServerError = 'InternalServerError',
  NotImplemented = 'NotImplemented',
  BadGateway = 'BadGateway',
  ServiceUnavailable = 'ServiceUnavailable',
  GatewayTimeout = 'GatewayTimeout',
  HttpVersionNotSupported = 'HttpVersionNotSupported',
  VariantAlsoNegotiates = 'VariantAlsoNegotiates',
  InsufficientStorage = 'InsufficientStorage',
  LoopDetected = 'LoopDetected',
  NotExtended = 'NotExtended',
  NetworkAuthenticationRequired = 'NetworkAuthenticationRequired',
}

type Nullable<T> = T | undefined | null | void

export type Input<
  Operation,
  Result,
  Context extends Nullable<Record<string, unknown>> = undefined
> = Exchange_input<Operation, Result, Context>

export type Exchange<
  A,
  B,
  Result,
  Context extends Nullable<Record<string, unknown>> = undefined
> = Exchange_t<A, B, Result, Context>
