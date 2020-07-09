open Wonka_types;

module Exchange = {
  /**
   * Exchange input includes at least a "forward" exchangeIO operator that forwards to the next
   * exchange in the chain. It can optionally include additional parameters with context for the
   * operation.
   */
  [@genType]
  type input('operation, 'result, 'context) = {
    forward: operatorT('operation, 'result),
    context: Js.Nullable.t('context),
  };

  /**
   * An Exchange takes input and returns an operator to handle an operation.
   */
  [@genType]
  type t('operation, 'forward, 'result, 'context) =
    (. input('forward, 'result, 'context)) => operatorT('operation, 'result);

  [@genType]
  let bind =
      (
        exchange: t('a, 'b, 'result, 'context),
        next: t('b, 'c, 'result, 'context),
      ) =>
    (. input) => exchange(. {...input, forward: next(. input)});

  module Infix = {
    let (>>=) = bind;
  };
};
