open ExpressMiddleware;

let root = source => source |> RouterExchange.router(Router.routes);

[@gentype]
let default = {"middleware": middleware(root)};
