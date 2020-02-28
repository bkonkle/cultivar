open Express;
open Js.Promise;

[@genType]
let test =
  Middleware.from((_next, _req, res) => {
    Postgres.getPgClient(
      "postgres://allay_root:password@localhost:5432/allay",
    )
    |> then_(pgClient => resolve(Js.log2("pgClient", pgClient)))
    |> catch(error => resolve(Js.Console.error(Obj.magic(error))))
    |> ignore;

    res |> Response.sendString("Yay!");
  });
