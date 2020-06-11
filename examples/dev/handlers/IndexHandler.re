open Authn;
open Express;
open Js.Json;
open Wonka;
open WonkaMiddleware;

let handle = source =>
  source
  |> authenticate
  |> map((. event) =>
       Respond(
         Response.StatusCode.Ok,
         toJson([
           ("success", boolean(true)),
           ("isAuthenticated", boolean(isAuthenticated(event))),
         ]),
       )
     );
