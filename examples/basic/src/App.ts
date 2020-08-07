import {map} from 'wonka'

import {forward} from '../../../src/ExpressHttp'
import {createMiddleware} from '../../../src/ExpressMiddleware'

export const middleware = createMiddleware({
  exchange: () => map(forward),
})
