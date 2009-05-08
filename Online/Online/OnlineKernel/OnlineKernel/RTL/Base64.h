#ifndef ONLINEKERNEL_RTL_BASE64_H
#define ONLINEKERNEL_RTL_BASE64_H 1


#ifdef __cplusplus
#include <cstdlib>

/*
 *   RTL namespace declaration
 */
namespace RTL {

  /* @class Base64  Base64.h RTL/Base64.h
   *
   *   Small helper class to invoke base64 encoding of strings and memory buffers.
   *
   *   @author  M.Frank
   *   @version 1.0
   *
   */
  class Base64 {
  public:
    /// base64 encode a stream adding padding and line breaks as per spec.
    static int encode(const void* in_buff, 
		      size_t in_len, 
		      void* out_buff, 
		      size_t out_len_max, 
		      size_t* out_len, 
		      int linesize=999999);

    /// decode a base64 encoded stream discarding padding, line breaks and noise.
    static int decode(const void* in_buff, 
		      size_t in_len, 
		      void* out_buff, 
		      size_t out_len_max, 
		      size_t* out_len);
  };
}  // End namespace RTL

extern "C" {
#define __CXX_CONST const
#else
#include <stdlib.h>
#define __CXX_CONST
#endif

  /* base64 encode a stream adding padding and line breaks as per spec.           */
  int lib_rtl_encode64(__CXX_CONST void* in_buff, 
		      size_t in_len, 
		      void* out_buff, 
		      size_t out_len_max, 
		      size_t* out_len, 
		      int linesize);
  /* decode a base64 encoded stream discarding padding, line breaks and noise.    */
  int lib_rtl_decode64(__CXX_CONST void* in_buff, 
		      size_t in_len, 
		      void* out_buff, 
		      size_t out_len_max, 
		      size_t* out_len);

#ifdef __cplusplus
}
#endif
#undef __CXX_CONST
#endif // ONLINEKERNEL_RTL_BASE64_H
