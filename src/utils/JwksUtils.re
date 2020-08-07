open Wonka_types;

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

  [@bs.new] [@bs.module] external make: options => t = "jwks-rsa";

  [@bs.send]
  external getKeys: (t, (. Js.nullable(Js.Exn.t), 'keys) => unit) => unit =
    "getKeys";

  [@bs.send]
  external getSigningKeys:
    (t, (. Js.nullable(Js.Exn.t), Js.Array.t(signingKey')) => unit) => unit =
    "getSigningKeys";

  [@bs.send]
  external getSigningKey:
    (t, string, (. Js.nullable(Js.Exn.t), signingKey') => unit) => unit =
    "getSigningKey";

  let getPublicKey: (. signingKey') => string = [%bs.raw
    {|
    function (signingKey) {
      return signingKey.publicKey || signingKey.rsaPublicKey;
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

[@genType]
let getJwksSecret: (. Client.options) => JwtAuthn.getSecret =
  (. config) =>
    curry((_req, header, _payload) =>
      Js.Promise.(
        make((~resolve, ~reject) => {
          let (toOption, null) = Js.Nullable.(toOption, null);
          let getSigningKey = Client.(getSigningKey);

          let client = Client.make(config);

          switch (header |> toOption) {
          | Some(header) =>
            switch (header##alg) {
            | "RS256" =>
              switch (header##kid |> toOption) {
              | Some(kid) =>
                getSigningKey(client, kid) @@
                handleSigningKey(resolve, reject);
                resolve(. null);
              | _ => resolve(. null)
              }
            | _ => resolve(. null)
            }

          | None => resolve(. null)
          };
        })
        |> catch(catchSigningKeyError)
      )
    );
