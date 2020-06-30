import {mocked} from 'ts-jest'
import {Source} from 'wonka'
import {Request, Response, NextFunction} from 'express'

import {middleware} from '../src/ExpressMiddleware.gen'
import {respond} from '../src/ExpressHttp.gen'
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
        respond(StatusCode.SuccessOK, {success: true})

      const app = middleware({getContext: (_req) => ({})}, exchange)

      const next = mocked<NextFunction>(jest.fn())

      app(<Request>req, <Response>res, next)

      expect(next).not.toBeCalled()

      expect(res.status).toBeCalledTimes(1)
      expect(res.status).toBeCalledWith({})

      expect(res.send).toBeCalledTimes(1)
      expect(res.send).toBeCalledWith({})
    })
  })
})
