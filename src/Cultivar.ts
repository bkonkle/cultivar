import {Source} from 'wonka'

export interface ExchangeInput<Operation, Result> {
  forward: ExchangeIO<Operation, Result>
}

export type Exchange<Operation, Result> = (
  input: ExchangeInput<Operation, Result>
) => ExchangeIO<Operation, Result>

export type ExchangeIO<Operation, Result> = (
  ops$: Source<Operation>
) => Source<Result>
