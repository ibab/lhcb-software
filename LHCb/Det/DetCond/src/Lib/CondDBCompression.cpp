// Include files
#include <bzlib.h>
#include <string.h>

#include <iostream>
#include <memory>

// local
#include "DetCond/CondDBCompression.h"
#include "base64.h"

#define MAXBUFFSIZE  33554432 // 32MB maximum size for xml buffer

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBCompression
//
// 2013-02-14 : Liang SUN
//-----------------------------------------------------------------------------

std::string CondDBCompression::compress(const std::string& strin){
	char *dest = new char[MAXBUFFSIZE];
	unsigned int destLen = MAXBUFFSIZE;
	int retbit = BZ2_bzBuffToBuffCompress(dest, &destLen, const_cast<char*>(strin.c_str()), strin.length(), 4, 0, 0);
	if (retbit == BZ_MEM_ERROR){
		std::cerr << "Insufficient memory for BZ2_bzBuffToBuffCompress" << std::endl;
		return "";
	}
	else if (retbit == BZ_OUTBUFF_FULL){
		std::cerr << "The size of the compressed data exceeds "
				<< "*destLen in BZ2_bzBuffToBuffCompress" << std::endl;
		return "";
	}

    std::string deststr(dest, destLen);
	delete [] dest;
	std::string out;
	base64_encode(deststr, out);
	return out;
}


std::string CondDBCompression::decompress(const std::string& strin){

	std::string zdata;
	base64_decode(strin, zdata);
	size_t output_length = zdata.length();
//    std::auto_ptr<unsigned char> zdata(base64_decode(strin.c_str(), strin.length(), &output_length));
    if (output_length){
    	char* dest = new char[MAXBUFFSIZE];
    	unsigned int destLen = MAXBUFFSIZE;
    	int retbit = BZ2_bzBuffToBuffDecompress(dest, &destLen, const_cast<char*>(zdata.c_str()), output_length,0,0);
    	std::string out(dest, destLen);
    	delete [] dest;
    	if (retbit == BZ_OK)  return out;
    	else if (retbit == BZ_MEM_ERROR){
    		std::cerr << "Insufficient memory for BZ2_bzBuffToBuffDecompress" << std::endl;
    		return "";
    	}
    	else if (retbit == BZ_OUTBUFF_FULL){
    		std::cerr << "The size of the compressed data exceeds "
    				<< "*destLen in BZ2_bzBuffToBuffDecompress" << std::endl;
    		return "";
    	}
/*    	else if (retbit == BZ_DATA_ERROR ) { std::cerr << "BZ_DATA_ERROR"<<std::endl;
            return "";
        }
    	else if (retbit == BZ_DATA_ERROR_MAGIC ) { std::cerr << "BZ_DATA_ERROR_MAGIC"<<std::endl;
            return "";
        }
    	else if (retbit == BZ_UNEXPECTED_EOF ) { std::cerr << "BZ_UNEXPECTED_EOF"<<std::endl;
            return "";
        }*/
    }
    return strin;
}


