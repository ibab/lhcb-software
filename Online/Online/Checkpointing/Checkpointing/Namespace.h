#ifndef CHECKPOINTING_NAMESPACE_H
#define CHECKPOINTING_NAMESPACE_H

namespace Checkpointing {
  typedef unsigned int Marker;
  typedef unsigned long VA;
};

#ifndef CHECKPOINTING_NAMESPACE 
#define CHECKPOINTING_NAMESPACE Checkpointing
#define CHECKPOINTING_HAS_NAMESPACE 1
#else
#endif

namespace CHECKPOINTING_NAMESPACE {
  enum MTCP_OUTPUT_LEVEL  { 
    MTCP_DEBUG   = 1,
    MTCP_INFO    = 2,
    MTCP_WARNING = 3,
    MTCP_ERROR   = 4,
    MTCP_FATAL   = 5,
    MTCP_MAX_LEVEL = 15,
    MTCP_NO_HEADER = 64,
    MTCP_PRINT_NO_PID = 128
  };
}

#ifdef __STATIC__
#define STATIC(x) static x
#define WEAK(x)   inline  x
#define HIDDEN(x) static inline  x
#define INLINE(x) inline x
#else
#define STATIC(x) x
#define INLINE(x) x
#define WEAK(x)   __attribute__ ((weak)) x
#define HIDDEN(x) __attribute__ ((weak)) x
#endif

#endif  // CHECKPOINTING_NAMESPACE_H
