// $Id: ToString.h,v 1.1 2006-06-18 18:35:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
#ifndef CALOPIDS_TOSTRING_H 
#define CALOPIDS_TOSTRING_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
#include <sstream>
// ============================================================================
namespace Gaudi
{
  namespace Utils 
  {
    /// @todo the file&functions to be removes as soon as new gaudi comes 
    template <class TYPE>
    inline std::string toString ( const TYPE& o ) 
    {
      std::ostringstream stream ;
      stream << o ;
      return stream.str() ;
    } ;
    inline std::string toString ( const std::string& o ) 
    { return "\"" + o + "\"" ; } ;
    template <class TYPE>
    inline std::string toString ( const std::vector<TYPE>& v ) 
    {
      std::ostringstream stream ;
      stream << "[" ;
      typedef typename std::vector<TYPE>::const_iterator iterator ;
      for ( iterator i = v.begin() ; v.end() != i ; ++i ) 
      {
        if ( v.begin() != i ) { stream << "," ; } // use the delimeter 
        stream << toString( *i ) ;
      }
      stream << "]" ;
      return stream.str() ;
    } ;
  } // end of namespace Utils 
} // end of namespace Gaudi 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOPIDS_TOSTRING_H
// ============================================================================
