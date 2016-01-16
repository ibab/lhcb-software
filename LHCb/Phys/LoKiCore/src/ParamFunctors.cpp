// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GetAlg.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/ParamFunctors.h"
#include "LoKi/Param.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Interface.h"
// ============================================================================
// Boost 
// ============================================================================
#include <boost/regex.hpp>
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Param
 *  @see LoKi::Param
 *
 *  @date 2014-02-02 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @todo   replace Boost-regex with STD/STL-regex 
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 * By usage of this code one clearly states the disagreement 
 * with the smear campaign of Dr.O.Callot et al.: 
 * ``No Vanya's lines are allowed in LHCb/Gaudi software''
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  typedef std::map<std::string,double>                      MAP_D  ;
  typedef std::map<std::string,float>                       MAP_F  ;
  typedef std::map<std::string,int>                         MAP_I  ;
  //
  typedef SimpleProperty<double>                            DPROP  ;
  typedef SimpleProperty<MAP_D>                             MPROP  ;
  //
  class MyAlgSel : public Gaudi::Utils::AlgSelector
  {
  public:
    // ========================================================================
    MyAlgSel ( const std::string& name ,
               const std::string& prop , 
               const std::string& key  )
      : m_name ( name  ) 
      , m_prop ( prop  ) 
      , m_key  ( key   ) 
    {}
    // ========================================================================
  public:
    // ========================================================================
    /** the only one essential method:
     *  Seek for algorithm that 
     *    - matched the name pattern 
     *    - has property "Property"
     *    - the property shoudl be convertible either to 
     *        simple double or map { string: dobule}
     *    - for map-case, the presence of the 
     *       corresponsing key in map is required. 
     */
    virtual bool operator() ( const IAlgorithm* a ) const 
    {
      //
      if ( 0 == a )                                        { return false ; }
      //
      if ( !m_name.empty() ) 
      {
        boost::regex pattern ( m_name ) ;
        if ( !boost::regex_match ( a->name() , pattern ) ) { return false ; }
      }
      //
      const Property* p = Gaudi::Utils::getProperty ( a , m_prop ) ;
      if ( 0 == p )                                        { return false ; }
      //
      DPROP dp ;
      MPROP mp ;
      //
      bool db = false ;
      bool mb = false ;
      //
      try         { db = dp.assign ( *p ) ; }
      catch (...) { db = false            ; }
      try         { mb = mp.assign ( *p ) ; }
      catch (...) { mb = false            ; }
      //
      if      ( !db && !mb           )             { return false ; }
      else if (  db && m_key.empty() )             { return true  ; } // TRUE
      else if (  mb ) 
      {
        const MAP_D& m = mp.value() ;
        if ( m.end() != m.find ( m_key ) )         { return true  ; } // TRUE 
      }
      // finally: false 
      return false ;                                         // finally: false 
    }
  private: 
    // ========================================================================
    std::string m_name ;
    std::string m_prop ;
    std::string m_key  ;
    // ========================================================================    
  };
  // ==========================================================================
  const Property* getProperty 
  ( const SmartIF<IAlgContextSvc>& cntx  , 
    const LoKi::Param&             param )
  { 
    //
    if ( !cntx ) { return 0 ; }
    //
    MyAlgSel sel ( param.algorithm () , 
                   param.property  () , 
                   param.key       () ) ;
    
    IAlgorithm* ialg = Gaudi::Utils::getAlgorithm ( cntx , sel ) ;
    if ( 0 == ialg ) { return 0 ; }
    //
    return Gaudi::Utils::getProperty ( ialg , param.property() ) ;
  }
  // ==========================================================================
  const Property* getProperty 
  ( const LoKi::ILoKiSvc* svc   , 
    const LoKi::Param&    param )
  { 
    //
    if ( 0 == svc ) { return 0 ; }
    //
    LoKi::ILoKiSvc* svc0 = const_cast<LoKi::ILoKiSvc*> ( svc ) ;
    // get the context service: 
    SmartIF<IAlgContextSvc> cntx ( svc0 ) ;
    //
    return getProperty ( cntx , param ) ;
  }
  // ==========================================================================
  const Property* getProperty 
  ( const LoKi::AuxFunBase& base  , 
    const LoKi::Param&      param )
  { return getProperty ( base.lokiSvc() , param ) ; }
  // ==========================================================================
}
// ============================================================================
/*  Standard constructor from property 
 *  @param property   the property 
 */
// ============================================================================
LoKi::Parameters::ParamBase::ParamBase
( const Property&    property )
  : LoKi::AuxFunBase ( )
  , m_param          ( )
  , m_property       ( &property )
{}
// ============================================================================
/*  constructor from Param-object 
 *  @param property the property description
 */
// ============================================================================
LoKi::Parameters::ParamBase::ParamBase
( const LoKi::Param& property )
  : LoKi::AuxFunBase (   )
  , m_param          ( property )
  , m_property       ( 0 )
{
  if ( gaudi() ) { getProp() ; }
}
// ============================================================================
LoKi::Parameters::ParamBase::~ParamBase() { m_property = 0 ; }
// ============================================================================
void LoKi::Parameters::ParamBase::getProp() const
{ 
  m_property  = ::getProperty ( *this , m_param ) ;
  Assert ( 0 != m_property , "Unable to get property" ) ;
}

// ============================================================================
// constructor from parameter 
// ============================================================================
LoKi::Parameters::Parameter::Parameter  
( const LoKi::Param& param ) 
  : LoKi::AuxFunBase ( std::tie ( param ) )  
  , LoKi::Functor<void,double> () 
  , LoKi::Parameters::ParamBase ( param ) 
  , m_map_d     ( false ) 
  , m_map_f     ( false ) 
  , m_map_i     ( false ) 
  , m_scalar_d  ( false ) 
  , m_scalar_f  ( false ) 
  , m_scalar_i  ( false ) 
{
  if ( gaudi() ) { getProp() ; }
}
// ============================================================================
// constructor from parameter 
// ============================================================================
LoKi::Parameters::Parameter::Parameter  
( const std::string& param ) 
  : LoKi::AuxFunBase ( std::tie ( param ) )  
  , LoKi::Functor<void,double> () 
  , LoKi::Parameters::ParamBase ( param ) 
  , m_map_d     ( false ) 
  , m_map_f     ( false ) 
  , m_map_i     ( false ) 
  , m_scalar_d  ( false ) 
  , m_scalar_f  ( false ) 
  , m_scalar_i  ( false ) 
{
  if ( gaudi() ) { getProp() ; }
}
// ============================================================================
// virtual destructor 
// ============================================================================
LoKi::Parameters::Parameter::~Parameter () {}
// ============================================================================
// clone-method: "virtual constructor"
// ============================================================================
LoKi::Parameters::Parameter*
LoKi::Parameters::Parameter::clone() const 
{ return new LoKi::Parameters::Parameter ( *this ) ; }
// ============================================================================
void LoKi::Parameters::Parameter::getParams() const 
{
  const Property* p = property() ;
  if ( 0 == p ) { getProp() ; }
  //
  m_map_d     = dynamic_cast<const PropertyWithValue<MAP_D>*>   ( p ) ;
  m_map_f     = dynamic_cast<const PropertyWithValue<MAP_F>*>   ( p ) ;
  m_map_i     = dynamic_cast<const PropertyWithValue<MAP_I>*>   ( p ) ;
  m_scalar_d  = dynamic_cast<const PropertyWithValue<double>*>  ( p ) ;
  m_scalar_f  = dynamic_cast<const PropertyWithValue<float>*>   ( p ) ;
  m_scalar_i  = dynamic_cast<const PropertyWithValue<int>*>     ( p ) ;
  //
  Assert ( m_map_d    ||
           m_map_f    || 
           m_map_i    || 
           m_scalar_d || 
           m_scalar_f || 
           m_scalar_i  , "Invalid property type" ) ;  
}
// ============================================================================
// optional: nice printout 
// ============================================================================
std::ostream& LoKi::Parameters::Parameter::fillStream ( std::ostream& s ) const 
{ return s << param() ; }
// ============================================================================
// the major method 
// ============================================================================
namespace
{
  // ==========================================================================
  template <class TYPE> 
  double get ( const Property* prop ) 
  {
    const PropertyWithValue<TYPE>* p = 
      static_cast<const PropertyWithValue<TYPE>*> ( prop ) ;
    return p->value () ;
  }
  // ==========================================================================
}
// ============================================================================
LoKi::Parameters::Parameter::result_type 
LoKi::Parameters::Parameter::operator ()( /* argument */ ) const 
{
  //
  if      ( m_scalar_d ) { return  get<double> ( property () ) ; }
  else if ( m_scalar_f ) { return  get<float>  ( property () ) ; }
  else if ( m_scalar_i ) { return  get<int>    ( property () ) ; }
  //
  if ( m_map_d ) 
  {
    const PropertyWithValue<MAP_D>* p = 
      static_cast<const PropertyWithValue<MAP_D>*>( property() ) ;
    const MAP_D& m = p->value() ;
    MAP_D::const_iterator it = m.find ( param().key() ) ;
    Assert ( m.end() != it , "No proper key is found" ) ;
    return it->second ;
  }
  else if ( m_map_f )
  {
    const PropertyWithValue<MAP_F>* p = 
      static_cast<const PropertyWithValue<MAP_F>*>( property() ) ;
    const MAP_F& m = p->value() ;
    MAP_F::const_iterator it = m.find ( param().key() ) ;
    Assert ( m.end() != it , "No proper key is found" ) ;
    return it->second ;
  }
  else if ( m_map_i )
  {
    const PropertyWithValue<MAP_I>* p = 
      static_cast<const PropertyWithValue<MAP_I>*>( property() ) ;
    const MAP_I& m = p->value() ;
    MAP_I::const_iterator it = m.find ( param().key() ) ;
    Assert ( m.end() != it , "No proper key is found" ) ;
    return it->second ;
  }
  //
  Assert ( 1 < 2 , "Invalid type of parameter" ) ;
  //
  return 0 ;
}
// ============================================================================
// The END 
// ============================================================================



