open Express;
open WonkaMiddleware;
open EventOperators;

[@genType]
let test =
  middleware(source =>
    source
    |> requireAuthentication((_event, _user) =>
         Respond(
           Response.StatusCode.Ok,
           toJson(
             Js.Json.[
               ("success", boolean(true)),
               ("anonymous", boolean(false)),
             ],
           ),
         )
       )
  );
