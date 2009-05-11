/* Credits:

Original AUTHOR: Bob Trower 08/04/01

COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001
NOTE:           This source code may be used as you wish, subject to
                the MIT license.  See the LICENCE section below.


Hacked up by M.Frank
*/

#include "RTL/Base64.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

/// Translation Table as described in RFC1113
static const char encodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/// Translation Table to decode (created by author)
static const char decodeTable[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/// encode 3 8-bit binary bytes as 4 '6-bit' characters
static void encodeblock( unsigned char in[3], unsigned char out[4], int len )    {
  out[0] = encodeTable[ in[0] >> 2 ];
  out[1] = encodeTable[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (unsigned char) (len > 1 ? encodeTable[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
  out[3] = (unsigned char) (len > 2 ? encodeTable[ in[2] & 0x3f ] : '=');
}

/// base64 encode a stream adding padding and line breaks as per spec.
int RTL::Base64::encode(const void* in_buff, 
	   size_t in_len, 
	   void* out_buff, 
	   size_t out_len_max, 
	   size_t* out_len, 
	   int linesize)    
{
  typedef unsigned char u_c;
  typedef const unsigned char cu_c;
  const unsigned char *in_ptr = (cu_c*)in_buff, *in_end = ((cu_c*)in_buff)+in_len, *out_end = ((cu_c*)out_buff)+out_len_max;
  unsigned char *out_ptr = (u_c*)out_buff;
  unsigned char in[3], out[4];
  int i, len, blocksout = 0;

  *out_len = 0;
  while( in_ptr <= in_end ) {
    len = 0;
    for(i = 0; i < 3; ++i) {
      in[i] = 0;
      if ( in_ptr+1<=in_end )  {
	in[i] = *in_ptr;
	++in_ptr;
	++len;
      }
    }
    if( len ) {
      encodeblock( in, out, len );
      for( i = 0; i < 4; i++ ) {
	if ( out_ptr+1 <= out_end )
	  *out_ptr++ = out[i];
      }
      blocksout++;
    }
    if( blocksout >= (linesize/4) || in_ptr >= in_end ) {
      if( blocksout ) {
	if ( out_ptr+2 <= out_end ) {
	  //*out_ptr++ = '\r';
	  //*out_ptr++ = '\n';
	}
	else {
	  return 0;
	}
      }
      // Loop termination!!!!
      if ( in_ptr >= in_end ) {
	break;
      }
      blocksout = 0;
    }
  }
  *out_len = out_ptr-(u_c*)out_buff;
  return 1;
}

/// decode 4 '6-bit' characters into 3 8-bit binary bytes
static void decodeblock( unsigned char in[4], unsigned char out[3])   {   
  out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
  out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
  out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/// decode a base64 encoded stream discarding padding, line breaks and noise
/// base64 encode a stream adding padding and line breaks as per spec.
int RTL::Base64::decode(const void* in_buff, 
			size_t in_len, 
			void* out_buff, 
			size_t out_len_max, 
			size_t* out_len)
{
  typedef unsigned char u_c;
  typedef const unsigned char cu_c;
  const unsigned char *in_ptr = (cu_c*)in_buff, *in_end = ((cu_c*)in_buff)+in_len, *out_end = ((cu_c*)out_buff)+out_len_max;
  unsigned char *out_ptr = (u_c*)out_buff;
  unsigned char in[4], out[3], v;
  int i, len;

  while( in_ptr <= in_end ) {
    for( len = 0, i = 0; i < 4 && in_ptr<=in_end; i++ ) {
      v = 0;
      while( in_ptr<=in_end && v == 0 ) {
	v = *in_ptr++;
	v = (unsigned char) ((v < 43 || v > 122) ? 0 : decodeTable[ v - 43 ]);
	if( v ) {
	  v = (unsigned char) ((v == '$') ? 0 : v - 61);
	}
      }
      if( in_ptr<=in_end ) {
	len++;
	if( v ) {
	  in[ i ] = (unsigned char) (v - 1);
	}
      }
      else {
	in[i] = 0;
      }
    }
    if( len ) {
      decodeblock( in, out );
      for( i = 0; i < len - 1; i++ ) {
	if ( out_ptr+1 <= out_end ) {
	  *out_ptr++ = out[i];
	}
	else {
	  *out_len = 0;
	  return 0;
	}
      }
    }
  }
  *out_ptr = 0;
  *out_len = out_ptr - (u_c*)out_buff;
  return 1;
}

/* base64 encode a stream adding padding and line breaks as per spec.           */
extern "C" int lib_rtl_encode64(const void* in_buff, 
				size_t in_len, 
				void* out_buff, 
				size_t out_len_max, 
				size_t* out_len, 
				int linesize) 
{
  return RTL::Base64::encode(in_buff,in_len,out_buff,out_len_max,out_len,linesize);
}

/* decode a base64 encoded stream discarding padding, line breaks and noise.    */
extern "C" int lib_rtl_decode64(const void* in_buff, 
				size_t in_len, 
				void* out_buff, 
				size_t out_len_max, 
				size_t* out_len)
{
  return RTL::Base64::decode(in_buff,in_len,out_buff,out_len_max,out_len);
}


#include <iostream>
using namespace std;

extern "C" int testEncode64(int argc, char** argv) {
  const char* str="Hello Markus, How are you?";
  char buff[1024], sbuff[1024];
  size_t len=0, slen=0;
  int* ip;

  if ( argc>1 ) str = argv[1];
  int res = lib_rtl_encode64(str,strlen(str)+1,buff,sizeof(buff),&len,99999);
  cout << endl << "++++++++++++++++ String encoding:" << endl;
  cout   << "String to be encoded:'" << str << "' Result=" << res << " Len=" << len << endl;
  if ( res == 1 ) {
    cout << "Result:              '" << buff << "'" << endl;
  }
  else {
    cout << "The encoding failed." << endl;
  }
  res = lib_rtl_decode64(buff,len,sbuff,sizeof(sbuff),&slen);
  if ( res == 1 ) {
    cout   << "String again decoded:'" << sbuff << "' Result=" << res << endl;
  }
  else {
    cout << "The decoding failed." << endl;
  }
  
  cout << endl << "++++++++++++++++ Buffer encoding:" << endl;

  for(len=0, ip=(int*)sbuff; len<10; ++len) {
    *ip++ = len*10;
  }
  ::memset(buff,0,sizeof(buff));

  cout   << "Buffer to be encoded:'";
  for(len=0, ip = (int*)sbuff; len<10; ++len)
    cout << *ip++ << " ";
  res = lib_rtl_encode64(sbuff,10*sizeof(int),buff,sizeof(buff),&len,99999);
  cout << "' Result=" << res << " Len=" << len << endl;
  if ( res == 1 ) {
    cout << "Result:              '" << buff << "' Len=" << len << endl;
  }
  else {
    cout << "The encoding failed." << endl;
  }
  slen=0;
  ::memset(sbuff,0,sizeof(sbuff));
  res = lib_rtl_decode64(buff,len,sbuff,sizeof(sbuff),&slen);
  if ( res == 1 ) {
    cout   << "Buffer again decoded: Len:" << slen << " '";
    for(len=0, ip = (int*)sbuff; len<10; ++len)
      cout << *ip++ << " ";
    cout << "' Result=" << res << endl;
  }
  else {
    cout << "The decoding failed." << endl;
  }
  return 1;
}
