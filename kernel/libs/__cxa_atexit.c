#include <stddef.h>

#define ATEXIT_MAX_FUNCS 128

struct AtExitFuncs
{
	void (*destructor_func)(void*);
	void* obj_ptr;
	void* dso_handle;
} __atexit_funcs[ATEXIT_MAX_FUNCS];

unsigned int __atexit_func_count = 0;
void* __dso_handle = nullptr;

int __cxa_atexit(void (*func)(void*), void* objptr, void* dso)
{
	if(__atexit_func_count >= ATEXIT_MAX_FUNCS)
	{
		return -1;
	}

	__atexit_funcs[__atexit_func_count].destructor_func = func;
	__atexit_funcs[__atexit_func_count].obj_ptr = objptr;
	__atexit_funcs[__atexit_func_count].dso_handle = dso;
	__atexit_func_count++;

	return 0;
}