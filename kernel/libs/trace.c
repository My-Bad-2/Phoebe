#include <libs/trace.h>
#include <logger.h>

#define STACKTRACE_MAX 64

struct Stackframe
{
	struct Stackframe* next;
	void* return_address;
};

void dump_stacktrace(void)
{
	struct Stackframe* frame = (struct Stackframe*)(__GET_FRAME(0));
	void* address = NULL;

	int i = 0;

	while((frame != NULL) && (i++ < STACKTRACE_MAX))
	{
		address = frame->return_address;
		frame = frame->next;

		if(address == NULL)
		{
			break;
		}

		log_trace("Frame #%d -> %p", i, address);
	}
}