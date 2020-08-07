import faker from 'faker'
import {pipe, toPromise, fromValue} from 'wonka'
import {mocked} from 'ts-jest/utils'

import {AuthnExchange, JwtAuthn} from 'exchanges'
import {makeRequest, makeResponse} from '../../TestUtils'

jest.mock('jsonwebtoken')

describe('exchanges/authentication/AuthnExchange', () => {
  beforeEach(() => {
    jest.resetAllMocks()
  })

  const forward = jest.fn((a) => a)
  const input = {forward, context: {}}

  const getSecret = mocked<JwtAuthn.getSecret>(jest.fn())
  const user = {id: faker.random.uuid()}

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

  describe('jwtAuthentication()', () => {
    it('test', async () => {
      const exchange = AuthnExchange.jwtAuthentication({
        getSecret,
        toUser: (_op, _token) => user,
        verifyOptions: options,
      })

      const req = makeRequest()
      const res = makeResponse()

      const result = await pipe(
        fromValue({http: {req, res}}),
        exchange(input),
        toPromise
      )

      expect(result).toEqual({})
    })
  })
})
