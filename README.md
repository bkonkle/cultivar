# Cultivar

Extensible TypeScript API services based on Promises and functional programming.

## Idea

Handle requests from many different sources, and compose a response using many handlers.

### Goals

- Portable: Should work with many different backends.
- Opinionated, but replaceable: Pieces should be able to be removed and substituted with other solutions without issue.
- Batteries-included: Should include a full toolkit to handle everyday API concerns.

### Sources

- HTTP requests
- Messages from a queue
- WebSocket events
- WebRTC events?

### Features

- Routing
- Authentication
- Validation
- Pagination
- Database Access
- Sessions
- Authorization
