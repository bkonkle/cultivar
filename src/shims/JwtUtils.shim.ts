import {Secret} from 'jsonwebtoken'

import * as JwtUtils from '../utils/JwtUtils.gen'

export type stringOrNumberJs = string | number

export type stringOrArrayJs = string | string[]

export type stringOrRegexJs = string | RegExp

export type stringOrRegexArrayJs = stringOrRegexJs | stringOrRegexJs[]

export type stringOrBufferJs = string | Buffer

export type secretJs = Secret

export type payloadJs<
  T extends Record<string, unknown> = Record<string, unknown>
> = string | Buffer | T

export type header = JwtUtils.header
export type token = JwtUtils.token
export type verifyOptions = JwtUtils.verifyOptions
export type verify = typeof JwtUtils.verify
