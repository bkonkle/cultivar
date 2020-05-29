open Express;
open WonkaMiddleware;
open EventOperators;

[@genType]
let test =
  middleware(source =>
    source
    |> authenticate
    |> requireAuthentication((_event, _user) =>
         Respond(
           Response.StatusCode.Ok,
           toJson([("success", Js.Json.boolean(true))]),
         )
       )
  );
