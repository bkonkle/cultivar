open Express;
open Wonka;
open Wonka_types;
open WonkaMiddleware;

/**
 * Take a set of routes that use WonkaMiddleware handlers, and return a single handler that will
 * route to the appropriate one based on the request path.
 */
let router = (routes: Routes.router(handler)): handler =>
  curry(source =>
    curry(sink =>
      source((. signal) => {
        switch (signal) {
        | Start(tb) => sink(. Start(tb))
        | Push(event) =>
          switch (Routes.match'(routes, ~target=event.req |> Request.path)) {
          | Some(handler) => handler(fromValue(event), sink)
          | None =>
            sink(.
              Push(
                Respond(
                  Response.StatusCode.NotFound,
                  toJson(
                    Js.Json.[
                      ("success", boolean(false)),
                      ("error", string("Not found")),
                    ],
                  ),
                ),
              ),
            )
          }
        | End => sink(. End)
        }
      })
    )
  );
