open Authentication;
open Express;
open Wonka;
open Wonka_types;
open ExpressMiddleware;

type authenticator('user) =
  sourceT(Http.event) => sourceT(Authentication.event('user));

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
  });
