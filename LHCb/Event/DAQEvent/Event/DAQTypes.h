#ifndef DAQTypes_H
#define DAQTypes_H 1


/** @file
 *
 * To avoid implementation dependent size for int's and long's 
 * we (type)define here the explicit POSIX sizes for integers to be used in the
 * buffers banks.
 *
 *  @author Jose' Helder Lopes
 *  @date   06/08/2003
 */
 
#ifdef WIN32
typedef int   raw_int ;
typedef short l1_int ;

#else
//(See at /usr/include/sys/types.h)
#include <sys/types.h>
typedef int32_t raw_int ;
typedef int16_t l1_int ;
#endif   // WIN32

typedef raw_int* RawBufferPtr;
typedef l1_int*  L1BufferPtr;
#include <map>

#endif  // DAQTypes_H
