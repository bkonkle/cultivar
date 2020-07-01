import {pipe, fromValue, toPromise} from 'wonka'

import {StatusCode_fromString, notFound} from 'ExpressHttp.gen'

describe('ExpressHttp', () => {
  describe('notFound()', () => {
    it('is an operator that always returns a 404 response', async () => {
      await expect(
        pipe(fromValue({}), notFound({message: 'Test message'}), toPromise)
      ).resolves.toEqual(
        expect.arrayContaining([
          StatusCode_fromString('NotFound'),
          {message: 'Test message', success: false},
        ])
      )
    })
  })
})
