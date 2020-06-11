class type _certSigningKey =
  [@bs]
  {
    [@bs.set]
    pub kid: string;
    [@bs.set]
    pub nbf: string;
    [@bs.set]
    pub publicKey: string;
    pub getPublicKey: unit => string;
  };

type certSigningKey = Js.t(_certSigningKey);

class type _rsaSigningKey =
  [@bs]
  {
    [@bs.set]
    pub kid: string;
    [@bs.set]
    pub nbf: string;
    [@bs.set]
    pub rsaPublicKey: string;
    pub getPublicKey: unit => string;
  };

type rsaSigningKey = Js.t(_rsaSigningKey);

type unionOfCertOrRsa =
  | Cert(certSigningKey)
  | Rsa(rsaSigningKey);

type signingKey;

[@bs.val]
external signingKey: unionOfCertOrRsa => certOrRsa =
  "Array.prototype.shift.call";

type options;

type t;

[@bs.obj]
external options:
  (
    ~jwksUri: string,
    ~rateLimit: bool=?,
    ~cache: bool=?,
    ~cacheMaxEntries: int=?,
    ~cacheMaxAge: int=?,
    ~jwksRequestsPerMinute: int=?,
    ~proxy: string=?,
    ~strictSsl: bool=?,
    ~requestHeaders: 'headers=?,
    ~timeout: int=?,
    unit
  ) =>
  options;

[@bs.new] [@bs.module "node-jwks-rsa"]
external make: options => t = "JwksClient";

[@bs.send]
external getKeys: (t, (. Js.nullable(Js.Exn.t), 'keys) => unit) => unit =
  "getKeys";

[@bs.send]
external getSigningKeys:
  (t, (. Js.nullable(Js.Exn.t), Js.Array.t(signingKey)) => unit) => unit =
  "getSiginingKeys";

[@bs.send]
external getSigningKey:
  (t, string, (. Js.nullable(Js.Exn.t), signingKey) => unit) => unit =
  "getSiginingKeys";
