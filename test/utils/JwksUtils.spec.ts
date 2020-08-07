import faker from 'faker'
import jwksRsa, {JwksClient} from 'jwks-rsa'
import {mocked} from 'ts-jest/utils'

import * as Utils from 'utils'
import {makeRequest} from '../TestUtils'

jest.mock('jwks-rsa')

describe('utils/JwksUtils', () => {
  const jwksRsaMock = mocked(jwksRsa, true)

  const client = mocked<JwksClient>(
    {
      getKeys: jest.fn(),
      getSigningKey: jest.fn(),
      getSigningKeys: jest.fn(),
    },
    true
  )

  beforeEach(() => {
    jest.resetAllMocks()
  })

  describe('getJwksSecret()', () => {
    const publicKey = faker.random.uuid()

    const kid = faker.random.uuid()

    const signingKey = {
      kid,
      nbf: faker.random.uuid(),
      getPublicKey: () => publicKey,
      publicKey,
    }

    const header = {
      alg: 'RS256',
      typ: undefined,
      kid,
      jku: undefined,
      x5u: undefined,
      x5t: undefined,
    }

    it('returns a Promise for a secret', async () => {
      const req = makeRequest()

      jwksRsaMock.mockReturnValue(client)

      client.getSigningKey.mockImplementation((signingKid, cb) => {
        expect(signingKid).toEqual(kid)

        cb(undefined, signingKey)
      })

      const config = ({test: true} as unknown) as Utils.Client_options

      const payload = {} as Utils.payloadJs

      const result = await Utils.getJwksSecret(config)(req, header, payload)

      expect(jwksRsaMock).toBeCalledTimes(1)
      expect(jwksRsaMock).toBeCalledWith(config)
      expect(client.getSigningKey).toBeCalledTimes(1)
      expect(result).toEqual(publicKey)
    })

    it("returns null if there is no 'kid'", async () => {
      const req = makeRequest()

      jwksRsaMock.mockReturnValue(client)

      const config = ({test: true} as unknown) as Utils.Client_options

      const payload = {} as Utils.payloadJs

      const result = await Utils.getJwksSecret(config)(
        req,
        {...header, kid: undefined},
        payload
      )

      expect(jwksRsaMock).toBeCalledTimes(1)
      expect(jwksRsaMock).toBeCalledWith(config)
      expect(client.getSigningKey).not.toBeCalled()
      expect(result).toEqual(null)
    })
  })
})
