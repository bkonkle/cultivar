open Express;
open Wonka;
open WonkaMiddleware;

module Paths = {
  let index = Routes.empty;
};

module Handlers = {
  let index: handler =
    source =>
      AuthenticateOperator.(
        source
        |> authenticate
        |> map((. event) =>
             Respond(
               Response.StatusCode.Ok,
               toJson(
                 Js.Json.[
                   ("success", boolean(true)),
                   ("anonymous", boolean(isAuthenticated(event))),
                 ],
               ),
             )
           )
      );
};

let routes: Routes.router(handler) =
  Routes.(one_of([Paths.index @--> Handlers.index]));

[@genType]
let middleware = Cultivar.app(routes);

[@gentype]
let default = {"middleware": middleware};
