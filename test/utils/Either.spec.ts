import {pipe, fromValue, toPromise, map} from 'wonka'

import {either, right, left} from 'utils'

describe('utils/Either', () => {
  describe('either()', () => {
    it('uses the right operator when the test returns Right', async () => {
      const rightOp = jest.fn((_: string) => 'result')
      const leftOp = jest.fn((_: string) => 'result')

      const operator = either({
        test: right,
        left: map(leftOp),
        right: map(rightOp),
      })

      const source = fromValue('test')

      await expect(pipe(source, operator, toPromise)).resolves.toEqual('result')

      expect(rightOp).toBeCalledTimes(1)
      expect(leftOp).not.toBeCalled()
    })

    it('uses the left operator when the test returns Left', async () => {
      const rightOp = jest.fn((_: string) => 'result')
      const leftOp = jest.fn((_: string) => 'result')

      const operator = either({
        test: left,
        left: map(leftOp),
        right: map(rightOp),
      })

      const source = fromValue('test')

      await expect(pipe(source, operator, toPromise)).resolves.toEqual('result')

      expect(leftOp).toBeCalledTimes(1)
      expect(rightOp).not.toBeCalled()
    })
  })
})
