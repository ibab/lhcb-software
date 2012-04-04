#ifndef CHECKPOINTING_NAMESPACE_H
#define CHECKPOINTING_NAMESPACE_H

namespace Checkpointing {
  typedef unsigned int Marker;
  typedef unsigned long VA;
}

#ifndef CHECKPOINTING_NAMESPACE 
#define CHECKPOINTING_NAMESPACE Checkpointing
#define CHECKPOINTING_HAS_NAMESPACE 1
#else
#endif

namespace CHECKPOINTING_NAMESPACE {
  enum MTCP_OUTPUT_LEVEL  { 
    MTCP_DEBUG        = 1,
    MTCP_INFO         = 2,
    MTCP_WARNING      = 3,
    MTCP_ERROR        = 4,
    MTCP_ALWAYS       = 5,
    MTCP_FATAL        = 6,
    MTCP_MAX_LEVEL    = 15,
    MTCP_SPARE_FLAG0  = (1<<4),  // = 16
    MTCP_SPARE_FLAG1  = (1<<5),  // = 32
    MTCP_NO_HEADER    = (1<<6),  // = 64
    MTCP_PRINT_NO_PID = (1<<7)   // = 128
  };
  enum MTCP_RESTORE_FLAGS {
    MTCP_STDIN_ENV    = (1<<0),  // = 1
    MTCP_SAVE_LIBS    = (1<<1),  // = 2
    MTCP_LAST_FLAG    = (1<<31)
  };
}

#ifdef __STATIC__
#define STATIC(x) static x
#define HIDDEN(x) static inline  x
#define INLINE(x) inline x

#ifdef __INTEL_COMPILER
#define WEAK(x)   x
#else
#define WEAK(x)   inline  x
#endif

#else

#define STATIC(x) x
#define INLINE(x) x

#ifdef __INTEL_COMPILER
#define WEAK(x)   x
#define HIDDEN(x) x
#else
#define WEAK(x)   __attribute__ ((weak)) x
#define HIDDEN(x) __attribute__ ((weak)) x
#endif

#endif

#endif  // CHECKPOINTING_NAMESPACE_H
