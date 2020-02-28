open Js.Option;
open Js.Nullable;
open Js.Promise;

module PgClient = {
  type t;

  [@bs.module "pg"] [@bs.new] external make: 'a => t = "Client";

  [@bs.send]
  external connect: (t, Js.Nullable.t(exn) => unit) => unit = "connect";

  [@bs.send] external end_: (t, Js.Nullable.t(exn) => unit) => unit = "end";

  [@bs.send]
  external on: (t, string, Js.Nullable.t(exn) => unit) => unit = "on";

  [@bs.send] external release: (t, unit) => Js.Promise.t(unit) = "release";

  [@bs.set]
  external set_release: (t, unit => Js.Promise.t(unit)) => unit = "release";
};

module PgPool = {
  type t;

  [@bs.module "pg"] [@bs.new] external make: 'a => t = "Pool";

  [@bs.send]
  external connect: (t, unit) => Js.Promise.t(PgClient.t) = "connect";
};

let isObject = obj =>
  Js.typeof(obj) == "object"
  && !Js.Array.isArray(obj)
  && !((Obj.magic(obj): Js.null('a)) === Js.null);

let isFunction = func => Js.typeof(func) === "function";

let constructorName = obj =>
  isFunction(obj##constructor) ? obj##constructor##name : "";

let maybePgClient = (pg): Js.Option.t(PgClient.t) =>
  isObject(pg)
  && constructorName(pg) === "Client"
  && isFunction(pg##connect)
  && isFunction(pg##_end)
  && isFunction(pg##escapeLiteral)
  && isFunction(pg##escapeIdentifier)
    ? Some(Obj.magic(pg)) : None;

let fromPgClient = (pg: PgClient.t) => {
  if (Js.isNullable(Obj.magic(pg)##release)) {
    Js.Exn.raiseError(
      "Only pg.Client instances from a pg.Pool are supported",
    );
  };
  resolve(pg);
};

let maybePgPool = (pg): Js.Option.t(PgPool.t) =>
  isObject(pg)
  && (constructorName(pg) === "Pool" || constructorName(pg) === "BoundPool")
  && !Js.isNullable(pg##_Client)
  && !Js.isNullable(pg##options)
  && isFunction(pg##connect)
  && isFunction(pg##_end)
  && isFunction(pg##query)
    ? Some(Obj.magic(pg)) : None;

let fromPgPool = (pg: PgPool.t) => pg->PgPool.connect();

let maybeString = (pg): Js.Option.t(string) =>
  Js.typeof(pg) === "string" ? Some(Obj.magic(pg)) : None;

let fromString = (pg: string) => {
  open PgClient;

  let client = PgClient.make(pg);

  client->on("error", e => Js.Console.error2("pg.Client error:", e));

  let release = () =>
    Js.Promise.make((~resolve, ~reject) => {
      end_(client, err =>
        switch (toOption(err)) {
        | Some(error) => reject(. error)
        | _ => resolve(. Js.null)
        }
      )
    })
    // Woakround an issue where `resolve(.)` caused type errors
    |> then_(_ => Js.Promise.resolve());

  client->PgClient.set_release(release);

  resolve(client);
};

let getPgClient = pg =>
  try(
    BsAbstract.Option.Infix.(
      maybeString(Obj.magic(pg))
      <#> (str => fromString(str))
      <|> (
        maybePgPool(Obj.magic(pg))
        <#> (pool => fromPgPool(pool))
        <|> (
          maybePgClient(Obj.magic(pg)) <#> (client => fromPgClient(client))
        )
      )
      |> getExn
    )
  ) {
  | _ =>
    Js.Exn.raiseError(
      "You must provide either a pg.Pool or pg.Client instance, or a connection string",
    )
  };
