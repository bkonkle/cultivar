import {
  GraphQLOptions,
  HttpQueryError,
  runHttpQuery,
  convertNodeHttpToRequest,
} from 'apollo-server-core'
import {fromPromise, mergeMap} from 'wonka'

import {Exchange} from '../../Cultivar'
import {Operation, OperationResult, respond, reject} from '../../ExpressHttp'
import StatusCode from 'status-code-enum'

export interface GraphQLOptionsFunction {
  (operation: Operation): Promise<GraphQLOptions>
}

type Options = GraphQLOptions | GraphQLOptionsFunction

export const runWithApollo = async (
  operation: Operation,
  options: Options
): Promise<OperationResult> => {
  const {
    http: {req, res},
  } = operation

  try {
    const {graphqlResponse, responseInit} = await runHttpQuery([req, res], {
      method: req.method,
      options,
      query: req.method === 'POST' ? req.body : req.query,
      request: convertNodeHttpToRequest(req),
    })

    if (responseInit.headers) {
      for (const [name, value] of Object.entries(responseInit.headers)) {
        res.setHeader(name, value)
      }
    }

    return respond(StatusCode.SuccessOK, graphqlResponse)
  } catch (err) {
    if (err.name !== 'HttpQueryError') {
      return reject(err)
    }

    const error: HttpQueryError = err

    if (error.headers) {
      Object.entries(error.headers).forEach(([name, value]) => {
        res.setHeader(name, value)
      })
    }

    return respond(error.statusCode as StatusCode, error.message)
  }
}

export const graphqlExchange = (options: Options): Exchange => () =>
  mergeMap((operation) => fromPromise(runWithApollo(operation, options)))
