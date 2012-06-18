#ifndef VL_RAW_BANK_CONSTANTS_H 
#define VL_RAW_BANK_CONSTANTS_H 1

#include <sys/types.h>

namespace VLDAQ {
  typedef u_int32_t row;
  // Size of a cluster word in bytes
  const size_t clusterWordSize = sizeof(unsigned short);
  // Number of cluster words per row 
  const size_t clustersPerRow = sizeof(row) / clusterWordSize;
  // Power of two that yields the number of bits in a cluster word,
  // needed for shifting
  const size_t clusterShift = 4;

  // Size of an adc word in bytes
  const size_t adcWordSize = sizeof(unsigned char);
  // Number of adc words per row word
  const size_t adcsPerRow = sizeof(row) / adcWordSize;
  // Power of two that yields the number of bits in an adc word,
  // needed for shifting
  const size_t adcShift = 3;
}
#endif 
