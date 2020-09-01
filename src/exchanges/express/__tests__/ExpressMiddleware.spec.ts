import {mocked} from 'ts-jest/utils'
import {NextFunction} from 'express'
import StatusCode from 'status-code-enum'

import {Express} from '../../../utils/testing'
import {createMiddleware} from '../ExpressMiddleware'
import {Exchange, respond, reject} from '../HttpExchange'
import {map} from 'wonka'

describe('ExpressMiddleware', () => {
  describe('middleware()', () => {
    it('handles "forward" calls by calling the next function with "route"', () => {
      const exchange: Exchange = ({forward}) => (ops$) => forward(ops$)

      const res = Express.makeResponse()
      const middleware = createMiddleware({exchange})

      const next = mocked<NextFunction>(jest.fn())

      middleware(Express.makeRequest(), res, next)

      expect(next).toBeCalledTimes(1)
      expect(next).toBeCalledWith('route')
    })

    it('handles responses by sending json', () => {
      const exchange: Exchange = () =>
        map((_op) => respond(StatusCode.SuccessOK, {success: true}))

      const res = Express.makeResponse()
      const middleware = createMiddleware({exchange})

      const next = mocked<NextFunction>(jest.fn())

      middleware(Express.makeRequest(), res, next)

      expect(next).not.toBeCalled()

      expect(res.status).toBeCalledTimes(1)
      expect(res.status).toBeCalledWith(200)

      expect(res.json).toBeCalledTimes(1)
      expect(res.json).toBeCalledWith({success: true})
    })

    it('handles rejection by calling "next" with the error parameter', () => {
      const error = new Error('This is an error')
      const exchange: Exchange = () => map((_op) => reject(error))

      const res = Express.makeResponse()
      const middleware = createMiddleware({exchange})

      const next = mocked<NextFunction>(jest.fn())

      middleware(Express.makeRequest(), res, next)

      expect(next).toBeCalledTimes(1)
      expect(next).toBeCalledWith(error)

      expect(res.status).not.toBeCalled()
      expect(res.json).not.toBeCalled()
    })
  })
})
