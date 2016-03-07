// Include files
#include <string> 
#include <cstdio>
#include "DetDesc/DetDesc.h"


/** print double value using format
 *  @param value   value to be printed 
 *  @param format  desirable format
 *  @return string representation of the "value"
 */
std::string DetDesc::print (const double value, const char*  format) {
  static const unsigned int buflen = 128;
  static char  buffer[buflen];
  return { buffer , buffer + snprintf( buffer , buflen, format , value ) };
}

/** print double value using format
 *  @param value   value to be printed 
 *  @param format  desirable format
 *  @return string representation of the "value"
 */
std::string DetDesc::print (const long value, const char*  format) {
  static const unsigned int buflen = 128;
  static char  buffer[buflen];
  return { buffer , buffer + snprintf( buffer , buflen, format , value ) };
}

/**
 * Gets an instance of Services
 */
DetDesc::Services* DetDesc::services() {
  return DetDesc::Services::services();
}
