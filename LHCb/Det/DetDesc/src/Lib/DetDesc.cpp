// $Id: DetDesc.cpp,v 1.3 2002-11-21 15:40:02 sponce Exp $ 

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


/// a static instance of the Services class
static DetDesc::Services* s_services = 0;

/**
 * Gets an instance of Services
 */
DetDesc::Services* DetDesc::services() {
  if (0 == s_services) {
    s_services = new Services();
  }
  s_services->addRef();
  return s_services;
}
