open Wonka_types;

/***
 * An Exchange is responsible for receiving an observable Operation event and returning an
 * Operation result.
 */

/**
 * An Operation has an event type and a result type.
 */
module type Operation = {
  type event;
  type result;
};

/**
 * Make an Exchange module based on an Operation.
 */
module Make = (Op: Operation) => {
  type t = operatorT(Op.event, Op.result);

  type input = {
    forward: t,
    event: Op.event,
  };

  type exchange = input => t;
};
