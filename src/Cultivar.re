open Express;
open Wonka;
open WonkaMiddleware;
open AuthenticateOperator;

[@genType]
let test =
  middleware(
    requireAuthentication(
      map((. (_event, _user)) =>
        Respond(
          Response.StatusCode.Ok,
          toJson(
            Js.Json.[
              ("success", boolean(true)),
              ("anonymous", boolean(false)),
            ],
          ),
        )
      ),
    ),
  );
