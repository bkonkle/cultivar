include Authentication;
include AuthenticateExchange;

module User = {
  type t = {. "payload": Js.nullable(JWT.payload)};
};

module Authenticated = {
  module Operation = AuthenticatedOperation.Make(User);
  module Exchange = OperationExchange.Make(Operation);
};

let getWithDefault = (default, nullable) =>
  nullable |> Js.Nullable.toOption |> Js.Option.getWithDefault(default);

[@bs.val]
external audience: Js.nullable(string) = "process.env.AUTH_AUDIENCE";

[@bs.val] external issuer: Js.nullable(string) = "process.env.AUTH_ISSUER";

let toUser = (_event, token: JWT.token): User.t => {
  "payload": token##payload,
};

let verifyOptions =
  JWT.verifyOptions'(
    ~audience=audience |> getWithDefault(""),
    ~issuer=issuer |> getWithDefault(""),
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
