open Config.Authn;
open Cultivar.Exchange.Infix;
open Express;
open ExpressHttp;
open Js.Json;

let map = Wonka.(map);
let toJson = JsonUtils.(toJson);

module Public = {
  let handler = _input =>
    map((. event) =>
      Respond(
        Response.StatusCode.Ok,
        toJson([
          ("success", boolean(true)),
          (
            "isAuthenticated",
            boolean(Authentication.isAuthenticated(. event)),
          ),
        ]),
      )
    );

  let exchange = () => authenticate >>= handler;
};

module App = {
  let handler = _input =>
    map((. event: Authenticated.operation(user)) =>
      Respond(
        Response.StatusCode.Ok,
        toJson([
          ("success", boolean(true)),
          ("isAuthenticated", boolean(true)),
          // WARNING: Uses magic.
          ("user", object_(event.user |> Obj.magic)),
        ]),
      )
    );

  let exchange = () => authenticate >>= requireAuthentication >>= handler;
};
