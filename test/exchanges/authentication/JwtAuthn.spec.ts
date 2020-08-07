import faker from 'faker'
import {pipe, toPromise, fromValue} from 'wonka'
import {mocked} from 'ts-jest/utils'

import {JwtAuthn} from 'exchanges'
import * as Authn from '../../../src/exchanges/authentication/Authn.bs'
import {makeRequest, makeResponse} from '../../TestUtils'

jest.mock('jsonwebtoken')

describe('exchanges/authentication/JwtAuthn', () => {
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

  describe('hasAuthInAccessControl()', () => {
    it('determines if the access-control-request-headers header includes authorization', () => {
      const req = makeRequest({
        headers: {
          ['access-control-request-headers']: 'authorization',
        },
      })

      const result = JwtAuthn.hasAuthInAccessControl(req)

      expect(result).toBe(true)
    })

    it('returns false otherwise', () => {
      const req = makeRequest({
        headers: {
          ['access-control-request-headers']: 'other',
        },
      })

      const result = JwtAuthn.hasAuthInAccessControl(req)

      expect(result).toBe(false)
    })
  })

  describe('skipOptions()', () => {
    it('skips OPTIONS requests if hasAuthInAccessControl', async () => {
      const req = makeRequest({
        method: 'OPTIONS',
        headers: {
          ['access-control-request-headers']: 'authorization',
        },
      })

      const res = makeResponse()

      const event = {http: {req, res}}

      const result = await pipe(
        fromValue(Authn.Authentication.fromHttp(event)),
        JwtAuthn.skipOptions,
        toPromise
      )

      expect(result).toEqual(
        expect.arrayContaining(Authn.Authentication.toAnonymous(event))
      )
    })

    it("doesn't skip OPTIONS requests if not hasAuthInAccessControl", async () => {
      const req = makeRequest({
        method: 'OPTIONS',
        headers: {
          ['access-control-request-headers']: 'other',
        },
      })

      const res = makeResponse()

      const event = {http: {req, res}}

      const result = await pipe(
        fromValue(Authn.Authentication.fromHttp(event)),
        JwtAuthn.skipOptions,
        toPromise
      )

      expect(result).toEqual(
        expect.arrayContaining(Authn.Authentication.toAnonymous(event))
      )
    })

    it('throws errors for unexpected methods', async () => {
      const req = makeRequest({
        method: 'INVALID',
      })

      const res = makeResponse()

      await expect(
        pipe(
          fromValue(Authn.Authentication.fromHttp({http: {req, res}})),
          JwtAuthn.skipOptions,
          toPromise
        )
      ).rejects.toEqual(
        expect.arrayContaining([
          'Express.Request.method_ Unexpected method: INVALID',
        ])
      )
    })

    it('leaves other requests alone', async () => {
      const req = makeRequest({
        method: 'OPTIONS',
        headers: {
          ['access-control-request-headers']: 'other',
        },
      })

      const res = makeResponse()

      const event = {http: {req, res}}

      const result = await pipe(
        fromValue(Authn.Authentication.fromHttp(event)),
        JwtAuthn.skipOptions,
        toPromise
      )

      expect(result).toEqual(
        expect.arrayContaining(Authn.Authentication.toAnonymous(event))
      )
    })
  })
})
