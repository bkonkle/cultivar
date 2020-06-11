/***
 * Various union types to emulate JavaScript's polymorphic function arguments.
 */

type unionOfStringOrNumber =
  | String(string)
  | Number(float);

type stringOrNumber;

[@bs.val]
external stringOrNumber: unionOfStringOrNumber => stringOrNumber =
  "Array.prototype.shift.call";

type unionOfStringOrArray =
  | Single(string)
  | Array(Js.Array.t(string));

type stringOrArray;

[@bs.val]
external stringOrArray: unionOfStringOrArray => stringOrArray =
  "Array.prototype.shift.call";

type unionOfStringOrRegex =
  | String(string)
  | Regex(Js.Re.t);

type stringOrRegex;

[@bs.val]
external stringorRegex: unionOfStringOrRegex => stringOrRegex =
  "Array.prototype.shift.call";

type unionOfStringOrRegexArray =
  | Single(stringOrRegex)
  | Array(Js.Array.t(stringOrRegex));

type stringOrRegexArray;

[@bs.val]
external stringOrRegexArray: unionOfStringOrRegexArray => stringOrRegexArray =
  "Array.prototype.shift.call";

type unionOfStringOrBuffer =
  | String(string)
  | Buffer(Node.Buffer.t);

type stringOrBuffer;

[@bs.val]
external stringOrBuffer: unionOfStringOrBuffer => stringOrBuffer =
  "Array.prototype.shift.call";

type unionOfSecretTypes =
  | String(string)
  | Buffer(Node.Buffer.t)
  | Credentials(
      {
        .
        "key": stringOrBuffer,
        "passphrase": string,
      },
    );

type secret;

[@bs.val]
external secret: unionOfSecretTypes => secret = "Array.prototype.shift.call";

type unionOfPayloadTypes('obj) =
  | String(string)
  | Buffer(Node.Buffer.t)
  | Object('obj);

type payload;

[@bs.val]
external payload: unionOfPayloadTypes('obj) => payload =
  "Array.prototype.shift.call";

/***
 * Options for the JWT operations available.
 */

type signOptions('header) = {
  .
  "algorithm": Js.nullable(string),
  "keyId": Js.nullable(string),
  "expiresIn": Js.nullable(stringOrNumber),
  "notBefore": Js.nullable(stringOrNumber),
  "audience": Js.nullable(stringOrArray),
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
    ~expiresIn: stringOrNumber=?,
    ~notBefore: stringOrNumber=?,
    ~audience: stringOrArray=?,
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

type verifyOptions = {
  .
  "algorithms": Js.nullable(Js.Array.t(string)),
  "audience": Js.nullable(stringOrRegexArray),
  "clockTimestamp": Js.nullable(int),
  "clockTolerance": Js.nullable(int),
  "complete": Js.nullable(bool),
  "issuer": Js.nullable(stringOrArray),
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
    ~audience: stringOrRegexArray=?,
    ~clockTimestamp: int=?,
    ~clockTolerance: int=?,
    ~complete: bool=?,
    ~issuer: stringOrArray=?,
    ~ignoreExpiration: bool=?,
    ~ignoreNotBefore: bool=?,
    ~jwtId: string=?,
    ~nonce: string=?,
    ~subject: string=?,
    ~maxAge: string=?,
    unit
  ) =>
  verifyOptions;

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

type header = {
  .
  "alg": string,
  "typ": Js.nullable(string),
  "kid": Js.nullable(string),
  "jku": Js.nullable(string),
  "x5u": Js.nullable(string),
  "x5t": Js.nullable(string),
};

type token = {
  .
  "header": Js.nullable(header),
  "payload": Js.nullable(payload),
};

exception InvalidToken(Js.Exn.t);

/***
 * External function calls and helpers.
 */

[@bs.module "jsonwebtoken"]
external sign: (payload, secret, Js.nullable(signOptions('b))) => string =
  "sign";

[@bs.module "jsonwebtoken"]
external verify_:
  (
    string,
    secret,
    Js.nullable(verifyOptions),
    (Js.nullable(Js.Exn.t), token) => unit
  ) =>
  unit =
  "verify";

let verify = (~options=?, secret, token) =>
  Js.Promise.make((~resolve, ~reject) => {
    verify_(token, secret, options |> Js.Nullable.fromOption, (err, decoded) =>
      switch (Js.Nullable.toOption(err)) {
      | Some(err) => reject(. InvalidToken(err))
      | None => resolve(. decoded)
      }
    )
  });

[@bs.module "jsonwebtoken"]
external decode: (string, Js.nullable(decodeOptions)) => Js.nullable(token) =
  "decode";
