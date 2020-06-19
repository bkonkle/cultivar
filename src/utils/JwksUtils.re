module Client = {
  class type certSigningKey =
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

  type certSigningKey' = Js.t(certSigningKey);

  class type rsaSigningKey =
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

  type rsaSigningKey' = Js.t(rsaSigningKey);

  type signingKey =
    | Cert(certSigningKey')
    | Rsa(rsaSigningKey');

  type signingKey';

  [@bs.val]
  external signingKey: signingKey => signingKey' =
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

  exception Error(Js.Promise.error);

  [@bs.new] [@bs.module "node-jwks-rsa"]
  external make: options => t = "Client";

  [@bs.send]
  external getKeys: (t, (. Js.nullable(Js.Exn.t), 'keys) => unit) => unit =
    "getKeys";

  [@bs.send]
  external getSigningKeys:
    (t, (. Js.nullable(Js.Exn.t), Js.Array.t(signingKey')) => unit) => unit =
    "getSiginingKeys";

  [@bs.send]
  external getSigningKey:
    (t, string, (. Js.nullable(Js.Exn.t), signingKey') => unit) => unit =
    "getSiginingKeys";

  let getPublicKey: (. signingKey') => string = [%bs.raw
    {|
    function (signingKey) {
      return key.publicKey || key.rsaPublicKey;
    }
    |}
  ];
};

let catchSigningKeyError: Js.Promise.error => Js.Promise.t('a) =
  error => {
    open Js.Nullable;
    let (resolve, reject) = Js.Promise.(resolve, reject);

    // WARNING: Uses magic.
    switch (error |> Obj.magic |> Js.Exn.name) {
    // If we didn't find a match, do nothing.
    | Some("SigningKeyNotFoundError") => resolve(null)
    // If an error occured like rate limiting or HTTP issue, we'll bubble up the error.
    | Some(_) => reject(Client.Error(error))
    // Otherwise, do nothing.
    | _ => resolve(null)
    };
  };

let handleSigningKey = (resolve, reject) =>
  (. err, signingKey) =>
    Js.Nullable.(
      switch (err |> toOption) {
      // WARNING: Uses magic.
      | Some(error) => reject(. error |> Obj.magic)
      | _ =>
        resolve(.
          Some(JwtUtils.secret(String(Client.getPublicKey(. signingKey))))
          |> fromOption,
        )
      }
    );

[@gentype]
let getJwksSecret = (config): JWTAuthn.getSecret =>
  (_req, header, _payload) => {
    open Js.Nullable;
    let getSigningKey = Client.(getSigningKey);
    let catch = Js.Promise.(catch);

    let client = Client.make(config);

    Js.Promise.make((~resolve, ~reject) => {
      switch (header |> toOption) {
      | Some(header) =>
        switch (header##alg) {
        | "RS256" =>
          switch (header##kid |> toOption) {
          | Some(kid) =>
            getSigningKey(client, kid) @@ handleSigningKey(resolve, reject);
            resolve(. null);
          | _ => resolve(. null)
          }
        | _ => resolve(. null)
        }

      | None => resolve(. null)
      }
    })
    |> catch(catchSigningKeyError);
  };
