open Wonka;
open Wonka_types;

type t =
  | Right
  | Left;

/**
 * Applies one Wonka operator or the other to a source based on a predicate that returns
 * an Either.
 */
[@genType]
let either =
    (
      ~test: 'operation => t,
      ~left: operatorT('operation, 'result),
      ~right: operatorT('operation, 'result),
    )
    : operatorT('operation, 'result) =>
  curry(source =>
    curry(sink =>
      source((. signal) => {
        switch (signal) {
        | Start(tb) => sink(. Start(tb))
        | Push(event) =>
          let handler =
            switch (test(event)) {
            | Left => left
            | Right => right
            };
          handler(fromValue(event), sink);
        | End => sink(. End)
        }
      })
    )
  );

[@genType]
let right = () => Right;

[@genType]
let left = () => Left;
