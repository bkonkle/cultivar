open Routes;
open ExpressHttp;

module Paths = {
  let index = empty;

  module Posts = {
    let root = s("posts");
    let post = root / s("id") / int /? nil;
  };

  module App = {
    let root = s("app");
    let index = root /? nil;
  };
};

let routes: Routes.router(Exchange.t(Config.context)) =
  one_of([
    Paths.index @--> IndexController.Public.exchange(),
    Paths.Posts.post @--> PostController.Posts.exchange,
    Paths.App.index @--> IndexController.App.exchange(),
  ]);
