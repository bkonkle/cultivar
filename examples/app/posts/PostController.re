open Config.Authn;
open Cultivar.Exchange.Infix;
open Express;
open ExpressHttp;
open Js.Json;
open JsonUtils;
open Wonka;

module Posts = {
  let getOne = (id: int) =>
    map((. _) =>
      Respond(
        Response.StatusCode.Ok,
        toJson([
          ("success", boolean(true)),
          ("id", number(float_of_int(id))),
        ]),
      )
    );

  let handler = (id: int) =>
    (. _) =>
      (. httpMethod: Request.httpMethod) =>
        switch (httpMethod) {
        | Get => getOne(id)
        | _ => notFound(~message=Js.Nullable.null)
        };

  let exchange = (id: int) =>
    authenticate >>= Authentication.handleByMethod(handler(id));
};
