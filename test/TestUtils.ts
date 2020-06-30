import http from 'http'
import {Request, Response} from 'express'

export function makeRequest(extra: Partial<Request> = {}): Request {
  const req = {
    ...Object.create(http.IncomingMessage.prototype),
    ...extra,
    headers: extra.headers || {},
  }

  req.header = (key: keyof typeof req.headers) => req.headers[key]

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
