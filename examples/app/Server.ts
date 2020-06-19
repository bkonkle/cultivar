import chalk from 'chalk'
import express, {Application} from 'express'
import http from 'http'
import morgan from 'morgan'

import App from './App.gen'

export function start() {
  const {NODE_ENV = 'development'} = process.env

  const isDev = NODE_ENV === 'development'

  const app = express()
    .disable('x-powered-by')
    .use(morgan(isDev ? 'dev' : 'combined'))
    .use(App.middleware)

  run(app, 3000)
}

export function run(app: Application, port: number, baseUrl?: string) {
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
  start()
}
