#ifndef _MEMCOPY_H
#define _MEMCOPY_H 1

#include <sys/defs.h>
#include <endian.h>

#define __need_size_t
#include <stddef.h>

#define OP_T_THRES 16

typedef unsigned long int __MAY_ALIAS op_t;
typedef unsigned char byte;

#define OPSIZ (sizeof(op_t))

#if __BYTE_ORDER == __LITTLE_ENDIAN
	#define MERGE(w0, sh_1, w1, sh_2) (((w0) >> (sh_1)) | ((w1) << (sh_2)))
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
	#define MERGE(w0, sh_1, w1, sh_2) (((w0) << (sh_1)) | ((w1) >> (sh_2)))
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
	#define CMP_LT_OR_GT(a, b) ((a) > (b) ? 1 : -1)
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
extern int memcmp_bytes(op_t a, op_t b);
	#define CMP_LT_OR_GT(a, b) memcmp_bytes((a), (b))
#endif

#define BYTE_COPY_FWD(dst_bp, src_bp, nbytes) \
	do                                        \
	{                                         \
		size_t __nbytes = (nbytes);           \
		while(__nbytes > 0)                   \
		{                                     \
			byte __x = ((byte*)src_bp)[0];    \
			src_bp += 1;                      \
			__nbytes -= 1;                    \
			((byte*)dst_bp)[0] = __x;         \
			dst_bp += 1;                      \
		}                                     \
	} while(0)

#define BYTE_COPY_BWD(dst_ep, src_ep, nbytes) \
	do                                        \
	{                                         \
		size_t __nbytes = (nbytes);           \
		while(__nbytes > 0)                   \
		{                                     \
			byte __x;                         \
			src_ep -= 1;                      \
			__x = ((byte*)src_ep)[0];         \
			dst_ep -= 1;                      \
			__nbytes -= 1;                    \
			((byte*)dst_ep)[0] = __x;         \
		}                                     \
	} while(0)

extern void _wordcopy_fwd_aligned(long int, long int, size_t) __LOCAL;
extern void _wordcopy_fwd_dest_aligned(long int, long int, size_t) __LOCAL;
#define WORD_COPY_FWD(dst_bp, src_bp, nbytes_left, nbytes)                \
	do                                                                    \
	{                                                                     \
		if(src_bp % OPSIZ == 0)                                           \
		{                                                                 \
			_wordcopy_fwd_aligned(dst_bp, src_bp, (nbytes) / OPSIZ);      \
		}                                                                 \
		else                                                              \
		{                                                                 \
			_wordcopy_fwd_dest_aligned(dst_bp, src_bp, (nbytes) / OPSIZ); \
		}                                                                 \
		src_bp += (nbytes) & -OPSIZ;                                      \
		dst_bp += (nbytes) & -OPSIZ;                                      \
		(nbytes_left) = (nbytes) % OPSIZ;                                 \
	} while(0)

extern void _wordcopy_bwd_aligned(long int, long int, size_t) __LOCAL;
extern void _wordcopy_bwd_dest_aligned(long int, long int, size_t) __LOCAL;

#define WORD_COPY_BWD(dst_ep, src_ep, nbytes_left, nbytes)                \
	do                                                                    \
	{                                                                     \
		if(src_ep % OPSIZ == 0)                                           \
		{                                                                 \
			_wordcopy_bwd_aligned(dst_ep, src_ep, (nbytes) / OPSIZ);      \
		}                                                                 \
		else                                                              \
		{                                                                 \
			_wordcopy_bwd_dest_aligned(dst_ep, src_ep, (nbytes) / OPSIZ); \
		}                                                                 \
		src_ep -= (nbytes) & -OPSIZ;                                      \
		dst_ep -= (nbytes) & -OPSIZ;                                      \
		(nbytes_left) = (nbytes) % OPSIZ;                                 \
	} while(0)

#endif // MEMCOPY_H