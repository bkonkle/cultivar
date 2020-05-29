open Express;
open Wonka;
open Wonka_types;
open WonkaMiddleware;

type user;

type authenticatedEvent =
  | Authenticated(event, user)
  | Anonymous(event);

let authenticate: operatorT(event, authenticatedEvent) =
  mergeMap((. event) => fromPromise(Js.Promise.resolve(Anonymous(event))));

let requireAuthentication = (respond, source) =>
  source
  |> authenticate
  |> map((. event) =>
       switch (event) {
       | Authenticated(event, user) => respond(event, user)
       | Anonymous(_event) =>
         Respond(
           Response.StatusCode.Unauthorized,
           toJson(
             Js.Json.[
               ("anonymous", boolean(true)),
               ("success", boolean(false)),
             ],
           ),
         )
       }
     );
