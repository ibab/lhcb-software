// Include files
#include <bzlib.h>
#include <string.h>

#include <iostream>
#include <memory>
extern "C"{
#include <ZipLZMA.h>
}

// local
#include "DetCond/CondDBCompression.h"
#include "base64.h"

#define MAXBUFFSIZE  33554432 // 32MB maximum size for xml buffer

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBCompression
//
// 2013-02-14 : Liang SUN
//-----------------------------------------------------------------------------

std::string CondDBCompression::compress(const std::string& strin, const int8_t method){
	char *dest = new char[MAXBUFFSIZE]; //Output buffer
	unsigned int destLen = MAXBUFFSIZE;
	int retbit(0);
    int srcsize(0);
    switch (method){
    case 0: //LZMA method from ROOT package
        srcsize = strin.length();
        R__zipLZMA(9, &srcsize, const_cast<char*>(strin.c_str()), (int*)(&destLen), dest, &retbit);
        if (retbit == 0) return "";
        destLen = retbit;
        break;
    case 1: //BZ2 method
        retbit = BZ2_bzBuffToBuffCompress(dest, &destLen, const_cast<char*>(strin.c_str()), strin.length(), 9, 0, 0);
        if (retbit != BZ_OK) return "";
        break;
    default: //Do nothing if method not recognized
        delete [] dest;
        return strin;
    }
	
/*    if (retbit == BZ_MEM_ERROR){
		std::cerr << "Insufficient memory for BZ2_bzBuffToBuffCompress" << std::endl;
		return "";
	}
	else if (retbit == BZ_OUTBUFF_FULL){
		std::cerr << "The size of the compressed data exceeds "
				<< "*destLen in BZ2_bzBuffToBuffCompress" << std::endl;
		return "";
	}*/

    std::string deststr(dest, destLen);
	delete [] dest;
	std::string out;
	base64_encode(deststr, out);
    out.insert(0, 1, '0'+method); //Set first byte to identify compression method
	return out;
}


std::string CondDBCompression::decompress(const std::string& strin){
    if (!(strin[0] >= '0' && strin[0]<='9'))
        return strin; //If not starting from a digit, do nothing as the compression method is not recognizable

    std::string strin2 = strin; //Create a clone for future modification
    const int8_t method = strin2[0] - '0';
    strin2.erase(0, 1); // Strip the method character
	std::string zdata;
	base64_decode(strin2, zdata);
	size_t output_length = zdata.length();
//    std::auto_ptr<unsigned char> zdata(base64_decode(strin.c_str(), strin.length(), &output_length));
    if (output_length){
    	char* dest = new char[MAXBUFFSIZE]; //Output buffer
    	unsigned int destLen = MAXBUFFSIZE;
    	int retbit(0);
        switch (method){
        case 0:
            R__unzipLZMA((int*)&output_length, (unsigned char*)(const_cast<char*>(zdata.c_str())), (int*)(&destLen), (unsigned char*)(dest), &retbit);    
            if (retbit == 0) return ""; // empty string if error during compression
            destLen = retbit;
            break;
        case 1:    
            retbit = BZ2_bzBuffToBuffDecompress(dest, &destLen, const_cast<char*>(zdata.c_str()), output_length,0,0);
            if (retbit != BZ_OK) return ""; // empty string if error during compression
            break;
        default: //Do nothing if method not recognized
            delete [] dest;
            return strin;
        }
        if (destLen == 0 ) return "";
    	std::string out(dest, destLen);
    	delete [] dest;
        return out;
/*    	if (retbit == BZ_OK)  return out;
    	else if (retbit == BZ_MEM_ERROR){
    		std::cerr << "Insufficient memory for BZ2_bzBuffToBuffDecompress" << std::endl;
    		return "";
    	}
    	else if (retbit == BZ_OUTBUFF_FULL){
    		std::cerr << "The size of the compressed data exceeds "
    				<< "*destLen in BZ2_bzBuffToBuffDecompress" << std::endl;
    		return "";
    	}
    	else if (retbit == BZ_DATA_ERROR ) { std::cerr << "BZ_DATA_ERROR"<<std::endl;
            return "";
        }
    	else if (retbit == BZ_DATA_ERROR_MAGIC ) { std::cerr << "BZ_DATA_ERROR_MAGIC"<<std::endl;
            return "";
        }
    	else if (retbit == BZ_UNEXPECTED_EOF ) { std::cerr << "BZ_UNEXPECTED_EOF"<<std::endl;
            return "";
        }*/
    }
    return strin; // Do nothing if the string fails the compression process
}


