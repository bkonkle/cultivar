import {
  GraphQLOptions,
  HttpQueryError,
  runHttpQuery,
  convertNodeHttpToRequest,
} from 'apollo-server-core'
import {Application} from 'express'
import {fromPromise, mergeMap, pipe} from 'wonka'
import StatusCode from 'status-code-enum'

import {
  Exchange,
  Operation,
  OperationResult,
  createMiddleware,
  respond,
  reject,
} from '../express'
import {ApolloServer, ApolloServerExpressConfig} from './ApolloServer'

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

export const apolloExchange = (apollo: ApolloServer): Exchange => () => (
  ops$
) =>
  pipe(
    ops$,
    mergeMap(({http: {req, res}, ...rest}) =>
      fromPromise(
        Promise.all([
          // Include the operation
          Promise.resolve({http: {req, res}, ...rest}),

          // And the options derived by Apollo Server
          apollo.createGraphQLServerOptions(req, res),
        ])
      )
    ),
    mergeMap(([operation, options]) =>
      fromPromise(runWithApollo(operation, options))
    )
  )
