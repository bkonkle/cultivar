/**
 * Turn a list of keys and values into a JSON object.
 */
let toJson = (list: list((Js.Dict.key, 'a))) =>
  Js.Dict.fromList(list) |> Js.Json.object_;
