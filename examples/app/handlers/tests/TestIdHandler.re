open Express;
open Js.Json;
open Wonka;
open ExpressMiddleware;
open HttpOperation;

let handle = (id: int) =>
  map((. _event) =>
    Respond(
      Response.StatusCode.Ok,
      toJson([
        ("success", boolean(true)),
        ("id", number(float_of_int(id))),
      ]),
    )
  );
