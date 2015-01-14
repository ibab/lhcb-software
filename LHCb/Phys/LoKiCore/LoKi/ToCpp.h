// $Id:$ 
// ============================================================================
#ifndef LOKI_TOCPP_H 
#define LOKI_TOCPP_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <vector>
#include <tuple>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/KinTypes.h"
// ============================================================================
namespace LoKi 
{ 
  class AuxFunBase ; 
  class Dump       ; 
  class FirstN     ; 
  class Sort       ; 
  class Param      ; 
}
namespace LHCb 
{
  class ParticleID ;
}
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    GAUDI_API std::string toCpp ( const LoKi::AuxFunBase&    o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Dump&          o ) ;
    GAUDI_API std::string toCpp ( const LoKi::FirstN&        o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Sort&          o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Param&         o ) ;
    GAUDI_API std::string toCpp ( const LoKi::LorentzVector& o ) ;
    GAUDI_API std::string toCpp ( const LoKi::ThreeVector&   o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Point3D&       o ) ;
    GAUDI_API std::string toCpp ( const LHCb::ParticleID&    o ) ;
    // ========================================================================
  }
  // ==========================================================================
}
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    inline std::string toCpp ( const char s ) { return "'" + std::string( 1 , s )  + "'"; }
    inline std::string toCpp ( const std::string&  s ) { return '"' + s + '"'; }
    template <unsigned N>
    inline std::string toCpp ( const char (&s)[N]    ) { return '"' + s + '"'; }
    template <unsigned N>
    inline std::string toCpp (       char (&s)[N]    ) { return '"' + s + '"'; }  
    inline std::string toCpp ( const double o        ) { return toString ( o ) ; }
    inline std::string toCpp ( const int    o        ) { return toString ( o ) ; }
    inline std::string toCpp ( const unsigned int  o ) { return toString ( o ) ; }
    inline std::string toCpp ( const long   o        ) { return toString ( o ) ; }
    inline std::string toCpp ( const unsigned long o ) { return toString ( o ) ; }
    inline std::string toCpp ( bool o ) { return o ? "true" : "false" ; }
    // ========================================================================
    GAUDI_API 
    std::string        toCpp ( const LoKi::AuxFunBase& o  ) ;
    // ========================================================================
    template <class TYPE>
    inline std::string toCpp ( const std::vector<TYPE>&     v ) ;
    template <class TYPE1, class TYPE2>
    inline std::string toCpp ( const std::map<TYPE1,TYPE2>& v ) ;
    // ========================================================================
    // declarations 
    // ========================================================================
    inline std::string toCpp( const std::tuple<>& /* o */ ) { return std::string() ; } 
    template <class TYPE1>
    inline std::string toCpp( const std::tuple<TYPE1>& o ) ;
    template <class TYPE1, class TYPE2>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2>& o ) ;
    template <class TYPE1, class TYPE2, class TYPE3>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2,TYPE3>& o ) ;
    template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2,TYPE3,TYPE4>& o ) ;
    template <class TYPE1, class TYPE2, class TYPE3, class TYPE4, class TYPE5>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2,TYPE3,TYPE4,TYPE5>& o ) ;
    // ========================================================================
    template <class TYPE>
    inline std::string toCpp ( const std::vector<TYPE>& v )
    {
      std::string o = "std::string<" + System::typeinfoName ( typeid(TYPE) ) + ">{" ;
      for ( typename std::vector<TYPE>::const_iterator it = v.begin() ; v.end() != it ; ++it ) 
      { o.append ( toCpp ( *it ) + ", " ) ; }
      //
      return o + '}' ;  
    }
    // ========================================================================
    template <class TYPE1, class TYPE2>
    inline std::string toCpp ( const std::map<TYPE1,TYPE2>& v ) 
    {
      std::string o = "std::map<" 
        + System::typeinfoName ( typeid(TYPE1) ) + ","
        + System::typeinfoName ( typeid(TYPE2) ) + ">{" ;
      for ( typename std::map<TYPE1,TYPE2>::const_iterator it = v.begin() ; v.end() != it ; ++it ) 
      { o.append ( "{" + toCpp ( it->first ) + ", " + toCpp( it->second ) + "} ," ) ; }
      //
      return o + '}' ;  
    }
    // ========================================================================
    template <class TYPE1>
    inline std::string toCpp( const std::tuple<TYPE1>& o ) 
    { return toCpp ( std::get<0> ( o ) ) ; } 
    //
    template <class TYPE1, class TYPE2>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2>& o ) 
    { return toCpp ( std::get<0> ( o ) ) + ", " + toCpp( std::get<1>( o ) ) ; } 
    //
    template <class TYPE1, class TYPE2, class TYPE3>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2,TYPE3>& o ) 
    {
      return 
        toCpp ( std::get<0> ( o ) ) + ", " + 
        toCpp ( std::get<1> ( o ) ) + ", " + 
        toCpp ( std::get<2> ( o ) ) ;
    }
    //
    template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2,TYPE3,TYPE4>& o ) 
    {
      return 
        toCpp ( std::get<0> ( o ) ) + ", " + 
        toCpp ( std::get<1> ( o ) ) + ", " + 
        toCpp ( std::get<2> ( o ) ) + ", " + 
        toCpp ( std::get<3> ( o ) ) ;
    }
    //
    template <class TYPE1, class TYPE2, class TYPE3, class TYPE4, class TYPE5>
    inline std::string toCpp( const std::tuple<TYPE1,TYPE2,TYPE3,TYPE4,TYPE5>& o ) 
    {
      return 
        toCpp ( std::get<0> ( o ) ) + ", " + 
        toCpp ( std::get<1> ( o ) ) + ", " + 
        toCpp ( std::get<2> ( o ) ) + ", " + 
        toCpp ( std::get<3> ( o ) ) + ", " + 
        toCpp ( std::get<4> ( o ) ) ;
    }
  } //                                               The end of namespace Gaudi 
  // ==========================================================================
} //                                          The end of namespace Gaudi::Utils 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TOCPP_H
// ============================================================================
