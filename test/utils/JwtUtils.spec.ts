import faker from 'faker'
import {verify, Secret, TokenExpiredError} from 'jsonwebtoken'
import {mocked} from 'ts-jest/utils'

import * as Utils from 'utils'

jest.mock('jsonwebtoken')

describe('utils/JwtUtils', () => {
  const mockVerify = mocked(verify)

  beforeEach(() => {
    jest.resetAllMocks()
  })

  describe('verify()', () => {
    const secret = {} as Secret
    const token = faker.random.alphaNumeric(10)
    const decoded = {header: {}, payload: {}}

    const options = {
      algorithms: ['RS256'],
      audience: faker.random.uuid(),
      clockTimestamp: undefined,
      clockTolerance: undefined,
      complete: undefined,
      issuer: undefined,
      ignoreExpiration: undefined,
      ignoreNotBefore: undefined,
      jwtid: undefined,
      nonce: undefined,
      subject: undefined,
      maxAge: undefined,
    }

    it('calls the verify function from jsonwebtoken', async () => {
      mockVerify.mockImplementation(
        (mockToken, mockSecret, mockOptions, cb) => {
          expect(mockToken).toEqual(token)
          expect(mockSecret).toEqual(secret)
          expect(mockOptions).toEqual(options)

          cb(undefined, decoded)
        }
      )

      const result = await Utils.verify({options}, secret, token)

      expect(mockVerify).toBeCalledTimes(1)
      expect(result).toEqual(decoded)
    })

    it('rejects error callbacks', async () => {
      const err = new TokenExpiredError('test', faker.date.recent())

      mockVerify.mockImplementation((_token, _secret, _options, cb) => {
        cb(err, {})
      })

      await expect(Utils.verify({}, secret, token)).rejects.toEqual([
        expect.arrayContaining(['JwtUtils-GraftCultivar.InvalidToken']),
        expect.objectContaining(err),
      ])

      expect(mockVerify).toBeCalledTimes(1)
    })
  })
})
