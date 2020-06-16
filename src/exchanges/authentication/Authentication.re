module type User = {type t;};

type authenticatedEvent('user) = {
  http: HttpOperation.t,
  user: 'user,
};

type event('user) =
  | Authenticating(HttpOperation.event)
  | Anonymous(HttpOperation.event)
  | Authenticated('user);

module AuthenticatedOperation = {
  module Make = (User: User) => {
    type event = authenticatedEvent(User.t);
    type result = HttpOperation.result;
  };
};

let isAuthenticated = event =>
  switch (event) {
  | Authenticated(_) => true
  | _ => false
  };

let toOption = event =>
  switch (event) {
  | Authenticated(event) => Some(event)
  | _ => None
  };

let toEither = event =>
  Either.(
    switch (event) {
    | Authenticated(_) => Right
    | _ => Left
    }
  );

let toHttp =
  (. event) =>
    switch (event) {
    | Authenticating(event) => event
    | Anonymous(event) => event
    | Authenticated(event) => {http: event.http}
    };

let fromHttp = (. event) => Authenticating(event);
