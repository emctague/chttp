#include "request.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct request parse_request(FILE *f) {
	struct request request;
	request.headers = NULL;

	/* Get request as a string. */
	char *line = NULL;
	size_t read = 0;
	getdelim(&line, &read, ' ', f);

	if (!strcmp(line, "GET "))
		request.method = METHOD_GET;
	else	request.method = METHOD_UNKNOWN;

	/* Get request URL */
	size_t reqlen = getdelim(&line, &read, ' ', f);
	line[reqlen - 1] = '\0';

	/* Parse percent-encoding (%XX -> char from XX in hex.) */
	char *url_parsed = malloc(reqlen);
	bzero(url_parsed, reqlen);

	int out_i = 0;
	for (int i = 0; i < reqlen - 1; i++) {
		if (line[i] == '%') {
      int i1 = ++i;
      int i2 = ++i;
			char text[3] = { line[i1], line[i2], 0 };
			url_parsed[out_i] = (char)strtol(text, NULL, 16);
		} else {
			url_parsed[out_i] = line[i];
		}

		out_i++;
	}

	request.url = strdup(url_parsed);
	free(url_parsed);

	/* We dispose of the rest of the line easily. */
	getline(&line, &read, f);

	/* Get headers */
	while (getline(&line, &read, f)) {
		if (!strcmp(line, "\r\n")) break;

		char *name = strtok(line, ": ");
		char *value = strtok(line, "\r\n");
		add_header(&request.headers, name, value);
	}
	
	free(line);
	return request;
}


void free_request(struct request request) {
	free(request.url);
	free_headers(request.headers);
}
