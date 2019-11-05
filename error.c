#include "error.h"
#include <stdlib.h>

__RSTYPE_TYPE RS_OK = {0, ""};


__RSTYPE_TYPE RS_BAD_USAGE = {1,
	"Usage: %s [options]\n"
	"Options:\n"
	"--port, -p [port]    Set the port. (default = 8080)\n"
	"--verbose, -v        Log more information (repeat for more)\n"
};

__RSTYPE_TYPE RS_CANNOT_LISTEN = {2, "Unable to listen\n"};

__RSTYPE_TYPE RS_CANNOT_BIND = {3, "Unable to bind to port: %d\n"};

__RSTYPE_TYPE RS_CANNOT_MAKE_SOCKET = {4, "Unable to create socket\n"};


void vdo_error(struct return_status_type status, va_list args)
{
	if (status.type != 0) vfprintf(stderr, status.fmt, args);
	exit(status.type);
}

void do_error(struct return_status_type status, ...)
{
	va_list args;
	va_start(args, status);
	vdo_error(status, args);
	va_end(args);
}

int doe_lt1(int outcome, struct return_status_type status, ...)
{
	if (outcome < 1) {
		va_list args;
		va_start(args, status);
		vdo_error(status, args);		
		va_end(args);
	}

	return outcome;	
}


int doe_n0(int outcome, struct return_status_type status, ...)
{
	if (outcome) {
		va_list args;
		va_start(args, status);
		vdo_error(status, args);		
		va_end(args);
	}

	return outcome;	
}


