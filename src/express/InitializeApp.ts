import chalk from 'chalk'
import {ApolloServer} from 'apollo-server-express'
import {ApolloServerExpressConfig} from 'apollo-server-express/dist/ApolloServer'
import express, {Application, RequestHandler} from 'express'
import http from 'http'
import morgan from 'morgan'
import {createConnection, ConnectionOptions} from 'typeorm'
import jwt from 'express-jwt'
import jwks from 'jwks-rsa'

import {jwtMiddleware} from './JwtMiddleware'

export interface InitOptions {
  label: string
  nodeEnv?: string
  port?: number
  db?: {
    config: ConnectionOptions
  }
  auth?: {
    config: {
      jwt: Omit<jwt.Options, 'secret'>
      jwks: jwks.ExpressJwtOptions
    }
    middleware?: RequestHandler
  }
  apollo: {
    config: ApolloServerExpressConfig
  }
}

export async function init({
  nodeEnv = 'production',
  db,
  auth,
  apollo: {config: apolloConfig},
}: InitOptions): Promise<Application> {
  const isDev = nodeEnv === 'development'

  if (db) {
    await createConnection(db.config)
  }

  const app = express()
    .disable('x-powered-by')
    .use(morgan(isDev ? 'dev' : 'combined'))

  if (auth) {
    const {config, middleware} = auth

    app.use(middleware || jwtMiddleware(config))
  }

  const apollo = new ApolloServer({
    introspection: isDev,
    playground: isDev
      ? {
          settings: {
            'request.credentials': 'same-origin',
          },
        }
      : false,
    tracing: true,
    cacheControl: true,
    ...apolloConfig,
  })
  apollo.applyMiddleware({app})

  return app
}

export function run(app: Application, port: number, label: string): void {
  const portStr = chalk.yellow(port.toString())

  const server = http.createServer(app)

  server.listen(port, () => {
    console.log(chalk.cyan(`> Started ${label} on port ${portStr}`))
  })

  server.on('close', () => {
    console.log(chalk.cyan(`> ${label} shutting down`))
  })
}

export async function start(options: InitOptions): Promise<void> {
  const {PORT} = process.env
  const {label, port = PORT ? Number(PORT) : 3000} = options

  run(await init(options), port, label)
}
