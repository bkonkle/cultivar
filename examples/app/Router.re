open ExpressMiddleware;

module Paths = {
  open Routes;

  let index = empty;

  let app = s("app");
  let appIndex = app /? nil;

  let tests = s("tests");
  let testId = tests / s("id") / int /? nil;
};

let routes: Routes.router(Exchange.t) =
  Routes.(
    one_of([
      Paths.index @--> IndexHandler.handle,
      Paths.appIndex @--> AppIndexHandler.handle,
      Paths.testId @--> TestIdHandler.handle,
    ])
  );
