module rec Types: {
  module Authenticated: {
    type operation('user) = {
      http: ExpressHttp.event,
      user: 'user,
    };
  };
  module Authentication: {
    type operation('user) =
      | Authenticating(ExpressHttp.operation)
      | Anonymous(ExpressHttp.operation)
      | Authenticated(Authenticated.operation('user));
  };
} = Types;

module Authenticated = {
  include Types.Authenticated;

  let toAuthenticating =
    (. operation: operation('user)) =>
      Types.Authentication.Authenticated(operation);

  let toHttp = (. operation) => operation.http;

  let httpMethod = operation =>
    toHttp(. operation).req |> Express.Request.httpMethod;

  let handleByMethod = (handler, input) =>
    Wonka.(
      mergeMap((. operation) =>
        fromValue(operation) |> handler(input, operation |> httpMethod)
      )
    );
};

module Authentication = {
  include Types.Authentication;

  let isAuthenticated =
    (. operation) =>
      switch (operation) {
      | Authenticated(_) => true
      | _ => false
      };

  let toOption =
    (. operation) =>
      switch (operation) {
      | Authenticated(event) => Some(event)
      | _ => None
      };

  let toEither =
    (. operation) =>
      Either.(
        switch (operation) {
        | Authenticated(_) => Right
        | _ => Left
        }
      );

  let toHttp =
    (. operation) =>
      switch (operation) {
      | Authenticating(event) => event
      | Anonymous(event) => event
      | Authenticated(event) => {http: event.http}
      };

  let fromHttp =
    (. operation: ExpressHttp.operation) => Authenticating(operation);

  let toAnonymous =
    (. operation: ExpressHttp.operation) => Anonymous(operation);

  let httpMethod = operation =>
    toHttp(. operation).http.req |> Express.Request.httpMethod;

  let handleByMethod = handler =>
    (. input) => {
      let byMethod = handler(. input);
      Wonka.(
        mergeMap((. operation) =>
          fromValue(operation) |> byMethod(. operation |> httpMethod)
        )
      );
    };
};
