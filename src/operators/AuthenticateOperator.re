open Express;
open Wonka;
open Wonka_types;
open WonkaMiddleware;

module Authenticated = {
  type event('user) = {
    http: Http.t,
    user: 'user,
  };

  type handler('user) = operatorT(event('user), jsonResult);
};

module Authn = {
  type event('user) =
    | Authenticating(Http.event)
    | Authenticated(Authenticated.event('user))
    | Anonymous(Http.event);

  type context =
    | Header(Js.Option.t(string)) // authorization header
    | Credentials(string, string) // scheme, credentials
    | Token(string) // Bearer credentials
    | Decoded(string, JWT.token) // unverified token
    | Verifying(string, JWT.secret) // secret
    | Verified(JWT.token) // verified token
    | Empty;

  type handler('user) = operatorT(event('user), jsonResult);

  let isAuthenticated = event =>
    switch (event) {
    | Authenticated(_) => true
    | _ => false
    };

  let toOption = event =>
    switch (event) {
    | Authenticated(event) => Some(event)
    | _ => None
    };

  let toEither = event =>
    Either.(
      switch (event) {
      | Authenticated(_) => Right
      | _ => Left
      }
    );

  let toHttp =
    (. event) =>
      switch (event) {
      | Authenticating(event) => event
      | Anonymous(event) => event
      | Authenticated(event) => {http: event.http}
      };

  let fromHttp = (. event) => Authenticating(event);
};

let getExnMessage = (e: exn) =>
  Js.Exn.(
    Js.Option.(e |> asJsExn |> andThen((. err) => err |> message) |> getExn)
  );

let methodIsOptions = req =>
  switch (req |> Request.httpMethod) {
  | Options => true
  | _ => false
  };

let hasAuthInAccessControl = req =>
  switch (req |> Request.get("access-control-request-headers")) {
  | Some(header) =>
    header
    |> Js.String.split(",")
    |> Js.Array.map(header => header |> Js.String.trim)
    |> Js.Array.indexOf("authorization") !== (-1)
  | None => false
  };

let skipOptions = source =>
  source
  |> Authn.(
       map((. authEvent: event('user)) =>
         switch (authEvent) {
         | Authenticating(event) =>
           methodIsOptions(event.http.req)
           && hasAuthInAccessControl(event.http.req)
             ? Anonymous(event) : Authenticating(event)
         | _ => authEvent
         }
       )
     );

let withAuthorizationHeader = source =>
  source
  |> Authn.(
       map((. authn: event('user)) =>
         switch (authn) {
         | Authenticating(event) => (
             Authenticating(event),
             Header(event.http.req |> Request.get("authorization")),
           )
         | _ => (authn, Empty)
         }
       )
     );

let withCredentials = source =>
  source
  |> Authn.(
       map((. authn: (event('user), context)) =>
         switch (authn) {
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
       )
     );

let withBearerToken = source =>
  source
  |> Authn.(
       map((. authn: (event('user), context)) =>
         switch (authn) {
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
       )
     );

let decodeToken = source =>
  source
  |> Authn.(
       map((. authn: (event('user), context)) =>
         switch (authn) {
         // Authenticating events with a token are transformed to include the decoded JWT
         | (Authenticating(event), Token(token)) =>
           let maybe =
             try(
               JWT.decode(
                 token,
                 Some(JWT.decodeOptions(~complete=true, ()))
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
       )
     );

let withSecret = getSecret =>
  Authn.(
    mergeMap((. authEvent) =>
      switch (authEvent) {
      | (Authenticating(event), Decoded(token, decoded)) =>
        fromPromise(
          getSecret(event.http.req, decoded##header, decoded##payload),
        )
        |> map((. secret: JWT.secret) =>
             (Authenticating(event), Verifying(token, secret))
           )
      | (Authenticating(event), _) => fromValue((Anonymous(event), Empty))
      | (event, _) => fromValue((event, Empty))
      }
    )
  );

let verifyToken = toUser =>
  Authn.(
    mergeMap((. authEvent) =>
      switch (authEvent) {
      | (Authenticating(event), Verifying(token, secret)) =>
        fromPromise(token |> JWT.verify(secret))
        |> map((. decoded) =>
             Authenticated({http: event.http, user: toUser(event, decoded)})
           )
      | (Authenticating(event), _) => fromValue(Anonymous(event))
      | (event, _) => fromValue(event)
      }
    )
  );

let rejectAnonymous = source =>
  source
  |> map((. _) =>
       Respond(
         Response.StatusCode.Unauthorized,
         toJson(
           Js.Json.[
             ("success", boolean(false)),
             ("error", string("Not authorized")),
           ],
         ),
       )
     );

[@genType]
let requireAuthentication =
    (handler: Authenticated.handler('user), source, sink) =>
  Authn.(
    source((. signal) => {
      switch (signal) {
      | Start(tb) => sink(. Start(tb))
      | Push(authEvent) =>
        switch (authEvent) {
        | Authenticated(event) => sink |> handler(fromValue(event))
        | _ => sink |> rejectAnonymous(fromValue(authEvent))
        }
      | End => sink(. End)
      }
    })
  );

let jwtAuthentication = (~getSecret, ~toUser, source) =>
  Authn.(
    source
    |> map(fromHttp)
    |> skipOptions
    |> withAuthorizationHeader
    |> withCredentials
    |> withBearerToken
    |> decodeToken
    |> withSecret(getSecret)
    |> verifyToken(toUser)
  );
