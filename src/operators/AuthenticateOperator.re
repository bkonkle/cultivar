open Express;
open Wonka;
open Wonka_types;
open WonkaMiddleware;

type user;

type authenticatedEvent = (httpEvent, user);

type authenticateEvent =
  | Authenticated(authenticatedEvent)
  | Anonymous(httpEvent);

let isAuthenticated = event =>
  switch (event) {
  | Anonymous(_) => false
  | Authenticated(_) => true
  };

let unauthorizedResult =
  Respond(
    Response.StatusCode.Unauthorized,
    toJson(
      Js.Json.[("anonymous", boolean(true)), ("success", boolean(false))],
    ),
  );

[@genType];
let authenticate: operatorT(httpEvent, authenticateEvent) =
  mergeMap((. event) => fromPromise(Js.Promise.resolve(Anonymous(event))));

[@genType]
let requireAuthentication =
    (operator: operatorT(authenticatedEvent, jsonResult), source) =>
  source
  |> authenticate
  |> curry(source =>
       curry(sink => {
         source((. signal) => {
           switch (signal) {
           | Start(tb) => sink(. Start(tb))
           | Push(event) =>
             switch (event) {
             | Anonymous(_) => sink(. Push(unauthorizedResult))
             | Authenticated((httpEvent, user)) =>
               operator(fromValue((httpEvent, user)), sink)
             }
           | End => sink(. End)
           }
         })
       })
     );
