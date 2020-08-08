import {Handler} from 'express'

import {ApolloServer, graphqlExchange} from '../../src/exchanges/graphql'
import {createMiddleware} from '../../src/express'

export const middleware = (apollo: ApolloServer): Handler =>
  createMiddleware({
    exchange: graphqlExchange(apollo),
  })
