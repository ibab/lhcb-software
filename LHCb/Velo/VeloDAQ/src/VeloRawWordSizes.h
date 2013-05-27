// $Id: VeloRawWordSizes.h,v 1.1 2006-03-07 15:53:00 krinnert Exp $
#ifndef VELORAWWORDSIZES_H 
#define VELORAWWORDSIZES_H 1

#include "SiDAQ/SiRawBufferWord.h"

namespace VeloDAQ {
  // the size of a cluster word in bytes
  const size_t clu_word_size = sizeof(unsigned short);
  // the number of cluster words per buffer word
  const size_t clu_per_buffer = sizeof(SiDAQ::buffer_word)/clu_word_size;
  // the power of two that yields the number of bits in a cluster word,
  // needed for shifting
  const size_t clu_shift = 4;

  // the size of an adc word in bytes
  const size_t adc_word_size = sizeof(unsigned char);
  // the number of adc words per buffer word
  const size_t adc_per_buffer = sizeof(SiDAQ::buffer_word)/adc_word_size;
  // the power of two that yields the number of bits in an adc word,
  // needed for shifting
  const size_t adc_shift = 3;
}

#endif // VELORAWWORDSIZES_H
