module Authn = {
  include Authn;
  include AuthnExchange;

  type user = {. "payload": Js.nullable(JwtUtils.payload)};

  [@bs.val]
  external audience: Js.nullable(string) = "process.env.AUTH_AUDIENCE";

  [@bs.val] external issuer: Js.nullable(string) = "process.env.AUTH_ISSUER";

  let getWithDefault = (default, nullable) =>
    nullable |> Js.Nullable.toOption |> Js.Option.getWithDefault(default);

  let toUser = (_event, token: JwtUtils.token): user => {
    "payload": token##payload,
  };

  let verifyOptions =
    JwtUtils.verifyOptions'(
      ~audience=audience |> getWithDefault(""),
      ~issuer=issuer |> getWithDefault(""),
      ~algorithms=[|"RS256"|],
      (),
    );

  let jwksOptions =
    JwksUtils.Client.options(
      ~jwksUri="https://cultivar.auth0.com/.well-known/jwks.json",
      ~cache=true,
      ~rateLimit=true,
      ~jwksRequestsPerMinute=5,
      (),
    );

  let authenticate = input =>
    jwtAuthentication(
      ~getSecret=JwksUtils.getJwksSecret(jwksOptions),
      ~toUser,
      ~verifyOptions,
      input,
    );
};

type context = Js.Option.t(Js.t(bool));
let context: context = None;
