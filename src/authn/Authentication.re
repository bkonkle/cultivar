open Wonka_types;
open ExpressMiddleware;

module Authenticated = {
  type event('user) = {
    http: Http.t,
    user: 'user,
  };

  type handler('user) = operatorT(event('user), jsonResult);
};

type event('user) =
  | Authenticating(Http.event)
  | Authenticated(Authenticated.event('user))
  | Anonymous(Http.event);

type handler('user) = operatorT(event('user), jsonResult);

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
