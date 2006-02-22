// $Id: SiRawBufferWord.h,v 1.1 2006-02-22 17:21:16 krinnert Exp $
#ifndef SIDAQ_SIRAWBUFFERWORD_H 
#define SIDAQ_SIRAWBUFFERWORD_H 1

namespace SiDAQ {
#ifdef WIN32
typedef unsigned int buffer_word;
#else
//(See at /usr/include/sys/types.h)
#include <sys/types.h>
typedef u_int32_t buffer_word;
#endif   // WIN32
}

#endif // SIDAQ_SIRAWBUFFERWORD_H
