import {mocked} from 'ts-jest'
import {Source} from 'wonka'
import {Request, Response, NextFunction} from 'express'

import {middleware} from '../src/ExpressMiddleware.gen'
import {respond, reject} from '../src/ExpressHttp.gen'
import {operation} from '../src/ExpressHttp.gen'
import {StatusCode} from '../src/Types'

describe('ExpressMiddleware', () => {
  const req: Partial<Request> = {}
  const res: Partial<Response> = {
    status: jest.fn(),
    json: jest.fn(),
    send: jest.fn(),
  }

  beforeEach(() => {
    jest.resetAllMocks()
  })

  describe('middleware()', () => {
    it('handles "forward" calls by calling the next function with "route"', () => {
      const exchange = ({forward}) => (op: Source<operation>) => forward(op)

      const app = middleware({getContext: (_req) => ({})}, exchange)

      const next = mocked<NextFunction>(jest.fn())

      app(<Request>req, <Response>res, next)

      expect(next).toBeCalledTimes(1)
      expect(next).toBeCalledWith('route')
    })

    it('handles responses by sending json', () => {
      const exchange = () => (_op: Source<operation>) =>
        respond(StatusCode.Ok, {success: true})

      const app = middleware({getContext: (_req) => ({})}, exchange)

      const next = mocked<NextFunction>(jest.fn())

      app(<Request>req, <Response>res, next)

      expect(next).not.toBeCalled()

      expect(res.status).toBeCalledTimes(1)
      expect(res.status).toBeCalledWith(200)

      expect(res.json).toBeCalledTimes(1)
      expect(res.json).toBeCalledWith({success: true})
    })

    it('handles rejection by calling "next" with the error parameter', () => {
      const error = new Error('This is an error')
      const exchange = () => (_op: Source<operation>) => reject(error)

      const app = middleware({getContext: (_req) => ({})}, exchange)

      const next = mocked<NextFunction>(jest.fn())

      app(<Request>req, <Response>res, next)

      expect(next).toBeCalledTimes(1)
      expect(next).toBeCalledWith(error)

      expect(res.status).not.toBeCalled()
      expect(res.json).not.toBeCalled()
    })
  })
})
