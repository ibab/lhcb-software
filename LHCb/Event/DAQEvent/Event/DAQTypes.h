#ifndef DAQTypes_H
#define DAQTypes_H 1

#include <sys/types.h>
//(See at /usr/include/sys/types.h)

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
//typedef u__int32_t hlt_int ;
//typedef u__int16_t l1_int ;
typedef __int32_t hlt_int ;
typedef __int16_t l1_int ;

#else
//typedef u_int32_t hlt_int ;
//typedef u_int16_t l1_int ;
typedef int32_t hlt_int ;
typedef int16_t l1_int ;
#endif   // WIN32

#endif  // DAQTypes_H
