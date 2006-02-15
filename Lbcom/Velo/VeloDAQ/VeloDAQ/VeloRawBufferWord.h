// $Id: VeloRawBufferWord.h,v 1.2 2006-02-15 18:08:39 krinnert Exp $
#ifndef VELODAQ_VELORAWBUFFERWORD_H 
#define VELODAQ_VELORAWBUFFERWORD_H 1

#ifdef WIN32
typedef unsigned int   buffer_word;
#else
//(See at /usr/include/sys/types.h)
#include <sys/types.h>
typedef u_int32_t buffer_word;
#endif   // WIN32


#endif // VELODAQ_VELORAWBUFFERWORD_H
