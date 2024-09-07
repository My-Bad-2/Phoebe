#ifndef _SYS_DEFS_H
#define _SYS_DEFS_H

#ifdef __cplusplus
	#define __CDECLS_BEGIN \
		extern "C"         \
		{
	#define __CDECLS_END }
#else
	#define __CDECLS_BEGIN
	#define __CDECLS_END
#endif

#ifdef __cplusplus
	#define restrict __restrict
#endif

#ifndef __STRINGIFY
	#define __STRINGIFY(a, b) a##b
#endif

#ifndef __has_cpp_attribute
	#define __has_cpp_attribute(x) 0
#endif

#ifndef __has_feature
	#define __has_feature(x) 0
#endif

#define __NO_INLINE __attribute__((__noinline__))
#define __ALWAYS_INLINE __attribute__((__always_inline__))
#define __USED __attribute__((used))

#ifndef __cplusplus
	#define __UNUSED __attribute__((__unused__))
#endif

#define __PACKED __attribute__((__packed__))
#define __ALIGNED(x) __attribute__((aligned(x)))
#define __NO_RETURN __attribute__((__noreturn__))
#define __WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))

#if defined(__clang__)
	#define __UNINITIALIZED __attribute__((uninitialized))
#else
	#define __UNINITIALIZED
#endif

#if defined(__cplusplus)
	#if __cplusplus >= 201703L && (defined(__clang__) || (defined(__GNUC__) && (__GNUC__ >= 10)))
		#define __WARN_UNUSED_CONSTRUCTOR [[nodiscard]]
	#else
		#define __WARN_UNUSED_CONSTRUCTOR
	#endif
#endif

#if !defined(__DEPRECATE)
	#define __DEPRECATE __attribute__((__deprecated__))
#endif

#define __PURE __attribute__((__pure__))
#define __CONST __attribute__((__const__))
#define __MALLOC __attribute__((__malloc__))

#if defined(__clang__)
	#define __ALLOC_SIZE(x, ...) __attribute__((__alloc_size__(x, ##__VA_ARGS__)))
#else
	#define __ALLOC_SIZE(x, ...)
#endif

#define __PRINTFLIKE(__fmt, __varargs) __attribute__((__format__(__printf__, __fmt, __varargs)))
#define __SCANFLIKE(__fmt, __varargs) __attribute__((__format__(__scanf__, __fmt, __varargs)))
#define __NONNULL(n) __attribute__((__nonnull__ n))
#define __LEAF_FN __attribute__((__leaf__))

#if !defined(__clang__)
	#define __OPTIMIZE(x) __attribute__((__optimize__(x)))
#else
	#define __OPTIMIZE(x)
#endif

#if defined(__clang__)
	#define __NO_SAFESTACK __attribute__((__no_sanitize__("safe-stack", "shadow-call-stack")))
#else
	#define __NO_SAFESTACK
#endif

#if __has_cpp_attribute(no_unique_address)
	#define __NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
	#define __NO_UNIQUE_ADDRESS
#endif

#if defined(__clang__)
	#define __ANALYZER_CREATE_SINK __attribute__((analyzer_noreturn))
#else
	#define __ANALYZER_CREATE_SINK
#endif

#if !defined(__clang__)
	#define __EXTERNALLY_VISIBLE __attribute__((__externally_visible__))
#else
	#define __EXTERNALLY_VISIBLE
#endif

#define __ALIAS(x) __attribute__((__alias__(x)))
#define __SECTION(x) __attribute__((__section__(x)))
#define __WEAK __attribute__((__weak__))
#define __WEAK_ALIAS(x) __attribute__((__weak__, __alias__(x)))
#define __ALWAYS_EMIT __attribute__((__used__))
#define __EXPORT __attribute__((__visibility__("default")))
#define __LOCAL __attribute__((__visibility__("hidden")))

#ifndef likely
	#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
	#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define __GET_CALLER(x) __builtin_return_address(0)
#define __GET_FRAME(x) __builtin_frame_address(0)
#define __ISCONSTANT(x) __builtin_constant_p(x)
#define __UNREACHABLE() __builtin_unreachable()
#define __offsetof(type, field) __builtin_offsetof(type, field)

#define add_overflow(a, b, result) __builtin_add_overflow(a, b, result)
#define sub_overflow(a, b, result) __builtin_sub_overflow(a, b, result)
#define mul_overflow(a, b, result) __builtin_mul_overflow(a, b, result)

#if defined(__cplusplus) && __cplusplus >= 201703L
	#define __FALLTHROUGH [[fallthrough]]
#elif defined(__cplusplus) && defined(__clang__)
	#define __FALLTHROUGH [[clang::fallthrough]]
#elif __GNUC__ >= 7 || (!defined(__cplusplus) && defined(__clang__))
	#define __FALLTHROUGH __attribute__((__fallthrough__))
#endif

#ifndef __OWNER
	#ifdef __clang__
		#define __OWNER(x) [[gsl::Owner(x)]]
		#define __POINTER(x) [[gsl::Pointer(x)]]
	#else
		#define __OWNER(x)
		#define __POINTER(x)
	#endif
#endif

#if defined(__cpp_constinit) && __cpp_constinit >= 201907L
	#define __CONSTINIT constinit
#elif defined(__clang__)
	#define __CONSTINIT [[clang::require_constant_initialization]]
#elif defined(__GNUC__) && __GNUC__ >= 13
	#define __CONSTINIT __constinit
#endif

#endif // _SYS_DEFS_H