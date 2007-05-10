// $Id: StringUtils.h,v 1.2 2007-05-10 14:52:57 cattanem Exp $
#ifndef KERNEL_STRINGUTILS_H 
#define KERNEL_STRINGUTILS_H 1

// Include files

#include <string>

/** @file StringUtils.h
 *
 *  @author Juan PALACIOS
 *  @date   2006-06-19
 */
namespace DaVinci 
{
  namespace StringUtils 
  {
    /** Strip particle name down to basic type.
     *  Strips off any "+", "-", "~", "anti-" from an std::string.
     *
     *  Example: mu+ -> mu, Delta++ -> Delta, anti-nu_tau -> nu_tau
     *
     *  @code
     *  std::string myParticleName("tau+");
     *  DaVinci::StringUtils::stripParticleName(myParticleName);
     *  @endcode
     *
     *  @param name (input) string to be stripped.
    */

    void stripParticleName(std::string& name)
    {
      if ( 0 == name.find("anti-") ) { name.erase(0,5); }
      std::string::size_type pos = name.find_first_of("+-~") ;
      if ( std::string::npos == pos ) { return; }
      stripParticleName( name.erase( pos , 1 ) ) ;  
    }

  }
}
#endif // KERNEL_STRINGUTILS_H
