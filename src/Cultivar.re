open WonkaMiddleware;

[@genType]
let app = routes => middleware(RouterOperator.router(routes));
