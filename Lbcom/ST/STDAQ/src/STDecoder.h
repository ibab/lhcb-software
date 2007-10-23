#ifndef _STDecoder_H 
#define _STDecoder_H 
#include "SiDAQ/SiRawBankDecoder.h"
#include "STClusterWord.h"
typedef SiRawBankDecoder<STClusterWord > STDecoder;
#endif
