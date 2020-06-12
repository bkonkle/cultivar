open ExpressMiddleware;

let root = source => source |> RouterOperator.router(Router.routes);

[@gentype]
let default = {"middleware": middleware(root)};
