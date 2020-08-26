import {Handler, Request} from 'express'
import jwt from 'express-jwt'
import jwks from 'jwks-rsa'
import {fromValue, pipe, map, forEach} from 'wonka'

import {Exchange} from '../Cultivar'
import {ResultKind, OperationContext, handleResult} from './ExpressHttp'

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
            next('route')
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

export const jwtMiddleware = (config: {
  jwt: Omit<jwt.Options, 'secret'>
  jwks: jwks.ExpressJwtOptions
}) =>
  jwt({
    ...config.jwt,
    secret: jwks.expressJwtSecret({
      cache: true,
      rateLimit: true,
      jwksRequestsPerMinute: 5,
      ...config.jwks,
    }),
  })
