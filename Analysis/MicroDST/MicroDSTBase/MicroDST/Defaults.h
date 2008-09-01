// $Id: Defaults.h,v 1.1 2008-09-01 11:56:09 jpalac Exp $
#ifndef MICRODST_DEFAULTS_H 
#define MICRODST_DEFAULTS_H 1

// Include files

/** @class Defaults Defaults.h MicroDST/Defaults.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-09-01
 */
namespace _defaults 
{
  static const std::string& DefaultString("");
}

template <typename T> struct Defaults 
{
  
  static const std::string& location()
  {
    return _defaults::DefaultString;
  }
  static const std::string& cloner()
  {
    return _defaults::DefaultString;
  }

};
#endif // MICRODST_DEFAULTS_H
