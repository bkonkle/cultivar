open Express;
open WonkaMiddleware;

let requireAuthentication = AuthenticateOperator.(requireAuthentication);
let isAuthenticated = Authentication.(isAuthenticated);

module Auth = {
  [@bs.val]
  external audience: Js.nullable(string) = "process.env.AUTH_AUDIENCE";

  [@bs.val] external issuer: Js.nullable(string) = "process.env.AUTH_ISSUER";
};

let getWithDefault = (default, nullable) =>
  nullable |> Js.Nullable.toOption |> Js.Option.getWithDefault(default);

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

  let toUser = (_event, token) => {"payload": token##payload};

  let verifyOptions =
    JWT.verifyOptions'(
      ~audience=Auth.audience |> getWithDefault(""),
      ~issuer=Auth.issuer |> getWithDefault(""),
      ~algorithms=[|"RS256"|],
      (),
    );

  let jwksOptions =
    JWKS.JwksClient.options(
      ~jwksUri="https://cultivar.auth0.com/.well-known/jwks.json",
      ~cache=true,
      ~rateLimit=true,
      ~jwksRequestsPerMinute=5,
      (),
    );

  let authenticate =
    JWTAuthentication.authentication(
      ~getSecret=JWKS.getJwksSecret(jwksOptions),
      ~toUser,
      ~verifyOptions,
    );

  let index = source =>
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
       );

  let appIndex = source =>
    source
    |> authenticate
    |> requireAuthentication(
         map((. _event) =>
           Respond(
             Response.StatusCode.Ok,
             toJson(Js.Json.[("success", boolean(true))]),
           )
         ),
       );

  let testId = (id: int) =>
    map((. _event) =>
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

let routes: Routes.router(WonkaMiddleware.handler) =
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
