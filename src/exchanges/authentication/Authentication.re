module rec Types: {
  module Authenticated: {
    type operation('user) = {
      http: ExpressHttp.event,
      user: 'user,
    };
  };
  module Authenticating: {
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
      Types.Authenticating.Authenticated(operation);
};

module Authenticating = {
  include Types.Authenticating;

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
};
