#ifndef BITS_IOPORT_H
#define BITS_IOPORT_H

// Framework include files
#include "Framework/AVX.h"
#include <algorithm>

namespace Framework   {
  typedef std::pair<const __m128i*,size_t> BitRegisters;
  typedef AVXMask<1> IOMask;
  typedef AVXMask<1> InstanceMask;
}

#endif // BITS_IOPORT_H
