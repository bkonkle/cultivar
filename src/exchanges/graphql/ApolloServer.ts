import express from 'express'
import corsMiddleware from 'cors'
import {json, OptionsJson} from 'body-parser'
import {
  renderPlaygroundPage,
  RenderPageOptions as PlaygroundRenderPageOptions,
} from '@apollographql/graphql-playground-html'
import {
  GraphQLOptions,
  FileUploadOptions,
  ApolloServerBase,
  formatApolloErrors,
  processFileUploads,
  ContextFunction,
  Context,
  Config,
} from 'apollo-server-core'
import {ExecutionParams} from 'subscriptions-transport-ws'
import accepts from 'accepts'
import typeis from 'type-is'

export {GraphQLOptions, GraphQLExtension, gql} from 'apollo-server-core'

export interface GetMiddlewareOptions {
  path?: string
  cors?:
    | corsMiddleware.CorsOptions
    | corsMiddleware.CorsOptionsDelegate
    | boolean
  bodyParserConfig?: OptionsJson | boolean
  onHealthCheck?: (req: express.Request) => Promise<unknown>
  disableHealthCheck?: boolean
}

export interface ServerRegistration extends GetMiddlewareOptions {
  app: express.Application
}

const fileUploadMiddleware = (
  uploadsConfig: FileUploadOptions,
  server: ApolloServerBase
) => (
  req: express.Request,
  res: express.Response,
  next: express.NextFunction
) => {
  // Note: we use typeis directly instead of via req.is for connect support.
  if (
    typeof processFileUploads === 'function' &&
    typeis(req, ['multipart/form-data'])
  ) {
    processFileUploads(req, res, uploadsConfig)
      .then((body) => {
        req.body = body
        next()
      })
      .catch((error) => {
        if (error.status && error.expose) res.status(error.status)

        next(
          formatApolloErrors([error], {
            formatter: server.requestOptions.formatError,
            debug: server.requestOptions.debug,
          })
        )
      })
  } else {
    next()
  }
}

export interface ExpressContext {
  req: express.Request
  res: express.Response
  connection?: ExecutionParams
}

export interface ApolloServerExpressConfig extends Config {
  context?: ContextFunction<ExpressContext, Context> | Context
}

/**
 * Forked from the official `apollo-server-express` ApolloServer class
 */
export class ApolloServer extends ApolloServerBase {
  constructor(config: ApolloServerExpressConfig) {
    super(config)
  }

  // This translates the arguments from the middleware into graphQL options It
  // provides typings for the integration specific behavior, ideally this would
  // be propagated with a generic to the super class
  async createGraphQLServerOptions(
    req: express.Request,
    res: express.Response
  ): Promise<GraphQLOptions> {
    return super.graphQLServerOptions({req, res})
  }

  protected supportsSubscriptions(): boolean {
    return true
  }

  protected supportsUploads(): boolean {
    return true
  }

  public async applyMiddleware({
    app,
    ...rest
  }: ServerRegistration): Promise<void> {
    app.use(await this.getMiddleware(rest))
  }

  public async getMiddleware({
    path,
    cors,
    bodyParserConfig,
    disableHealthCheck,
    onHealthCheck,
  }: GetMiddlewareOptions = {}): Promise<express.Router> {
    await this.willStart()

    if (!path) path = '/graphql'

    const router = express.Router()

    if (!disableHealthCheck) {
      router.use('/.well-known/apollo/server-health', (req, res) => {
        // Response follows https://tools.ietf.org/html/draft-inadarei-api-health-check-01
        res.type('application/health+json')

        if (onHealthCheck) {
          onHealthCheck(req)
            .then(() => {
              res.json({status: 'pass'})
            })
            .catch(() => {
              res.status(503).json({status: 'fail'})
            })
        } else {
          res.json({status: 'pass'})
        }
      })
    }

    let uploadsMiddleware
    if (this.uploadsConfig && typeof processFileUploads === 'function') {
      uploadsMiddleware = fileUploadMiddleware(this.uploadsConfig, this)
    }

    // XXX multiple paths?
    this.graphqlPath = path

    // Note that we don't just pass all of these handlers to a single app.use call
    // for 'connect' compatibility.
    if (cors === true) {
      router.use(path, corsMiddleware())
    } else if (cors !== false) {
      router.use(path, corsMiddleware(cors))
    }

    if (bodyParserConfig === true) {
      router.use(path, json())
    } else if (bodyParserConfig !== false) {
      router.use(path, json(bodyParserConfig))
    }

    if (uploadsMiddleware) {
      router.use(path, uploadsMiddleware)
    }

    // Note: if you enable playground in production and expect to be able to see your
    // schema, you'll need to manually specify `introspection: true` in the
    // ApolloServer constructor; by default, the introspection query is only
    // enabled in dev.
    router.use(path, (req, res, next) => {
      if (this.playgroundOptions && req.method === 'GET') {
        // perform more expensive content-type check only if necessary
        // XXX We could potentially move this logic into the GuiOptions lambda,
        // but I don't think it needs any overriding
        const accept = accepts(req)
        const types = accept.types() as string[]
        const prefersHTML =
          types.find(
            (x: string) => x === 'text/html' || x === 'application/json'
          ) === 'text/html'

        if (prefersHTML) {
          const playgroundRenderPageOptions: PlaygroundRenderPageOptions = {
            endpoint: req.originalUrl,
            subscriptionEndpoint: this.subscriptionsPath,
            ...this.playgroundOptions,
          }
          res.setHeader('Content-Type', 'text/html')
          const playground = renderPlaygroundPage(playgroundRenderPageOptions)
          res.write(playground)
          res.end()
          return
        }
      }

      // NOTE: The main difference between this and `apollo-server-express` is here.
      // Instead of applying the middleware now, allow the GraphQLExchange to be added
      // later as a Wonka operator.
      next()
    })

    return router
  }
}
