#pragma once

/** Represents a success or failure result. */
typedef struct Result {
  void *result; /**< The result, if an error did not occur. */
  char *message; /**< The error message, if an error occurred, or NULL otherwise. */
} Result;

/** Shorthand to produce an OK result. */
#define ResultOK(VALUE) ((Result){(void*)VALUE, NULL})

/** Shorthand to produce an error result. */
#define ResultError(MESSAGE) ((Result){NULL, MESSAGE})

/** Attempt an expression, and continue as normal (casting result to TYPE)
 * if successful, or execute error-handling block if failed.
 * Note: enclose EXPRESSION in parentheses. */
#define OkOr(TYPE, EXPRESSION, IFBAD) ({ \
  Result OKOR_temp = EXPRESSION; \
  if (OKOR_temp.message) {IFBAD;} \
  (TYPE)OKOR_temp.result; \
})

/** Value to access the Result object from within the IFBAD block of Okor. */
#define OkOrResult OKOR_temp

/** Value to access the error message from within the IFBAD block of OkOr. */
#define OkOrMessage OKOR_temp.message

