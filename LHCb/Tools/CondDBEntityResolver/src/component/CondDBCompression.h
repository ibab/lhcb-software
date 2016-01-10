/*
 * CondDBCompression.h
 *
 *  Created on: Feb 14, 2013
 *      Author: Liang Sun
 */

#ifndef CONDDBCOMPRESSION_H_
#define CONDDBCOMPRESSION_H_

#include <string>
#include <cstdint>

/** @class CondDBCompression CondDBCompression.h DetCond/CondDBCompression.h
 *
 *  Provides two functions to compress & decompress the strings
 *  from CondDB, so that we are able to have compressed CondDBs to read/write
 *
 *  @author Liang SUN
 *  @date   Feb 2013
 */

struct CondDBCompression{
	static std::string compress(const std::string& strin, int8_t method = 0);
	static std::string decompress(const std::string&);
};

#endif /* CONDDBCOMPRESSION_H_ */
