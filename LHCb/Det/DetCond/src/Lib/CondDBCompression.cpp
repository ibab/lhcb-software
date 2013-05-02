// Include files
#include <string.h>

#include <iostream>
#include <memory>
extern "C"{
#include <ZipLZMA.h>
}
#include "GaudiKernel/GaudiException.h"

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
        if (retbit == 0 ) 
            throw GaudiException("Error during LZMA compression", "CondDBCompression.cpp", StatusCode::FAILURE );
        destLen = retbit;
        break;
    default: //Do nothing if method not recognized
        delete [] dest;
        return strin;
    }
	
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
            if (retbit == 0 ) 
                throw GaudiException("Error during LZMA decompression", "CondDBCompression.cpp", StatusCode::FAILURE );
            destLen = retbit;
            break;
        default: //Do nothing if method not recognized
            delete [] dest;
            return strin;
        }
    	std::string out(dest, destLen);
    	delete [] dest;
        return out;
    }
    return strin; // Do nothing if the string fails the compression process
}


