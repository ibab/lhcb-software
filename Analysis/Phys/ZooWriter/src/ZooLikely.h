/**
 * @file ZooLikely.h
 *
 * macros to hint at probably branches
 *
 * @author Manuel Schiller
 * @date 2012-02-19
 */

#ifndef _ZOO_LIKELY_H
#define _ZOO_LIKELY_H

// get rid of previous definitions
#ifdef UNLIKELY
#undef UNLIKELY
#endif
#ifdef LIKELY
#undef LIKELY
#endif

#ifdef __GNUC__
#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#endif // _ZOO_LIKELY_H

// vim: sw=4:tw=78:ft=cpp
