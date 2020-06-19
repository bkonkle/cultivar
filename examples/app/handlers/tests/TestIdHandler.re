open Config.Authn;
open Cultivar.Exchange.Infix;
open Express;
open ExpressHttp;
open Js.Json;

let map = Wonka.(map);
let toJson = JsonUtils.(toJson);

let handler = (id: int, _input, source) =>
  source
  |> map((. _event) =>
       Respond(
         Response.StatusCode.Ok,
         toJson([
           ("success", boolean(true)),
           ("id", number(float_of_int(id))),
         ]),
       )
     );

let exchange = (id: int) => authenticate >>= handler(id);
