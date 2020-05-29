open Express;
open Wonka;
open WonkaMiddleware;

let toJson = (list: list((Js.Dict.key, 'a))) =>
  Js.Dict.fromList(list) |> Js.Json.object_;

[@genType]
let test =
  middleware((_req, _res) =>
    Response(
      Response.StatusCode.Ok,
      toJson([("success", Js.Json.boolean(true))]),
    )
    |> fromValue
  );
