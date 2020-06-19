open Config.Authn;
open Cultivar.Exchange.Infix;
open Express;
open ExpressHttp;
open Js.Json;
open JsonUtils;
open Wonka;

module Post = {
  let getOne = (id: int, _input) =>
    map((. _event) =>
      Respond(
        Response.StatusCode.Ok,
        toJson([
          ("success", boolean(true)),
          ("id", number(float_of_int(id))),
        ]),
      )
    );

  let switcher = (id: int, input, httpMethod: Request.httpMethod) =>
    switch (httpMethod) {
    | Get => getOne(id, input)
    | _ => notFound(input)
    };

  let exchange = (id: int) =>
    authenticate >>= Authentication.byMethod(switcher(id));
};
