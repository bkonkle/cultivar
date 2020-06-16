open Authentication;
open Express;
open Wonka_types;
open Wonka;

type authenticator('user) =
  sourceT(HttpOperation.event) => sourceT(event('user));

let rejectAnonymous = source =>
  source
  |> map((. _) =>
       HttpOperation.Respond(
         Response.StatusCode.Unauthorized,
         ExpressMiddleware.toJson(
           Js.Json.[
             ("success", boolean(false)),
             ("error", string("Not authorized")),
           ],
         ),
       )
     );

[@genType]
let requireAuthentication =
    (
      exchange: operatorT(authenticatedEvent('user), HttpOperation.result),
      source,
      sink,
    ) =>
  source((. signal) => {
    switch (signal) {
    | Start(tb) => sink(. Start(tb))
    | Push(authEvent) =>
      switch (authEvent) {
      | Authenticated(event) => sink |> exchange(fromValue(event))
      | _ => sink |> rejectAnonymous(fromValue(authEvent))
      }
    | End => sink(. End)
    }
  });
