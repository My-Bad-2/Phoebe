#ifndef LIBS_TRACE_H
#define LIBS_TRACE_H 1

#include <sys/defs.h>

__CDECLS_BEGIN

/**
 * @brief Logs a stack trace from the current stack frame up to a specified maximum depth.
 *
 * This function traverses the call stack, logging each frame's return address to aid in
 * debugging by providing a snapshot of the current execution path.
 */
void dump_stacktrace(void);

__CDECLS_END

#endif // LIBS_TRACE_H