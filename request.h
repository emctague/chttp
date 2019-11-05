#pragma once
#include "http.h"
#include <stdio.h>

/** Represents an HTTP request. */
struct request
{
	/** The HTTP method being used. */
	enum method method;

	/** The path being requested - does not include domain, protocol, etc. */
	char *url;

	/** A linked list of HTTP headers included in the request. */
	struct header *headers;
};


/** Parse and return an HTTP request.
 * Parameters:
 * f - The socket, opened with fdopen, to read from.
 * Returns:
 * A request object, which should be cleaned up with free_request.
 */
struct request parse_request(FILE *f);

/** Clean up a request.
 * Parameters:
 * request - The request to clean up.
 */
void free_request(struct request request);

