open Routes;
open ExpressHttp;

module Paths = {
  let index = empty;

  let app = s("app");
  let appIndex = app /? nil;

  let tests = s("tests");
  let testId = tests / s("id") / int /? nil;
};

let routes: Routes.router(Exchange.t(Config.context)) =
  one_of([
    Paths.index @--> IndexHandler.exchange(),
    Paths.appIndex @--> AppIndexHandler.exchange(),
    Paths.testId @--> TestIdHandler.exchange,
  ]);
