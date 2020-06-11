open Authn;
open Express;
open Js.Json;
open Wonka;
open WonkaMiddleware;

let handle = source =>
  source
  |> authenticate
  |> requireAuthentication(
       map((. _event) =>
         Respond(
           Response.StatusCode.Ok,
           toJson([
             ("success", boolean(true)),
             ("isAuthenticated", boolean(true)),
           ]),
         )
       ),
     );
