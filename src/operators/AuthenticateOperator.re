open Express;
open Wonka;
open Wonka_types;
open WonkaMiddleware;

type user;

type authenticatedEvent = {
  event: httpEvent,
  user,
};

type authenticateEvent =
  | Authenticated(authenticatedEvent)
  | Anonymous(httpEvent);

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

[@genType];
let authenticate: operatorT(httpEvent, authenticateEvent) =
  mergeMap((. event) => fromPromise(Js.Promise.resolve(Anonymous(event))));

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

let authenticateJwt = (handler: operatorT(authenticateEvent, jsonResult)) =>
  curry(source =>
    curry(sink =>
      source((. signal) => {
        switch (signal) {
        | Start(tb) => sink(. Start(tb))
        | Push(event) =>
          WonkaMiddleware.(
            if (event.req
                |> methodIsOptions
                && event.req
                |> hasAuthInAccessControl) {
              Next;
            } else {
              Next;
            }
          )
        | End => sink(. End)
        }
      })
    )
  );

let handleAnonymous: operatorT(authenticateEvent, jsonResult) =
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
  );

let handleAuthenticated =
    (handler: operatorT(authenticatedEvent, jsonResult), source) =>
  source
  |> map((. event) =>
       switch (event) {
       | Authenticated(value) => value
       | _ =>
         Js.Exn.raiseError(
           "Anonymous requests should not be given to handleAuthenticated",
         )
       }
     )
  |> handler;

[@genType]
let requireAuthentication =
    (handler: operatorT(authenticatedEvent, jsonResult), source) =>
  source
  |> authenticate
  |> either(
       ~test=
         event =>
           switch (event) {
           | Anonymous(_) => Left
           | Authenticated(_) => Right
           },
       ~left=handleAnonymous,
       ~right=handler |> handleAuthenticated,
     );
