import chalk from 'chalk'
import express, {Application} from 'express'
import http from 'http'
import morgan from 'morgan'

import {ApolloServer, gql} from '../../src/exchanges/graphql/ApolloServer'
import * as App from './App'

const typeDefs = gql`
  type Query {
    hello: String
  }
`

const resolvers = {
  Query: {
    hello: () => 'Hello world!',
  },
}

export async function start(): Promise<void> {
  const {NODE_ENV = 'development'} = process.env

  const isDev = NODE_ENV === 'development'

  const app = express()
    .disable('x-powered-by')
    .use(morgan(isDev ? 'dev' : 'combined'))

  const apollo = new ApolloServer({typeDefs, resolvers})
  await apollo.applyMiddleware({app})

  app.use(App.middleware)

  run(app, 3000)
}

export function run(app: Application, port: number, baseUrl?: string): void {
  const baseUrlStr = baseUrl ? `at ${baseUrl}` : ''
  const portStr = chalk.yellow(port.toString())

  const server = http.createServer(app)

  server.listen(port, () => {
    console.log(chalk.cyan(`> Started API on port ${portStr}${baseUrlStr}`))
  })

  server.on('close', () => {
    console.log(chalk.cyan(`> API shutting down`))
  })
}

if (require.main === module) {
  start().catch(console.error).finally(process.exit)
}
