open Express;
open WonkaMiddleware;

module Paths = {
  open Routes;

  let index = empty;

  let app = s("app");
  let appIndex = app /? nil;

  let tests = s("tests");
  let testId = tests / s("id") / int /? nil;
};

module Handlers = {
  open Wonka;

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
                   ("anonymous", boolean(!isAuthenticated(event))),
                 ],
               ),
             )
           )
      );

  let appIndex: handler =
    source =>
      AuthenticateOperator.(
        source
        |> requireAuthentication(
             map((. _event) =>
               Respond(
                 Response.StatusCode.Ok,
                 toJson(Js.Json.[("success", boolean(true))]),
               )
             ),
           )
      );

  let testId = (id: int): handler =>
    source =>
      source
      |> map((. _event) =>
           Respond(
             Response.StatusCode.Ok,
             toJson(
               Js.Json.[
                 ("success", boolean(true)),
                 ("id", number(float_of_int(id))),
               ],
             ),
           )
         );
};

let routes: Routes.router(handler) =
  Routes.(
    one_of([
      Paths.index @--> Handlers.index,
      Paths.appIndex @--> Handlers.appIndex,
      Paths.testId @--> Handlers.testId,
    ])
  );

[@genType]
let middleware = Cultivar.app(routes);

[@gentype]
let default = {"middleware": middleware};
