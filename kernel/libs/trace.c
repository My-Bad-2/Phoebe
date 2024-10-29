#include <libs/trace.h>
#include <logger.h>

#define STACKTRACE_MAX 64

struct Stackframe
{
	struct Stackframe* next; ///< Pointer to the next stack frame in the chain.
	void* return_address; ///< Return address of the current stack frame.
};

/**
 * @brief Dumps the stack trace by traversing linked stack frames up to a defined maximum.
 *
 * The function reads the frame pointer to access the current stack frame and iterates through
 * the stack, logging each frame's return address until the limit defined by `STACKTRACE_MAX`
 * or a NULL frame is encountered.
 */
void dump_stacktrace(void)
{
	struct Stackframe* frame = (struct Stackframe*)(__GET_FRAME(0)); // Start with current frame
	void* address = NULL;

	int i = 0;

	while((frame != NULL) && (i++ < STACKTRACE_MAX))
	{
		address = frame->return_address;
		frame = frame->next;

		if(address == NULL)
		{
			break; // Stop if there is no return address
		}

		log_trace("Frame #%d -> %p", i, address); // Log the frame index and address
	}
}