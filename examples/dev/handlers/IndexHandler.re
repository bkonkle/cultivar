open Authn;
open Express;
open ExpressMiddleware;
open Js.Json;
open Wonka;

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
