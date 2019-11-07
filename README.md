# chttp

chttp is a simple HTTP server library developed in C.
It allows for simple request routing.

## Basics

```c
#include <chttp/CHTTP.c>

// Handler for '/' returns the client's user agent.
Result indexHandler(Request req, Response res) {
  Header_set(res, "Content-type", "text/html");

  char *ua = Header_get(req, "User-agent");
  Response_printf(res, "Your user-agent is: <b>%s</b>", ua);

  return ResultOK(NULL)
}

// Handler for Error 404
Result errorHandler404(Request req, Response res) {
  Header_set(res, "Content-type", "text/html");
  Response_printf(res, "<h1>Error 404</h1><b>Route Not Found</b>");
  return ResultOK(NULL);
}

int main () {
  // Create a server.
  Server server = Server_new();

  // Set up a route.
  Server_route(server, "/", indexHandler);
  
  // Set up Error 404 Handler
  Server_route404(server, errorHandler404);

  // Try to listen on port 8080, or handle the error if one occurs.
  // The first argument to OkOr is the result type on success - Server_listen
  // doesn't return anything meaningful, so we set it to void*.
  OkOr(void*, (Server_listen(server, 8080)), {
      fprintf(stderr, "Error in Server_listen: %s\n", OkOrMessage);
  });
}
```

## Usage

chttp can be included with:

```c
#include <chttp/CHTTP.c>
```

chttp operates through a `Server` object, which is used to configure the server:

```c
Server server = Server_new();
```

### Result Type

chttp communicates error states through a special type, `Result`.
A `Result` contains either a value pointer, when successful, or an error string,
when failing. The value pointer can be NULL when no real result value is needed.

Checking for errors can be done with the `OkOr` macro, which is used as follows:

```c
OkOr(Result Type, (Statement), { Error Handler });
```

If the result of `Statement` is successful, the result value will be cast to
`Result Type` and made available as the value of the statement.

If the result is a failure, the code within `Error Handler` will be run. Within
this block, the macro `OkOrMessage` refers to the error message, as a `char*`,
and the macro `OkOrResult` refers to the entire result object.

A success result can be created using `ResultOK(value)`, and an error result
can be created with `ResultError("message")`.

### Starting the Server

To start the server, execute `Server_listen(server, port)`.
This returns a `Result<void>`, which can be handled like so:

```c
OkOr(void*, (Server_listen(server, 8080)), {
    fprintf(stderr, "%s\n", OkOrMessage);
    return 1;
});
```

The server will run until interrupted with SIGINT, or with `Server_stop(server)`.
Interrupt handlers can be disabled if necessary.

### Routing

Routes are regular functions with this signature:

```c
Result (Request, Response)
```

These are registered using the function `Server_route(server, path, handler)`.
These should return `ResultOK(NULL)`. If they return an error, the server will
fall back to the Error 500 handler.

### Error Handling

Special routes for Error 404 and 500 can be registered with
`Server_route404(server, handler)` and `Server_route500(server, handler)`.

### Configuring the Server

Before starting the server, you may configure several settings:

**Verbosity** impacts the amount of logging the server will output. '0' is the default,
and '3' will produce incredibly verbose logs. This is set with `Server_setVerbosity(server, value)`.

**Hooks** impacts whether or not the server will listen for SIGINT and handle it.
The default is `1`, and it can be changed with `Server_setHooks(server, useHooks)`.

### Checking Request Information

 * `request.path` contains the path being routed.
 * `Header_get(request, "name")` can be used to get the value of an HTTP Request header. Returns NULL on failure.
   All headers are in **lowercase**, and Header_get is case-sensitive!
 * `request.method` contains the HTTP request method being used. This can be:
   * `HTTPMethodGET` - HTTP GET request
   * `HTTPMethodPOST` - HTTP POST request
   * `HTTPMethodHEAD` - HTTP HEAD request, same as GET except returns size sans-data
   * `HTTPMethodUnknown` - Any other HTTP request method.

### Setting Response Information

 * `response.status` contains the HTTP response status code. This defaults to 200, except within error handlers.
 * `Response_write(response, void *buffer, size_t size)` can append information to the response body.
 * `Response_printf(response, "format string", ...)` can append text to the response body, using the same format as `printf`.
 * `Header_set(response, "name", "value")` can set a response header. All headers must be in LOWERCASE.

## Example

For an example, see [test.c](test.c).

## TODO

 - Add PROPER support for other request types, e.g. POST.
 - Add GET and POST parameter support
 - Add routing pattern matching, e.g. "/user/&lt;userid&gt;/profile"
 - Allow routes to filter by request method.

## Existing Errors

StringMap type is broken - insertion does not occur in proper order. No idea why, though, everything seems fine.

