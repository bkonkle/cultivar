import {pipe, map, fromValue, toPromise} from 'wonka'
import {Exchange, Input, bind} from 'index'

describe('Cultivar', () => {
  describe('Exchange', () => {
    describe('bind()', () => {
      const double: Exchange<number, number, string> = (input) => (source) =>
        pipe(
          source,
          map((a) => a * 2),
          input.forward
        )

      const toString: Exchange<number, string, string> = (input) => (source) =>
        pipe(
          source,
          map((a) => `${a}`),
          input.forward
        )

      it('connects two exchanges together', async () => {
        const exchange = bind(double, toString)

        const input: Input<string, string> = {
          forward: jest.fn((a) => a),
          context: undefined,
        }

        await expect(toPromise(exchange(input)(fromValue(2)))).resolves.toEqual(
          '4'
        )

        expect(input.forward).toBeCalledTimes(1)
      })
    })
  })
})
