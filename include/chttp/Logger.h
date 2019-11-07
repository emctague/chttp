#pragma once

/** A logger prints structured logs of events. */
typedef int Verbosity;

/** Returns whether or not the logger would log a message with the given verbosity. */
int L_will_log(Verbosity logger, int verbosity);

/** Log an event if it meets the minimum verbosity. */
void L_log(Verbosity logger, int verbosity, const char *fmt, ...);

