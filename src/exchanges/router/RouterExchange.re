open Express;
open ExpressHttp;
open JsonUtils;
open Wonka_types;
open Wonka;

let router =
    (routes: Routes.router(Exchange.t('context))): Exchange.t('context) =>
  (input, source, sink) =>
    source((. signal) => {
      switch (signal) {
      | Start(tb) => sink(. Start(tb))
      | Push(event) =>
        switch (Routes.match'(routes, ~target=event.http.req |> Request.path)) {
        | Some(exchange) => sink |> exchange(input, fromValue(event))
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
