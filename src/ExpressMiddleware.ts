import {Handler, Request} from 'express'
import {fromValue, pipe, map, forEach} from 'wonka'

import {
  Exchange,
  ResultKind,
  OperationContext,
  handleResult,
} from './ExpressHttp'

export type GetContext = (req: Request) => OperationContext

export interface MiddlewareOptions {
  exchange: Exchange
  getContext?: GetContext
}

export const getEmptyContext = (_req: Request): OperationContext => ({})

export const createMiddleware = ({
  exchange,
  getContext = getEmptyContext,
}: MiddlewareOptions): Handler => (req, res, next) => {
  pipe(
    fromValue({http: {req, res}, context: getContext(req)}),
    exchange({
      forward: map(() => ({kind: ResultKind.Forward})),
    }),
    forEach(
      handleResult({
        Forward: () => {
          try {
            next()
          } catch (err) {
            next(err)
          }
        },
        Respond: (result) => {
          res.status(result.status)
          res.json(result.data)
        },
        Reject: (result) => {
          next(result.error)
        },
      })
    )
  )
}
