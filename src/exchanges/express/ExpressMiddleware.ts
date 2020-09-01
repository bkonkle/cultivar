import {Handler, Request, NextFunction} from 'express'
import {pipe, map, fromValue, forEach, mergeMap, makeSubject} from 'wonka'

import {
  Exchange,
  OperationContext,
  OperationResult,
  handleResult,
  forward,
  reject,
} from './HttpExchange'

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
      forward: map(forward),
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

          typeof result.data === 'string'
            ? res.send(result.data)
            : res.json(result.data)
        },
        Reject: (result) => {
          next(result.error)
        },
      })
    )
  )
}

export const fromMiddleware = (middleware: Handler): Exchange => () => (ops$) =>
  pipe(
    ops$,
    mergeMap((op) => {
      const {
        http: {req, res},
      } = op

      const subject = makeSubject<OperationResult>()

      const next: NextFunction = (err: Error | 'router') => {
        if (err === 'router') {
          subject.next(
            reject(new Error('Unable to support next("router") calls.'))
          )
        } else if (err) {
          subject.next(reject(err))
        } else {
          subject.next(forward())
        }

        // Override the types a bit to get TypeScript to accept the complete marker
        subject.next((subject.complete as unknown) as OperationResult)
      }

      middleware(req, res, next)

      return subject.source
    })
  )
