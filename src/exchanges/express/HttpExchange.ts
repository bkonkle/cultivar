import {Request, Response} from 'express'
import {StatusCode} from 'status-code-enum'

import * as Cultivar from '../../Cultivar'

export type OperationContext = Record<string, unknown>

export interface Operation {
  http: {
    req: Request
    res: Response
  }
  context: OperationContext
}

export enum ResultKind {
  Forward = 'Forward',
  Respond = 'Respond',
  Reject = 'Reject',
}

export interface ForwardResult {
  kind: ResultKind.Forward
}

export interface RespondResult {
  kind: ResultKind.Respond
  status: StatusCode
  data: string | Record<string, unknown>
}

export interface RejectResult {
  kind: ResultKind.Reject
  error: Error
}

export type OperationResult = ForwardResult | RespondResult | RejectResult

export type ExchangeInput = Cultivar.ExchangeInput<Operation, OperationResult>

export type Exchange = Cultivar.Exchange<Operation, OperationResult>

export type ExchangeIO = Cultivar.ExchangeIO<Operation, OperationResult>

export const forward = (): ForwardResult => ({kind: ResultKind.Forward})

export const respond = (
  status: RespondResult['status'],
  data: RespondResult['data']
): RespondResult => ({
  kind: ResultKind.Respond,
  status,
  data,
})

export const reject = (error: RejectResult['error']): RejectResult => ({
  kind: ResultKind.Reject,
  error,
})

export const handleResult = <T = void>(handlers: {
  Forward: (result: ForwardResult) => T
  Respond: (result: RespondResult) => T
  Reject: (result: RejectResult) => T
}) => (result: OperationResult): T => {
  switch (result.kind) {
    case ResultKind.Forward:
      return handlers.Forward(result)
    case ResultKind.Respond:
      return handlers.Respond(result)
    case ResultKind.Reject:
      return handlers.Reject(result)
  }
}
