#pragma once
#include "router.h"

/** Print logging information if verbosity level is met. */
void ch_log(int verbosity_level, const char *fmt, ...);

/** Begin serving CHTTP server with given verbosity level and port. */
void ch_serve(int verbosity_level, int port);

/** Add a route. */
void ch_route(char *path, pfn_handler handler);

/** Set the error 404 handler. */
void ch_route_error(pfn_handler notfound_handler);

#define CH_ROUTE_INNER(NAMING, PATH, ...) void handler_##NAMING (struct request *req, struct response *res) __VA_ARGS__; void __attribute__ ((constructor)) initialize_handler_##NAMING () { ch_route(PATH, handler_##NAMING); }
#define CH_ROUTE(PATH, ...) CH_ROUTE_INNER(__COUNTER__, PATH, __VA_ARGS__)
#define CH_ROUTE_ERROR_INNER(NAMING, ...) void ehandler_##NAMING (struct request *req, struct response *res) __VA_ARGS__; void __attribute__ ((constructor)) initialize_ehandler_##NAMING () { ch_route_error(ehandler_##NAMING); }
#define CH_ROUTE_ERROR(...) CH_ROUTE_ERROR_INNER(__COUNTER__, __VA_ARGS__)
