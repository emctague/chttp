#pragma once
#include <stdarg.h>
#include <stdio.h>

#define __RSTYPE_TYPE const struct return_status_type

/* Represents a program return status. */
struct return_status_type {
	unsigned type;
	const char *fmt;
};

/* Success. */
extern __RSTYPE_TYPE RS_OK;

/* Bad command usage - prints help. */
extern __RSTYPE_TYPE RS_BAD_USAGE;

/* Unable to listen. */
extern __RSTYPE_TYPE RS_CANNOT_LISTEN;

/* Unable to bind. */
extern __RSTYPE_TYPE RS_CANNOT_BIND;

/* Unable to create socket. */
extern __RSTYPE_TYPE RS_CANNOT_MAKE_SOCKET;


/* Handle an error - va_list version. */
void vdo_error(struct return_status_type type, va_list args);


/* Handle an error. */
void do_error(struct return_status_type status, ...);

/* Throw an error if the outcome is less than 1. */
int doe_lt1(int outcome, struct return_status_type status, ...);

/* Throw an error if the outcome is not 0. */
int doe_n0(int outcome, struct return_status_type status, ...);
