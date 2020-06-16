open Express;
open ExpressMiddleware;
open Wonka_types;
open Wonka;

/**
 * Take a set of Routes that use ExpressMiddleware handlers, and return a single handler that will
 * route to the appropriate one based on the request path.
 */
let router = (routes: Routes.router(Exchange.t)): Exchange.t =>
  (source, sink) =>
    source((. signal) => {
      switch (signal) {
      | Start(tb) => sink(. Start(tb))
      | Push(event) =>
        switch (Routes.match'(routes, ~target=event.http.req |> Request.path)) {
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
    });
