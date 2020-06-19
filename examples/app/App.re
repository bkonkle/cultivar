open ExpressMiddleware;

[@gentype]
let default = {
  "middleware": middleware(Router.routes |> RouterExchange.router),
};
