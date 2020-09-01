import jwt from 'express-jwt'
import jwks from 'jwks-rsa'

import {fromMiddleware} from './ExpressMiddleware'

export interface Token {
  iss: string
  sub: string
  aud: string[]
  iat: number
  exp: number
  azp: string
  scope: string
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

export const jwtExchange = (config: {
  jwt: Omit<jwt.Options, 'secret'>
  jwks: jwks.ExpressJwtOptions
}) => fromMiddleware(jwtMiddleware(config))