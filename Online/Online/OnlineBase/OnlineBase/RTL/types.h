#ifndef RTL_TYPES_H
#define RTL_TYPES_H

#include <sys/types.h>

#ifdef _WIN32
typedef unsigned  __int64 u_int64_t;
typedef unsigned  __int32 u_int32_t;
typedef unsigned  __int16 u_int16_t;
typedef unsigned  __int8  u_int8_t;

typedef           __int64 int64_t;
typedef           __int32 int32_t;
typedef           __int16 int16_t;
typedef           __int8  int8_t;

#else

#include <sys/types.h>

#endif

#endif // RTL_TYPES_H
