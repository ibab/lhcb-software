// Include files
#include <string.h>

#include <iostream>
#include <memory>
#include <algorithm>

#include "boost/optional.hpp"
extern "C"{
#include <ZipLZMA.h>
}
#include "GaudiKernel/GaudiException.h"

// local
#include "CondDBCompression.h"
#include "base64.h"

constexpr auto MAXBUFFSIZE = 33554432; // 32 MB maximum xml buffer size

namespace {

boost::optional<std::string::size_type> unzip_LZMA_size(const unsigned char* tgt)
{
  if (tgt[0]=='X'&&tgt[1]=='Z'&&tgt[2]==0) {
    // assume this is a 9 byte ROOT header in front LZMA header
    // (see ZipLZMA.c in the ROOT distribution, search for tgt[0]
    //uint32_t out_size = (  uint32_t(tgt[3])
    //                    | (uint32_t(tgt[4])<<8)
    //                    | (uint32_t(tgt[5])<<16) );
    uint32_t in_size = (  uint32_t(tgt[6])
                       | (uint32_t(tgt[7])<<8)
                       | (uint32_t(tgt[8])<<16) );
    return in_size;
  }
  return boost::none;
}

}

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBCompression
//
// 2013-02-14 : Liang SUN
//-----------------------------------------------------------------------------

std::string CondDBCompression::compress(const std::string& strin, const int8_t method){
	std::vector<char> dest(MAXBUFFSIZE, char{}); //Output buffer
	int destLen = dest.size();
    switch (method){
    case 0: { //LZMA method from ROOT package
        int srcsize = strin.length();
	    int retbit(0);
        R__zipLZMA(9, &srcsize, const_cast<char*>(strin.data()), &destLen, dest.data(), &retbit);
        if (retbit == 0 )
            throw GaudiException("Error during LZMA compression", "CondDBCompression.cpp", StatusCode::FAILURE );
        destLen = retbit;
        break;
    }
    default: //Do nothing if method not recognized
        return strin;
    }

    // pass a preamble which sets the first byte to identify compression method
	auto str = base64_encode( { dest.data(), size_t(destLen) }, {{ char('0'+method) }} );
    return str;
}


std::string CondDBCompression::decompress(const std::string& strin){
  if (strin.empty() || !(strin[0] >= '0' && strin[0]<='9'))
    return strin; //If not starting from a digit, do nothing as the compression method is not recognizable

  const int8_t method = strin[0] - '0';
  std::string zdata = base64_decode(strin.data()+1, strin.size()-1);// Strip the method character before decoding
  int output_length = zdata.length();
  if (!output_length) return strin; // Do nothing if the string fails the compression process

  switch (method){
  case 0: {
    const unsigned char* tgt = reinterpret_cast<const unsigned char*>(zdata.data());
    auto uncompressed_size = unzip_LZMA_size(tgt).get_value_or(MAXBUFFSIZE);
    std::string dest(uncompressed_size, char{} ); //Output buffer
    int destSize = dest.size();
    int retbit = 0;
    // strictly speaking, writing directly to string::data invokes undefined behaviour...
    // (but at least in C++11, string::data is guaranteed to be contiguous memor )
    R__unzipLZMA(&output_length, const_cast<unsigned char*>(tgt),
                 &destSize, const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(dest.data())),
                 &retbit);
    if (retbit == 0) {
      throw GaudiException("Error during LZMA decompression", "CondDBCompression.cpp", StatusCode::FAILURE );
    }
    destSize = dest.size();
    if (retbit!=destSize) {
      if (destSize != MAXBUFFSIZE) {
          throw GaudiException("Uncompressed size not what was expected???", "CondDBCompression.cpp", StatusCode::FAILURE );
      }
      // didn't have a-priori knowledge of the size, so shrink down to the right size
      dest.resize(retbit);
    }
    return dest;
  }
  default: //Do nothing if method not recognized
    return strin;
  }
}
