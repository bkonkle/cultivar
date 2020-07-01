import {pipe, map, fromValue, toPromise} from 'wonka'
import {Exchange, bind} from 'index'

describe('Cultivar', () => {
  describe('Exchange', () => {
    describe('bind()', () => {
      const double: Exchange<number, number> = (input) => (source) =>
        pipe(
          source,
          map((a) => a * 2),
          input.forward
        )
      const toString: Exchange<number, string> = (input) => (source) =>
        pipe(
          source,
          map((a) => `${a}`),
          input.forward
        )

      it('connects two exchanges together through the forward operator', async () => {
        const exchange = bind(double, toString)
        const input = {forward: jest.fn(), context: undefined}

        await expect(
          pipe(fromValue(2), exchange(input), toPromise)
        ).resolves.toEqual({})
      })
    })
  })
})
