#ifndef DAVINCIUTILS_FUNCTIONS_H 
#define DAVINCIUTILS_FUNCTIONS_H 1

/** @namespace DaVinci::Utils DaVinciUtils/Functions.h
 *  
 *  General helper functions for use in DaVinci.
 *  These should only know about Gaudi classes and patterns.
 *  No knowledge of DaVinci specific code should be assumed.
 * 
 *  @author Juan PALACIOS palacios@uzh.uzh.ch
 *  @date   2010-09-21
 */

namespace DaVinci 
{
  
namespace Utils
{

  ///  Is obj in TES
  template <class T>
  inline bool inTES(const T* obj) {
    return ( 0!=obj && 0!=obj->parent()) ;
  }

} // namespace Utils
  
} // namespace DaVinci

#endif // DAVINCIUTILS_FUNCTIONS_H 
