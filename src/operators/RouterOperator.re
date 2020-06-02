open Express;
open Wonka;
open WonkaMiddleware;

type route = string;

// TODO: A route is a combination of a path and a handler

let sum = () => Routes.(s("sum") / int / int /? nil);

let handler: (int, int) => handler =
  (a, b, source) =>
    source
    |> map((. _event) =>
         Respond(
           Response.StatusCode.Ok,
           toJson(
             Js.Json.[
               ("success", boolean(true)),
               ("anonymous", boolean(false)),
             ],
           ),
         )
       );

let sumRoute = () => Routes.(sum() @--> handler);

let routes = () => Routes.one_of([sumRoute()]);

/**
 * Take multiple handlers with route specifiers, and decide which one to invoke with the event.
 */
let router = (handlers: list((route, handler)), event) =>
  event |> (handlers |> List.map(((_route, handler)) => handler) |> List.hd);
