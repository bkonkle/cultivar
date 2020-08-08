import {Source} from 'wonka'

import {Operation, OperationResult} from './ExpressHttp'

export interface ExchangeInput {
  forward: ExchangeIO
}

export type Exchange = (input: ExchangeInput) => ExchangeIO

export type ExchangeIO = (ops$: Source<Operation>) => Source<OperationResult>
