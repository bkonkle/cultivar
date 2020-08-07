import {map} from 'wonka'

import {respond} from '../../src/ExpressHttp'
import {createMiddleware} from '../../src/ExpressMiddleware'
import StatusCode from 'status-code-enum'

export const middleware = createMiddleware({
  exchange: () => map(() => respond(StatusCode.SuccessOK, {success: true})),
})
