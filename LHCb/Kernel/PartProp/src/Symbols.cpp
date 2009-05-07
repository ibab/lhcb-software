// $Id: Symbols.cpp,v 1.2 2009-05-07 15:03:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <map>
#include <iostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// PartProp 
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/Symbols.h"
#include "Kernel/CC.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/algorithm/string.hpp"
#include "boost/regex.hpp"
// ============================================================================
/** @file 
 *  Implementation file for functions form the file Kernel/Symbols.h
 *
 *  Many thanks to Antonio PELLEGRINO for the kind help with regex expressions 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-07
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// empty string 
  const std::string s_EMPTY ;
  // ==========================================================================
  /// check the JSpin hypothesis 
  std::string jspin ( const std::string& symbol ) 
  {
    static const boost::regex s_jspin 
      ( "[ ]*JSpin[ ]*\\([ ]*([1-9][0-9]*)[ ]*\\)[ ]*" ) ;
    boost::smatch what ;
    if ( boost::regex_match(  symbol , what , s_jspin ) && 1 < what.size() ) 
    { return what[1] ; }
    return  "" ;
  }
  /// check the LSpin hypothesis 
  std::string lspin ( const std::string& symbol ) 
  {
    static const boost::regex s_lspin 
      ( "[ ]*LSpin[ ]*\\([ ]*([1-9][0-9]*)[ ]*\\)[ ]*" ) ;
    boost::smatch what ;
    if ( boost::regex_match(  symbol , what , s_lspin ) && 1 < what.size() ) 
    { return what[1] ; }
    return  "" ;
  }
  /// check the SSpin hypothesis 
  std::string sspin ( const std::string& symbol ) 
  {
    static const boost::regex s_sspin 
      ( "[ ]*SSpin[ ]*\\([ ]*([1-9][0-9]*)[ ]*\\)[ ]*" ) ;
    boost::smatch what ;
    if ( boost::regex_match(  symbol , what , s_sspin ) && 1 < what.size() ) 
    { return what[1] ; }
    return  "" ;
  }
  /// check HasQuark hypothesis 
  std::string hasQ ( const std::string& symbol ) 
  {
    static const boost::regex s_hasQ 
      ( "[ ]*HasQuark[ ]*\\([ ]*(up|down|strange|charm|bottom|beauty|top|[1-6])[ ]*\\)[ ]*" ) ;
    boost::smatch what ;
    if ( boost::regex_match(  symbol , what , s_hasQ ) && 1 < what.size() ) 
    { return what[1] ; }
    return  "" ;
  }
  // check for []cc
  std::string cc ( const std::string& symbol ) 
  {
    static const boost::regex s_cc
      ( "[ ]*\\[[ ]*([[:word:]])[ ]*\\](cc|CC)[ ]*" ) ;
    boost::smatch what ;
    if ( boost::regex_match(  symbol , what , s_cc ) && 1 < what.size() ) 
    { return what[1] ; }
    return  "" ;
  }
  // ===========================================================================
}
// ============================================================================
/*  get the short description for the embedded symbol 
 *
 *  This is probably the most important method, 
 *  Please note the implementation - it costains the full
 *  list of "valid" symbols. The remaining symbols have a form of 
 *
 *  - " JSpin( i )"
 *  - " LSpin( i )"
 *  - " SSpin( i )"
 *  - " HasQuark( quark )"
 *  - " HasQuark( j )"
 *
 *  Where: 
 *   - <code>i</code> is integer positive number, 
 *   - <code>j</code> is a digit between 1 and 6, @see LHCb::ParticleID
 *   - <code>quark</code> is a quark name: <code>up</code>,
 *                   <code>down</code>, <code>strange</code>,
 *                   <code>charm</code>, <code>beauty</code>,
 *                   <code>bottom</code>, <code>top</code>
 *  @see LHCb::ParticleID 
 *
 *  @param sym the symbol 
 *  @return the description (empty string if unknown)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-07
 */
// ============================================================================
const std::string&  Decays::Symbols::symbol ( const std::string& _sym ) 
{
  // ==========================================================================
  const std::string sym = boost::trim_copy ( _sym ) ;
  // ==========================================================================
  typedef std::map<std::string,std::string> _Map ;
  static _Map s_map ;
  if ( s_map.empty() ) 
  {
    // ==========================================================================
    //       symbol           description                    class/instance 
    // ==========================================================================
    s_map [ "X"         ] = "Any particle"              ; // Any 
    s_map [ "Meson"     ] = "Any meson"                 ; // Meson
    s_map [ "Hadron"    ] = "Any hadron"                ; // Hadron
    s_map [ "Baryon"    ] = "Any baryon"                ; // Baryon
    s_map [ "Nucleus"   ] = "Any nucleus"               ; // Nucleus 
    s_map [ "Lepton"    ] = "Any lepton"                ; // Lepton 
    s_map [ "Nu"        ] = "Any neutral lepton"        ; // Neutrino
    s_map [ "nu"        ] = "Any neutral lepton"        ; // Neutrino
    s_map [ "l"         ] = "Any charged lepton"        ; // Ell 
    s_map [ "l+"        ] = "Any positive lepton"       ; // EllPlus 
    s_map [ "l-"        ] = "Any negative lepton"       ; // EllMinus 
    s_map [ "X0"        ] = "Any neutral  particle"     ; // Neutral 
    s_map [ "Xq"        ] = "Any charged  particle"     ; // Charged 
    s_map [ "X+"        ] = "Any positive particle"     ; // Positive 
    s_map [ "X-"        ] = "Any negative particle"     ; // Negative 
    s_map [ "Xd"        ] = "Any particle with d-quark" ; // HasQuark ( down    ) 
    s_map [ "Xu"        ] = "Any particle with u-quark" ; // HasQuark ( up      )
    s_map [ "Xs"        ] = "Any particle with s-quark" ; // HasQuark ( strange ) 
    s_map [ "Xc"        ] = "Any particle with c-quark" ; // HasQuark ( charm   )
    s_map [ "Xb"        ] = "Any particle with b-quark" ; // HasQuark ( botttom ) 
    s_map [ "Xt"        ] = "Any particle with t-quark" ; // HasQuark ( top     ) 
    s_map [ "Scalar"    ] = "Any scalar particle"       ; // JSpin ( 1 ) 
    s_map [ "Spinor"    ] = "Any spinor(1/2) particle"  ; // JSpin ( 2 ) 
    s_map [ "OneHalf"   ] = "Any spinor(1/2) particle"  ; // JSpin ( 2 ) 
    s_map [ "Vector"    ] = "Any vector particle"       ; // JSpin ( 3 ) 
    s_map [ "ThreeHalf" ] = "Any spinor(3/2) particle"  ; // JSpin ( 4 )
    s_map [ "Tensor"    ] = "Any tensor particle"       ; // JSpin ( 5 ) 
    s_map [ "FiveHalf"  ] = "Any spinor(5/2) particle"  ; // JSpin ( 6 ) 
    // ========================================================================
  }
  _Map::const_iterator ifind = s_map.find ( sym ) ;
  if ( s_map.end() != ifind ) { return ifind->second ; }
  // check for JSpin
  const std::string j = jspin ( sym ) ;
  if ( !j.empty() )                                        // JSpin( ... ) 
  {
    std::string result = "Any Particle with 2J+1= " + j ;
    s_map [ "JSpin(" + j + ")"] =  result ;
    s_map [ sym ] =  result ;
    return s_map[sym] ;
  }
  // check for LSpin
  const std::string l = lspin ( sym ) ;                    // LSpin ( ... )
  if ( !l.empty() ) 
  {
    std::string result = "Any Particle with 2L+1= " + l ;
    s_map [ "LSpin(" + l + ")"] =  result ;
    s_map [ sym ] =  result ;
    return s_map[sym] ;  
  }
  // check for SSpin
  const std::string s = sspin ( sym ) ;                     // SSPin ( ... ) 
  if ( !s.empty() ) 
  {
    std::string result = "Any Particle with 2S+1= " + s ;
    s_map [ "SSpin(" + s + ")"] =  result ;
    s_map [ sym ] =  result ;       
    return s_map[sym] ;  
  }
  // check for HasQuark 
  const std::string q = hasQ ( sym ) ;                      // HasQuark ( ... ) 
  if ( !q.empty() ) 
  {
    std::string result = "Any Particle with quark= " + q ;
    s_map [ "HasQuark" + q + ")"] =  result ;
    s_map [ sym                 ] =  result ;
    return s_map[sym] ;  
  }
  // finally
  return s_EMPTY ;
}
// ==========================================================================
/*  Check if the symbol/token is valid embedded symbol 
 *  @param sym the symbol to be checked 
 *  @param service the pointer to Particle property service 
 *  @return short description 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-07
 */
// ============================================================================
bool Decays::Symbols::validSymbol ( const std::string& sym ) 
{ return ! symbol( sym ).empty() ; }
// ==========================================================================
/*  get a list of particle names, known for Particle Properties Service 
 * 
 *  @code
 *  
 *   Decays::Names names ;
 *   const LHCb::IParticlePropertySvc* svc = ... ;
 *  
 *   StatusCode sc = Decays::particles ( svc  , names ) ;
 *   
 *  @endcode 
 *
 *  @see LHCb::IParticlePropertySvc 
 *  @param svc   (INPUT) pointer to particle property Service 
 *  @param name  (OUTPUT) list of particle names 
 *  @return status code
 */
// ============================================================================
StatusCode Decays::Symbols::particles 
( const LHCb::IParticlePropertySvc* service , 
  Decays::Symbols::Names&           names   ) 
{
  /// clear the container 
  names.clear() ;
  /// check the service 
  if ( 0 == service ) { return StatusCode::FAILURE ; }                 // RETURN 
  //
  typedef LHCb::IParticlePropertySvc::iterator iterator ;
  iterator begin = service -> begin () ;
  iterator end   = service -> end   () ;
  for ( ; end != begin ; ++begin ) 
  {
    const LHCb::ParticleProperty* pp = *begin ;
    if ( 0 == pp ) { continue ; }
    names.push_back ( pp -> particle () ) ;
  }
  // sort it according to CC-criteria 
  std::stable_sort ( names.begin() , names.end() , Decays::CC::CmpCC() ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  get a list of particle names, known for Particle Properties Service 
 * 
 *  @code
 *  
 *   const LHCb::IParticlePropertySvc* svc = ... ;
 *  
 *   Decays::Names names = Decays::particles ( svc  ) ;
 *   
 *  @endcode 
 *
 *  @see LHCb::IParticlePropertySvc 
 *  @param svc   (INPUT) pointer to particle property Service 
 *  @return list of particle names 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-07
 */
// ============================================================================
Decays::Symbols::Names Decays::Symbols::particles
( const LHCb::IParticlePropertySvc* service )
{
  Decays::Symbols::Names names ;
  StatusCode sc = particles ( service , names ) ;
  if ( sc.isFailure() ) { names.clear() ; }
  return names ;
}
// ============================================================================
/*  get a short description of the symbol (if known) 
 *  @param sym the symbol to be checked 
 *  @param service the pointer to Particle property service 
 *  @return short description 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-07
 */
// ============================================================================
std::string Decays::Symbols::symbol 
( const std::string&                sym_    , 
  const LHCb::IParticlePropertySvc* service ) 
{
  // ==========================================================================
  const std::string sym = boost::trim_copy ( sym_ ) ;
  // check if the symbol is embedded symbol:
  const std::string& special = symbol ( sym ) ;
  if ( !special.empty() ) { return special ; }                        // RETURN
  // otherwise it should be either particle name or []cc 
  if ( 0 == service ) { return "" ; }                                 // RETURN 
  //
  const LHCb::ParticleProperty* pp = service -> find ( sym  ) ;
  if ( 0 != pp ) { return pp->particle() ; }                           // RETURN 
  // check for CC 
  const std::string cq = cc ( sym ) ;
  if ( cq.empty() ) { return "" ; }                                   //  RETURN 
  /// look for the particle:
  pp = service->find ( cq ) ;
  if ( 0 == pp    ) { return "" ; }                                    // RETURN
  //
  std::string result = pp->particle() ;
  const LHCb::ParticleProperty* anti = pp->anti() ;
  if ( 0 != anti ) { result += " or " + anti->particle() ; }
  return result ;                                                      // RETURN 
}
// ============================================================================
/*  check if the symbol/token is valid symbol 
 *  @param sym the symbol to be checked 
 *  @param service the pointer to Particle property service 
 *  @return true for valid/known symbols
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-07
 */
// ============================================================================
bool Decays::Symbols::validSymbol 
( const std::string&                sym     , 
  const LHCb::IParticlePropertySvc* service ) 
{ return !symbol ( sym , service ).empty() ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
