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
let jwtAuthentication = (~getSecret, ~toUser, ~verifyOptions) =>
  (.
    input:
      Cultivar.Exchange.input(
        Authentication.operation('user),
        'result,
        'context,
      ),
  ) => (
    source =>
      source
      |> JwtAuthn.authentication(~getSecret, ~toUser, ~verifyOptions)
      |> input.forward:
      operatorT(ExpressHttp.operation, 'result)
  );

[@genType]
let requireAuthentication =
  (.
    input:
      Cultivar.Exchange.input(
        Authenticated.operation('user),
        ExpressHttp.result,
        'context,
      ),
  ) => (
    mergeMap((. operation) =>
      switch (operation) {
      | Authentication.Authenticated(event) =>
        input.forward(fromValue(event))
      | _ => fromValue(unauthorizedResponse)
      }
    ):
      operatorT(Authentication.operation('user), ExpressHttp.result)
  );
