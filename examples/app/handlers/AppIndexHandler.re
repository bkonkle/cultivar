open Authn;
open Express;
open ExpressMiddleware;
open HttpOperation;
open Js.Json;
open Wonka;

let handle = source =>
  source
  |> authenticate
  |> requireAuthentication(
       map((. event) =>
         Respond(
           Response.StatusCode.Ok,
           toJson([
             ("success", boolean(true)),
             ("isAuthenticated", boolean(true)),
             // WARNING: Uses magic.
             ("user", object_(event.user |> Obj.magic)),
           ]),
         )
       ),
     );
