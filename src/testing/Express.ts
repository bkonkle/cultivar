import {Request, Response} from 'express'
import faker from 'faker'
import http from 'http'
import jwt from 'jsonwebtoken'

import {Token} from '../express/JwtMiddleware'

export function makeRequest(extra: Partial<Request> = {}): Request {
  const req = {
    ...Object.create(http.IncomingMessage.prototype),
    ...extra,
    headers: extra.headers || {},
  }

  req.header = (key: keyof typeof req.headers) => req.headers[key]
  req.get = req.header

  return req
}

export function makeResponse(extra: Partial<Response> = {}): Response {
  const res: Response = {
    ...Object.create(http.OutgoingMessage.prototype),
    ...extra,
    status: jest.fn(),
    json: jest.fn(),
    send: jest.fn(),
  }

  return res
}

export const getToken = (): Token => ({
  sub: faker.random.alphaNumeric(10),
  iat: faker.random.number(10),
  aud: ['localhost'],
  iss: faker.random.alphaNumeric(10),
  exp: faker.random.number(10),
  azp: faker.random.alphaNumeric(10),
  scope: faker.random.alphaNumeric(10),
})

export const encodeToken = <T extends Token>(token: T) =>
  jwt.sign(token, 'test-secret', {algorithm: 'HS256'})
