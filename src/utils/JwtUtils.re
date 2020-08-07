/***
 * Various union types to emulate JavaScript's polymorphic function arguments.
 */

type stringOrNumber =
  | String(string)
  | Number(float);

[@genType.import "../shims/JwtUtils.shim"]
type stringOrNumberJs;

[@bs.val]
external stringOrNumber: stringOrNumber => stringOrNumberJs =
  "Array.prototype.shift.call";

type stringOrArray =
  | String(string)
  | Array(Js.Array.t(string));

[@genType.import "../shims/JwtUtils.shim"]
type stringOrArrayJs;

[@bs.val]
external stringOrArray: stringOrArray => stringOrArrayJs =
  "Array.prototype.shift.call";

type stringOrRegex =
  | String(string)
  | Regex(Js.Re.t);

[@genType.import "../shims/JwtUtils.shim"]
type stringOrRegexJs;

[@bs.val]
external stringorRegex: stringOrRegex => stringOrRegexJs =
  "Array.prototype.shift.call";

type stringOrRegexArray =
  | Single(stringOrRegexJs)
  | Array(Js.Array.t(stringOrRegexJs));

[@genType.import "../shims/JwtUtils.shim"]
type stringOrRegexArrayJs;

[@bs.val]
external stringOrRegexArray: stringOrRegexArray => stringOrRegexArrayJs =
  "Array.prototype.shift.call";

type stringOrBuffer =
  | String(string)
  | Buffer(Node.Buffer.t);

[@genType.import "../shims/JwtUtils.shim"]
type stringOrBufferJs;

[@bs.val]
external stringOrBuffer: stringOrBuffer => stringOrBufferJs =
  "Array.prototype.shift.call";

type secret =
  | String(string)
  | Buffer(Node.Buffer.t)
  | Credentials(
      {
        .
        "key": stringOrBufferJs,
        "passphrase": string,
      },
    );

[@genType.import "../shims/JwtUtils.shim"]
type secretJs;

[@bs.val] external secret: secret => secretJs = "Array.prototype.shift.call";

type payload('obj) =
  | String(string)
  | Buffer(Node.Buffer.t)
  | Object('obj);

[@genType.import "../shims/JwtUtils.shim"]
type payloadJs;

[@bs.val]
external payload: payload('obj) => payloadJs = "Array.prototype.shift.call";

/***
 * Options for the JWT operations available.
 */
[@genType]
type signOptions('header) = {
  .
  "algorithm": Js.nullable(string),
  "keyId": Js.nullable(string),
  "expiresIn": Js.nullable(stringOrNumberJs),
  "notBefore": Js.nullable(stringOrNumberJs),
  "audience": Js.nullable(stringOrArrayJs),
  "subject": Js.nullable(string),
  "issuer": Js.nullable(string),
  "jwtid": Js.nullable(string),
  "mutatePayload": Js.nullable(bool),
  "noTimestamp": Js.nullable(bool),
  "header": Js.nullable('header),
  "encoding": Js.nullable(string),
};

[@bs.obj]
external signOptions:
  (
    ~algorithm: string=?,
    ~keyId: string=?,
    ~expiresIn: stringOrNumberJs=?,
    ~notBefore: stringOrNumberJs=?,
    ~audience: stringOrArrayJs=?,
    ~subject: string=?,
    ~issuer: string=?,
    ~jwtid: string=?,
    ~mutatePayload: bool=?,
    ~noTimestamp: bool=?,
    ~header: 'header=?,
    ~encoding: string=?,
    unit
  ) =>
  signOptions('header);

[@genType]
type verifyOptions = {
  .
  "algorithms": Js.nullable(Js.Array.t(string)),
  "audience": Js.nullable(stringOrRegexArrayJs),
  "clockTimestamp": Js.nullable(int),
  "clockTolerance": Js.nullable(int),
  "complete": Js.nullable(bool),
  "issuer": Js.nullable(stringOrArrayJs),
  "ignoreExpiration": Js.nullable(bool),
  "ignoreNotBefore": Js.nullable(bool),
  "jwtid": Js.nullable(string),
  "nonce": Js.nullable(string),
  "subject": Js.nullable(string),
  "maxAge": Js.nullable(string),
};

[@bs.obj]
external verifyOptions:
  (
    ~algorithms: Js.Array.t(string)=?,
    ~audience: stringOrRegexArrayJs=?,
    ~clockTimestamp: int=?,
    ~clockTolerance: int=?,
    ~complete: bool=?,
    ~issuer: stringOrArrayJs=?,
    ~ignoreExpiration: bool=?,
    ~ignoreNotBefore: bool=?,
    ~jwtId: string=?,
    ~nonce: string=?,
    ~subject: string=?,
    ~maxAge: string=?,
    unit
  ) =>
  verifyOptions;

/**
 * A type-specific version of verifyOptions that works only with single strings.
 */
let verifyOptions' = (~audience: string, ~issuer: string) =>
  verifyOptions(
    ~audience=stringOrRegexArray(Single(stringorRegex(String(audience)))),
    ~issuer=stringOrArray(String(issuer)),
  );

type decodeOptions = {
  .
  "complete": Js.nullable(bool),
  "json": Js.nullable(bool),
};

[@bs.obj]
external decodeOptions:
  (~complete: bool=?, ~json: bool=?, unit) => decodeOptions;

/***
 * General type information.
 */
[@genType]
type header = {
  .
  "alg": string,
  "typ": Js.nullable(string),
  "kid": Js.nullable(string),
  "jku": Js.nullable(string),
  "x5u": Js.nullable(string),
  "x5t": Js.nullable(string),
};

[@genType]
type token = {
  .
  "header": Js.nullable(header),
  "payload": Js.nullable(payloadJs),
};

[@genType]
exception InvalidToken(Js.Exn.t);

/***
 * External function calls and helpers.
 */

[@bs.module "jsonwebtoken"]
external sign: (payloadJs, secretJs, Js.nullable(signOptions('b))) => string =
  "sign";

[@bs.module "jsonwebtoken"]
external verify_:
  (
    string,
    secretJs,
    Js.nullable(verifyOptions),
    (Js.nullable(Js.Exn.t), token) => unit
  ) =>
  unit =
  "verify";

[@bs.module "jsonwebtoken"]
external decode: (string, Js.nullable(decodeOptions)) => Js.nullable(token) =
  "decode";

[@genType]
let verify = (~options=?, secret, token) =>
  Js.Promise.make((~resolve, ~reject) => {
    verify_(token, secret, options |> Js.Nullable.fromOption, (err, decoded) =>
      switch (Js.Nullable.toOption(err)) {
      | Some(err) => reject(. InvalidToken(err))
      | None => resolve(. decoded)
      }
    )
  });
