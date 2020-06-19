open Authn.Authentication;
open Express;
open Wonka;

/**
 * This is the signature that "getSecret" functions must follow. An example is the
 * JWKS.getJwksSecret function.
 */
type getSecret =
  (
    Express.Request.t,
    Js.nullable(JwtUtils.header),
    Js.nullable(JwtUtils.payload)
  ) =>
  Js.Promise.t(Js.Nullable.t(JwtUtils.secret'));

/**
 * This context is used by the authentication flow to pass input for each step.
 */
type context =
  | Header(Js.Option.t(string)) // authorization header
  | Credentials(string, string) // scheme, credentials
  | Token(string) // Bearer credentials
  | Decoded(string, JwtUtils.token) // unverified token
  | Verifying(string, JwtUtils.secret') // secret
  | Verified(JwtUtils.token) // verified token
  | Empty;

/**
 * Pull a message out of a Caml exception.
 */
let getExnMessage = (e: exn) =>
  Js.Exn.(
    Js.Option.(e |> asJsExn |> andThen((. err) => err |> message) |> getExn)
  );

/**
 * Returns true if this is an OPTIONS request.
 */
let methodIsOptions = req =>
  switch (req |> Request.httpMethod) {
  | Options => true
  | _ => false
  };

/**
 * Returns true if the authorization header is in the access-control-request-headers list.
 */
let hasAuthInAccessControl = req =>
  switch (req |> Request.get("access-control-request-headers")) {
  | Some(header) =>
    header
    |> Js.String.split(",")
    |> Js.Array.map(header => header |> Js.String.trim)
    |> Js.Array.indexOf("authorization") !== (-1)
  | None => false
  };

/**
 * If this is just an OPTIONS request and the auth header is in the access control list, then
 * skip the request by passing it through as Anonymous.
 */
let skipOptions = source =>
  source
  |> map((. operation: operation('authenticated)) =>
       switch (operation) {
       | Authenticating(event) =>
         methodIsOptions(event.http.req)
         && hasAuthInAccessControl(event.http.req)
           ? Anonymous(event) : Authenticating(event)
       | _ => operation
       }
     );

/**
 * Return an Authenticating event with the authorization Header, if it exists.
 */
let withAuthorizationHeader = source =>
  source
  |> map((. operation: operation('authenticated)) =>
       switch (operation) {
       | Authenticating(event) => (
           Authenticating(event),
           Header(event.http.req |> Request.get("authorization")),
         )
       | _ => (operation, Empty)
       }
     );

/**
 * Return an Authenticating event with the token Credentials, if they exist.
 */
let withCredentials = source =>
  source
  |> map((. operation: (operation('authenticated), context)) =>
       switch (operation) {
       // Authenticating events with a header present are transformed to include credentials
       | (Authenticating(event), Header(Some(header))) =>
         let parts = header |> Js.String.split(" ");
         if (parts |> Js.Array.length === 2) {
           let scheme = parts[0];
           let credentials = parts[1];

           (Authenticating(event), Credentials(scheme, credentials));
         } else {
           %log.warn
           "Credentials not in 'Authorization: [scheme] [token]' format.";

           // Convert to an anonymous event
           (Anonymous(event), Empty);
         };
       // If no header is present, convert to an anonymous event
       | (Authenticating(event), _) => (Anonymous(event), Empty)
       | (event, _) => (event, Empty)
       }
     );

/**
 * Return an Authenticating event with the Bearer Token, if it exists.
 */
let withBearerToken = source =>
  source
  |> map((. operation: (operation('authenticated), context)) =>
       switch (operation) {
       // Authenticating events with credentials are transformed to include the Bearer token
       | (Authenticating(event), Credentials(scheme, credentials)) =>
         if (scheme === "Bearer") {
           (Authenticating(event), Token(credentials));
         } else {
           %log.warn
           "Scheme '"
           ++ scheme
           ++ "' is not supported. Use the 'Bearer [token]` format.";

           // Convert to an anonymous event
           (Anonymous(event), Empty);
         }
       // If no credentials are present, convert to an anonymous event
       | (Authenticating(event), _) => (Anonymous(event), Empty)
       | (event, _) => (event, Empty)
       }
     );

/**
 * Return an Authenticating event with a decoded JWT token if successful.
 */
let decodeToken = source =>
  source
  |> map((. operation: (operation('authenticated), context)) =>
       switch (operation) {
       // Authenticating events with a token are transformed to include the decoded JWT
       | (Authenticating(event), Token(token)) =>
         let maybe =
           try(
             JwtUtils.decode(
               token,
               Some(JwtUtils.decodeOptions(~complete=true, ()))
               |> Js.Nullable.fromOption,
             )
             |> Js.Nullable.toOption
           ) {
           | e =>
             %log.warn
             "Error while decoding token: " ++ getExnMessage(e);

             None;
           };

         switch (maybe) {
         | Some(decoded) => (
             Authenticating(event),
             Decoded(token, decoded),
           )
         | None =>
           %log.warn
           "Unable to decode bearer token.";

           // Convert to an anonymous event
           (Anonymous(event), Empty);
         };
       // If no Bearer token is present, convert to an anonymous event
       | (Authenticating(event), _) => (Anonymous(event), Empty)
       | (event, _) => (event, Empty)
       }
     );

/**
 * Return an Authenticating event with an unverified token and secret value if successful.
 */
let withSecret = (getSecret: getSecret) =>
  mergeMap((. authEvent) =>
    switch (authEvent) {
    | (Authenticating(event), Decoded(token, decoded)) =>
      fromPromise(
        getSecret(event.http.req, decoded##header, decoded##payload),
      )
      |> map((. secret) =>
           switch (secret |> Js.Nullable.toOption) {
           | Some(secret) => (
               Authenticating(event),
               Verifying(token, secret),
             )
           | None => (Anonymous(event), Empty)
           }
         )
    | (Authenticating(event), _) => fromValue((Anonymous(event), Empty))
    | (event, _) => fromValue((event, Empty))
    }
  );

/**
 * Return an Authenticating event and use the toUser function to transform the verified token
 * if successful.
 */
let verifyToken = (verifyOptions, toUser) =>
  mergeMap((. authEvent) =>
    switch (authEvent) {
    | (Authenticating(event), Verifying(token, secret)) =>
      fromPromise(token |> JwtUtils.verify(~options=verifyOptions, secret))
      |> map((. decoded) =>
           Authenticated({http: event.http, user: toUser(event, decoded)})
         )
    | (Authenticating(event), _) => fromValue(Anonymous(event))
    | (event, _) => fromValue(event)
    }
  );

/**
 * The full JWT authentication flow.
 */
let authentication = (~getSecret, ~toUser, ~verifyOptions, source) =>
  source
  |> map(fromHttp)
  |> skipOptions
  |> withAuthorizationHeader
  |> withCredentials
  |> withBearerToken
  |> decodeToken
  |> withSecret(getSecret)
  |> verifyToken(verifyOptions, toUser);
