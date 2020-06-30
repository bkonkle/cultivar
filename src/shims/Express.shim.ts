import {Request, Response, RequestHandler} from 'express'
import {StatusCode} from '../Types'

export type Middleware_t = RequestHandler
export type Request_t = Request
export type Response_t = Response
export type Response_StatusCode_t = StatusCode
