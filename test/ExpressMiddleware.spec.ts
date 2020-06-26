import {mocked} from 'ts-jest'
import {Source} from 'wonka'
import {Request, Response, NextFunction} from 'express'

import {middleware} from '../src/ExpressMiddleware.gen'
import {operation} from '../src/ExpressHttp.gen'

describe('ExpressMiddleware', () => {
  describe('middleware()', () => {
    it('handles "forward" calls by calling the next function with "route"', () => {
      const exchange = ({forward}) => (a: Source<operation>) => forward(a)
      const app = middleware({getContext: (_req) => ({})}, exchange)

      const req: Partial<Request> = {}
      const res: Partial<Response> = {}
      const next = mocked<NextFunction>(jest.fn())

      app(<Request>req, <Response>res, next)

      expect(next).toBeCalledTimes(1)
      expect(next).toBeCalledWith('route')
    })
  })
})
