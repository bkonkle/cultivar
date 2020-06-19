open Authn;
open Express;
open JsonUtils;
open Wonka;
open Wonka_types;

type authenticator('user) =
  sourceT(ExpressHttp.operation) => sourceT(Authentication.operation('user));

let unauthorizedResponse =
  ExpressHttp.Respond(
    Response.StatusCode.Unauthorized,
    toJson(
      Js.Json.[
        ("success", boolean(false)),
        ("error", string("Not authorized")),
      ],
    ),
  );

[@genType]
let jwtAuthentication =
    (
      ~getSecret,
      ~toUser,
      ~verifyOptions,
      input:
        Cultivar.Exchange.input(
          Authentication.operation('user),
          'result,
          'context,
        ),
    )
    : operatorT(ExpressHttp.operation, 'result) =>
  source =>
    source
    |> JWTAuthn.authentication(~getSecret, ~toUser, ~verifyOptions)
    |> input.forward;

[@genType]
let requireAuthentication =
    (
      input:
        Cultivar.Exchange.input(
          Authenticated.operation('user),
          ExpressHttp.result,
          'context,
        ),
    )
    : operatorT(Authentication.operation('user), ExpressHttp.result) =>
  mergeMap((. operation) =>
    switch (operation) {
    | Authentication.Authenticated(event) => input.forward(fromValue(event))
    | _ => fromValue(unauthorizedResponse)
    }
  );
