open Config.Authn;
open Cultivar.Exchange.Infix;
open Express;
open ExpressHttp;
open Js.Json;

let map = Wonka.(map);
let toJson = JsonUtils.(toJson);

let handler = (_input, source) =>
  source
  |> map((. event) =>
       Respond(
         Response.StatusCode.Ok,
         toJson([
           ("success", boolean(true)),
           (
             "isAuthenticated",
             boolean(Authenticating.isAuthenticated(. event)),
           ),
         ]),
       )
     );

let exchange = () => authenticate >>= handler;
