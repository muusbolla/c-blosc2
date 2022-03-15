/*********************************************************************
  Blosc - Blocked Shuffling and Compression Library

  Copyright (C) 2021  The Blosc Developers <blosc@blosc.org>
  https://blosc.org
  License: BSD 3-Clause (see LICENSE.txt)

  See LICENSE.txt for details about copyright and rights to use.
**********************************************************************/

/* Bit Manipulation Instruction intrinsics
** Different environments provide intrinsics that are nearly identical with different names.
** This file simplifies choosing an available intrinsic that implements a BMI operation.
** Include this file AFTER other headers that provide intrinsics definitions.
*/

#ifndef BLOSC_BMI_H
#define BLOSC_BMI_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLOSC_CTZ32
#if defined(__has_builtin) && __has_builtin(__builtin_ctz)
#define BLOSC_CTZ32(x) __builtin_ctz(x)  // clang/gcc builtin
#elif defined(_mm_tzcnt_32)
#define BLOSC_CTZ32(x) _mm_tzcnt_32(x)  // generic tzcnt
#elif defined(_tzcnt_u32)
#define BLOSC_CTZ32(x) (int32_t) _tzcnt_u32((uint32_t) x)  // intel tzcnt
#elif defined(__tzcnt_u32)
#define BLOSC_CTZ32(x) (int32_t) __tzcnt_u32((uint32_t) x)  // AMD tzcnt
#elif defined(_BitScanForward)
// fallback to BSF instruction, should be good enough for our use cases
// we won't encounter the undefined result when x == 0 with proper guarding
static int BLOSC_CTZ32(unsigned int x) {
    unsigned int i;
    _BitScanForward(&i, x);
    return (int) i;
}
#else
// fallback loop implementation
static int BLOSC_CTZ32(unsigned int x) {
    int i = 0;
    for (; i < 32; ++i) {
        if (x & (1 << i)) {
            break;
        }
    }
    return i;
}
#endif
#endif /* BLOSC_CTZ32 */

#ifndef BLOSC_CTZ64
#if defined(__has_builtin) && __has_builtin(__builtin_ctzll)
#define BLOSC_CTZ64(x) __builtin_ctzll(x)  // clang/gcc builtin
#elif defined(_mm_tzcnt_64)
#define BLOSC_CTZ64(x) _mm_tzcnt_64(x)  // generic tzcnt
#elif defined(_tzcnt_u64)
#define BLOSC_CTZ64(x) (int64_t) _tzcnt_u64((uint64_t) x)  // intel tzcnt
#elif defined(__tzcnt_u64)
#define BLOSC_CTZ64(x) (int64_t) __tzcnt_u64((uint64_t) x)  // AMD tzcnt
#elif defined(_BitScanForward)
// fallback to BSF instruction, should be good enough for our use cases
// we won't encounter the undefined result when x == 0 with proper guarding
static int64_t BLOSC_CTZ64(uint64_t x) {
    uint64_t i;
    _BitScanForward64(&i, x);
    return (int64_t) i;
}
#else
// fallback loop implementation
static int64_t BLOSC_CTZ64(uint64_t x) {
    int64_t i = 0;
    for (; i < 64; ++i) {
        if (x & (1ULL << i)) {
            break;
        }
    }
    return i;
}
#endif
#endif /* BLOSC_CTZ64 */

#ifndef BLOSC_BLSR64
#ifdef _blsr_u64
#define BLOSC_BLSR64(x) _blsr_u64(x); // intel blsr
#elif defined(__blsr_u64)
#define BLOSC_BLSR64(x) __blsr_u64(x); // amd blsr
#endif // no fallback. use "x & ~(1ULL << bitindex)" at call site
#endif /* BLOSC_BLSR64 */

#ifndef BLOSC_POPCNT64
#if defined(__has_builtin) && __has_builtin(__builtin_popcountll)
#define BLOSC_POPCNT64(x) __builtin_popcountll(x);
#elif defined(_mm_popcnt_u64)
#define BLOSC_POPCNT64(x) _mm_popcnt_u64(x);
#elif defined(_popcnt64)
#define BLOSC_POPCNT64(x) _popcnt64(x);
#elif defined(__popcntq)
#define BLOSC_POPCNT64(x) __popcntq(x);
#elif defined(__popcnt64)
#define BLOSC_POPCNT64(x) __popcnt64(x);  // microsoft
#endif // no fallback
#endif /* BLOSC_POPCNT64 */

#ifdef __cplusplus
}
#endif

#endif /* BLOSC_BMI_H */
