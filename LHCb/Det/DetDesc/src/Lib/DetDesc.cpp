// $Id: DetDesc.cpp,v 1.4 2002-12-13 14:18:20 ibelyaev Exp $ 

// Include files
#include <string> 
#include <stdio.h>
#include "DetDesc/DetDesc.h"

/** print double value using format
 *  @param value   value to be printed 
 *  @param format  desirable format
 *  @returnstring representation of the "value"
 */
const std::string DetDesc::print (const double value, const char*  format) {
  static const unsigned int buflen = 128;
  static char  buffer[buflen];
  return std::string( buffer , buffer + sprintf( buffer , format , value ) );
};

/** print double value using format
 *  @param value   value to be printed 
 *  @param format  desirable format
 *  @returnstring representation of the "value"
 */
const std::string DetDesc::print (const long value, const char*  format) {
  static const unsigned int buflen = 128;
  static char  buffer[buflen];
  return std::string( buffer , buffer + sprintf( buffer , format , value ) );
};


/**
 * Gets an instance of Services
 */
DetDesc::Services* DetDesc::services() {
  return DetDesc::Services::services();
}
