#ifndef VELOLITE_RAWWORDSIZES_H 
#define VELOLITE_RAWWORDSIZES_H 1

#include "SiDAQ/SiRawBufferWord.h"

namespace VeloLiteDAQ {
  // Size of a cluster word in bytes
  const size_t clu_word_size = sizeof(unsigned short);
  // Number of cluster words per buffer word
  const size_t clu_per_buffer = sizeof(SiDAQ::buffer_word)/clu_word_size;
  // Power of two that yields the number of bits in a cluster word,
  // needed for shifting
  const size_t clu_shift = 4;

  // Size of an adc word in bytes
  const size_t adc_word_size = sizeof(unsigned char);
  // Number of adc words per buffer word
  const size_t adc_per_buffer = sizeof(SiDAQ::buffer_word)/adc_word_size;
  // Power of two that yields the number of bits in an adc word,
  // needed for shifting
  const size_t adc_shift = 3;
}
#endif 
