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
typedef __int32 hlt_int ;
typedef __int16 l1_int ;

#else
//(See at /usr/include/sys/types.h)
#include <sys/types.h>
typedef int32_t hlt_int ;
typedef int16_t l1_int ;
#endif   // WIN32

#endif  // DAQTypes_H
