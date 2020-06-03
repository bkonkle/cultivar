open Express;
open Wonka;
open Wonka_types;
open WonkaMiddleware;

module Authenticated = {
  type user;

  type event = {
    http: Http.t,
    user,
  };

  type handler = operatorT(event, jsonResult);
};

module Authenticate = {
  type event =
    | Authenticated(Authenticated.event)
    | Anonymous(Http.event);

  type handler = operatorT(event, jsonResult);

  let isAuthenticated = event =>
    switch (event) {
    | Anonymous(_) => false
    | Authenticated(_) => true
    };

  let toOption = event =>
    switch (event) {
    | Anonymous(_) => None
    | Authenticated(value) => Some(value)
    };

  let toEither = event =>
    Either.(
      switch (event) {
      | Anonymous(_) => Left
      | Authenticated(_) => Right
      }
    );

  let toHttp =
    (. event) =>
      switch (event) {
      | Anonymous(event) => event
      | Authenticated(event) => {http: event.http}
      };

  /**
   * Note: Unsafe.
   */
  let toAuthenticated =
    (. event) =>
      switch (event) {
      | Authenticated(event) => event
      | Anonymous(_) =>
        Js.Exn.raiseError(
          "Anonymous events cannot be mapped to Authenticated events.",
        )
      };
};

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

let skipOptions = (next: (. Http.event) => Authenticate.event) => {
  Authenticate.(
    (. event: Http.event) =>
      methodIsOptions(event.http.req)
      && hasAuthInAccessControl(event.http.req)
        ? Anonymous(event) : next(. event)
  );
};

let withAuthorizationHeader =
    (next: (. Http.event, string) => Authenticate.event) =>
  (. event: Http.event) =>
    switch (event.http.req |> Request.get("authorization")) {
    | Some(header) => next(. event, header)
    | None => Anonymous(event)
    };

let authenticateJwt = source =>
  Authenticate.(
    source
    |> map(
         skipOptions @@
         withAuthorizationHeader((. event, header) => {
           let parts = header |> Js.String.split(" ");

           if (parts |> Js.Array.length !== 2) {
             %log.warn
             "Credentials not in 'Authorization: Bearer [token]' format";
           };

           Anonymous(event);
         }),
       )
  );

let eitherAuthenticated =
    (~left: handler, ~right: Authenticated.handler, source) =>
  source
  |> authenticateJwt
  |> Authenticate.(
       either(
         ~test=toEither,
         ~left=src => src |> map(toHttp) |> left,
         ~right=src => src |> map(toAuthenticated) |> right,
       )
     );

[@genType]
let requireAuthentication = (handler: Authenticated.handler) =>
  eitherAuthenticated(
    ~left=
      map((. _) =>
        Respond(
          Response.StatusCode.Unauthorized,
          toJson(
            Js.Json.[
              ("success", boolean(false)),
              ("error", string("Not authorized")),
            ],
          ),
        )
      ),
    ~right=handler,
  );
