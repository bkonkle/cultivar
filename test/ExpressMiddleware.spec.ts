import {mocked} from 'ts-jest/utils'
import {NextFunction} from 'express'

import {ExpressMiddleware, ExpressHttp, StatusCode} from 'index'
import {makeRequest, makeResponse} from './TestUtils'

const {middleware} = ExpressMiddleware
const {respond, reject} = ExpressHttp

describe('ExpressMiddleware', () => {
  describe('middleware()', () => {
    it('handles "forward" calls by calling the next function with "route"', () => {
      const exchange: ExpressHttp.Exchange_t<unknown> = ({forward}) => (op) =>
        forward(op)

      const res = makeResponse()
      const app = middleware({}, exchange)

      const next = mocked<NextFunction>(jest.fn())

      app(makeRequest(), res, next)

      expect(next).toBeCalledTimes(1)
      expect(next).toBeCalledWith('route')
    })

    it('handles responses by sending json', () => {
      const exchange: ExpressHttp.Exchange_t<unknown> = () => (_op) =>
        respond(StatusCode.Ok, {success: true})

      const res = makeResponse()
      const app = middleware({}, exchange)

      const next = mocked<NextFunction>(jest.fn())

      app(makeRequest(), res, next)

      expect(next).not.toBeCalled()

      expect(res.status).toBeCalledTimes(1)
      expect(res.status).toBeCalledWith(200)

      expect(res.json).toBeCalledTimes(1)
      expect(res.json).toBeCalledWith({success: true})
    })

    it('handles rejection by calling "next" with the error parameter', () => {
      const error = new Error('This is an error')
      const exchange: ExpressHttp.Exchange_t<unknown> = () => (_op) =>
        reject(error)

      const res = makeResponse()
      const app = middleware({}, exchange)

      const next = mocked<NextFunction>(jest.fn())

      app(makeRequest(), res, next)

      expect(next).toBeCalledTimes(1)
      expect(next).toBeCalledWith(error)

      expect(res.status).not.toBeCalled()
      expect(res.json).not.toBeCalled()
    })
  })
})
