#ifndef _ASSERT_H
#define _ASSERT_H

#include <sys/defs.h>

#ifdef __cplusplus
	#define __ASSERT_VOID_CAST static_cast<void>
	#define __ASSERT_FUNCTION __PRETTY_FUNCTION__
#else
	#define __ASSERT_VOID_CAST (void)
	#define __ASSERT_FUNCTION __FUNCTION__
#endif

#define __ASSERT_FILE __FILE_NAME__
#define __ASSERT_LINE __LINE__

#ifndef DEBUG
	#define assert(...) (__ASSERT_VOID_CAST(0))
#else

__CDECLS_BEGIN
extern void __assert_fail(const char* __assertion, const char* __file, int __line,
						  const char* __function) __NO_RETURN;
__CDECLS_END

	#ifdef __cplusplus
		#define assert(...)                                                               \
			(static_cast<void>((__VA_ARGS__) ? ((void)sizeof(bool(__VA_ARGS__))) :        \
											   __assert_fail(#__VA_ARGS__, __ASSERT_FILE, \
															 __ASSERT_LINE, __ASSERT_FUNCTION)))
	#else
static inline int __check_single_argument_passed_to_assert(int b)
{
	return b;
}

		#define assert(...)                                                       \
			((void)(__check_single_argument_passed_to_assert(__VA_ARGS__) ?       \
						(void)0 :                                                 \
						__assert_fail(#__VA_ARGS__, __ASSERT_FILE, __ASSERT_LINE, \
									  __ASSERT_FUNCTION)))

	#endif
#endif

#endif // _ASSERT_H