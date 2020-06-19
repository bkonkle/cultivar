open Config.Authn;
open Cultivar.Exchange.Infix;
open Express;
open ExpressHttp;
open Js.Json;
open JsonUtils;
open Wonka;

module Post = {
  let getOne = (id: int, _input, source) =>
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

  let handler = (id: int, input) =>
    mergeMap((. operation) =>
      switch (operation |> Authenticating.httpMethod) {
      | Get => getOne(id, input, fromValue(operation))
      | _ => notFound(input, fromValue(operation))
      }
    );

  let exchange = (id: int) => authenticate >>= handler(id);
};
