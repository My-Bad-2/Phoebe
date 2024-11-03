#ifdef __FLT_EVAL_METHOD__
# if __FLT_EVAL_METHOD__ == -1
#  define __GLIBC_FLT_EVAL_METHOD	2
# else
#  define __GLIBC_FLT_EVAL_METHOD	__FLT_EVAL_METHOD__
# endif
#elif defined(__x86_64__)
# define __GLIBC_FLT_EVAL_METHOD	0
#else
# define __GLIBC_FLT_EVAL_METHOD	2
#endif