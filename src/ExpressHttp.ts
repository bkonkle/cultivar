import {Request, Response} from 'express'
import {Source} from 'wonka'
import {StatusCode} from 'status-code-enum'

export type OperationContext = Record<string, unknown>

export interface Operation {
  http: {
    req: Request
    res: Response
  }
  context: OperationContext
}

export enum ResultKind {
  Forward,
  Respond,
  Reject,
}

export interface ForwardResult {
  kind: ResultKind.Forward
}

export interface RespondResult {
  kind: ResultKind.Respond
  status: StatusCode
  data: JSON
}

export interface RejectResult {
  kind: ResultKind.Reject
  error: Error
}

export type OperationResult = ForwardResult | RespondResult | RejectResult

export interface ExchangeInput {
  forward: ExchangeIO
}

export type Exchange = (input: ExchangeInput) => ExchangeIO

export type ExchangeIO = (ops$: Source<Operation>) => Source<OperationResult>

export const forward = (): ForwardResult => ({kind: ResultKind.Forward})

export const respond = (status: StatusCode, data: JSON): RespondResult => ({
  kind: ResultKind.Respond,
  status,
  data,
})

export const reject = (error: Error): RejectResult => ({
  kind: ResultKind.Reject,
  error,
})
