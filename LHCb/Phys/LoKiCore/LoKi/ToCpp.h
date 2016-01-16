// $Id$ 
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
#include <map>
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
/** @file LoKi/ToCpp.h
 *  set of utilities used fro autogeneration of C++ code for LoKi-functors 
 *  @see LoKi::AuxFunBase 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-03-30
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$ 
 */
// ============================================================================
// Forward declarations 
// ============================================================================
namespace LoKi 
{ 
  class AuxFunBase ; 
  class Dump       ; 
  class FirstN     ; 
  class Sort       ; 
  class Param      ; 
  class Histo      ; 
  class CounterDef ; 
  //
  class GAUDI_API StrKeep 
  {
  public:
    StrKeep( const std::string& data ) ;
    const std::string& data() const { return m_data ; }
  private: 
    std::string m_data ;
  } ;  
}
// ============================================================================
namespace LHCb 
{
  class ParticleID ;
  class LHCbID     ;
}
// ============================================================================
namespace Gaudi
{
  class StringKey  ;
  class Time       ;
  class TimeSpan   ;
  class Histo1DDef ;
}
// ============================================================================
namespace Decays
{
  class iNode ;
}
// ============================================================================
namespace GaudiAlg 
{
  class ID ;
}
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    GAUDI_API std::string toCpp ( const LoKi::AuxFunBase&    o ) ;
    GAUDI_API std::string toCpp ( const LoKi::CounterDef&    o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Dump&          o ) ;
    GAUDI_API std::string toCpp ( const LoKi::FirstN&        o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Histo&         o ) ;
    GAUDI_API std::string toCpp ( const LoKi::LorentzVector& o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Point3D&       o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Param&         o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Sort&          o ) ;
    GAUDI_API std::string toCpp ( const LoKi::StrKeep&       o ) ;
    GAUDI_API std::string toCpp ( const LoKi::ThreeVector&   o ) ;
    //
    GAUDI_API std::string toCpp ( const LHCb::LHCbID&        o ) ;
    GAUDI_API std::string toCpp ( const LHCb::ParticleID&    o ) ;
    //
    GAUDI_API std::string toCpp ( const Gaudi::Time&         o ) ;
    GAUDI_API std::string toCpp ( const Gaudi::TimeSpan&     o ) ;
    GAUDI_API std::string toCpp ( const Gaudi::StringKey&    o ) ;
    GAUDI_API std::string toCpp ( const Gaudi::Histo1DDef&   o ) ;
    //
    GAUDI_API std::string toCpp ( const GaudiAlg::ID&        o ) ;
    // ========================================================================
    GAUDI_API std::string toCpp ( const Decays::iNode&       o ) ;
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
    // strings and chars 
    // ========================================================================
    inline std::string toCpp ( const char          s ) { return "'" + std::string ( 1 , s )  + "'"; }
    inline std::string toCpp ( const std::string&  s ) { return '"' + s + '"' ; }
    template <unsigned N>
    inline std::string toCpp 
    ( const char (&s)[N]    ) { return toCpp ( std::string ( s , s + N ) ) ; }
    template <unsigned N>
    inline std::string toCpp 
    (       char (&s)[N]    ) { return toCpp ( std::string ( s , s + N ) ) ; }  
    // ========================================================================
    inline std::string toCpp ( const short              o ) { return toString ( o ) ; }
    inline std::string toCpp ( const unsigned short     o ) { return toString ( o ) ; }
    inline std::string toCpp ( const int                o ) { return toString ( o ) ; }
    inline std::string toCpp ( const unsigned int       o ) { return toString ( o ) ; }
    inline std::string toCpp ( const long               o ) { return toString ( o ) ; }
    inline std::string toCpp ( const unsigned long      o ) { return toString ( o ) ; }
    // ========================================================================
    inline std::string toCpp ( const long long          o ) { return toString ( o ) ; }
    inline std::string toCpp ( const unsigned long long o ) { return toString ( o ) ; }
    inline std::string toCpp ( const bool               o ) { return o ? "true" : "false" ; }
    // ========================================================================
    //
    GAUDI_API 
    std::string toCpp ( const long double o , const unsigned short p = 16 ) ;
    inline 
    std::string toCpp ( const double  o ) { return toCpp ( o  , 16 ) ; }
    inline 
    std::string toCpp ( const float   o ) { return toCpp ( o  , 12 ) ; }
    // ========================================================================
    GAUDI_API 
    std::string toCpp ( const LoKi::AuxFunBase& o  ) ;
    // ========================================================================
    // declarations (1) 
    // ========================================================================
    // std::vector 
    template <class TYPE>
    inline std::string toCpp ( const std::vector<TYPE>&     v ) ;
    // std::map
    template <class TYPE1, class TYPE2>
    inline std::string toCpp ( const std::map<TYPE1,TYPE2>& v ) ;
    // std::tuple 
    template<typename... Args>
    inline std::string toCpp ( const std::tuple<Args...>&   t ) ;
    // ========================================================================
    // implementations 
    // ========================================================================
    template <class TYPE>
    inline std::string toCpp ( const std::vector<TYPE>& v )
    {
      std::string o = "std::vector<" + System::typeinfoName ( typeid(TYPE) ) + ">{" ;
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
    namespace details 
    {
      // ======================================================================
      /// recursive print of std::tuple
      template<class Tuple, std::size_t N>
      struct TupleCppPrinter 
      {
        static std::string toCpp ( const Tuple& t ) 
        { return 
            TupleCppPrinter<Tuple, N-1>::toCpp ( t ) 
            + " , " 
            + Gaudi::Utils::toCpp ( std::get<N-1>( t ) ) ; } 
      } ;
      /// stopping criteria for compile-time recursion
      template<class Tuple>
      struct TupleCppPrinter<Tuple,1> 
      {
        static std::string toCpp ( const Tuple& t ) 
        { return Gaudi::Utils::toCpp ( std::get<0> ( t ) ) ; }
      } ;
      /// stopping criteria for compile-time recursion
      template<class Tuple>
      struct TupleCppPrinter<Tuple,0> 
      {
        static std::string toCpp ( const Tuple& t ) { return ""; }
      } ;
      // ======================================================================
    }
    // ========================================================================
    /// std::tuple as comma-separated list - what we need for AuxFunBase 
    template<typename... Args>
    inline std::string toCpp_lst ( const std::tuple<Args...>& t ) 
    { return details::TupleCppPrinter<decltype(t), sizeof...(Args)>::toCpp ( t ) ; }
    /// std::tuple, just for completness 
    template<typename... Args>
    inline std::string toCpp ( const std::tuple<Args...>& t ) 
    { return System::typeinfoName ( typeid(t) ) + "{" + toCpp_lst ( t ) + "}" ; }
    // ========================================================================
  } //                                               The end of namespace Gaudi 
  // ==========================================================================
} //                                          The end of namespace Gaudi::Utils 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TOCPP_H
// ============================================================================
