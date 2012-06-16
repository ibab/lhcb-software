#ifndef VL_RAW_BANK_CONSTANTS_H 
#define VL_RAW_BANK_CONSTANTS_H 1

#include <sys/types.h>

namespace VLDAQ {
  typedef u_int32_t bufferWord;
  // Size of a cluster word in bytes
  const size_t clusterWordSize = sizeof(unsigned short);
  // Number of cluster words per buffer word
  const size_t clustersPerBuffer = sizeof(bufferWord) / clusterWordSize;
  // Power of two that yields the number of bits in a cluster word,
  // needed for shifting
  const size_t clusterShift = 4;

  // Size of an adc word in bytes
  const size_t adcWordSize = sizeof(unsigned char);
  // Number of adc words per buffer word
  const size_t adcsPerBuffer = sizeof(bufferWord) / adcWordSize;
  // Power of two that yields the number of bits in an adc word,
  // needed for shifting
  const size_t adcShift = 3;
}
#endif 
