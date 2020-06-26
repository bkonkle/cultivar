open ExpressMiddleware;

[@genType]
let default = {
  "middleware": middleware(Router.routes |> RouterExchange.router),
};
