/* eslint-disable @typescript-eslint/ban-ts-comment */
import {Request, Response} from 'express'
import expressJwt from 'express-jwt'
import faker from 'faker'
import http from 'http'
import jwt from 'jsonwebtoken'

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

export interface Token {
  iss: string
  sub: string
  aud: string[]
  iat: number
  exp: number
  azp: string
  scope: string
}

const eJwt = (expressJwt as unknown) as jest.Mock<typeof expressJwt>

export const mockJwt = <T extends Token>(token: T) => {
  eJwt.mockImplementation(
    // @ts-ignore
    (): expressJwt.RequestHandler => (req: IncomingMessage, _res, next) => {
      req.user = token

      next()
    }
  )

  return eJwt
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
