// $Id$ 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/HistoDef.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiHistoID.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/Monitor.h"
#include "LoKi/HistoBook.h"
#include "LoKi/ToCpp.h"
// ============================================================================
/** @file
 *  implementation of helper classes for monitoring
 *  @date 2015-01-14 
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// constructors for Histogram service 
// ============================================================================
LoKi::Histo::Histo () 
  : m_path ()
  , m_id   () 
  , m_hdef () 
  , m_case  ( true  ) 
  , m_valid ( false ) 
  , m_hsvc ( 0 ) 
  , m_cntx ( 0    ) 
{}
// ============================================================================
LoKi::Histo::Histo 
( const std::string&       dir  , 
  const std::string&       id   , 
  const Gaudi::Histo1DDef& hdef , 
  IHistogramSvc*           svc  ) 
  : m_path  ( dir  )
  , m_id    ( id   ) 
  , m_hdef  ( hdef ) 
  , m_case  ( true ) 
  , m_valid ( true ) 
  , m_hsvc  ( svc  ) 
  , m_cntx  ( 0    ) 
{}
// ============================================================================
LoKi::Histo::Histo 
( const std::string&       dir  , 
  const int                id   , 
  const Gaudi::Histo1DDef& hdef , 
  IHistogramSvc*           svc  ) 
  : m_path  ( dir  )
  , m_id    ( id   ) 
  , m_hdef  ( hdef ) 
  , m_case  ( true ) 
  , m_valid ( true ) 
  , m_hsvc  ( svc  ) 
  , m_cntx  ( 0    ) 
{}
// ============================================================================
LoKi::Histo::Histo 
( const std::string&       dir  , 
  const GaudiAlg::ID&      id   , 
  const Gaudi::Histo1DDef& hdef , 
  IHistogramSvc*           svc  ) 
  : m_path  ( dir  )
  , m_id    ( id   ) 
  , m_hdef  ( hdef ) 
  , m_case  ( true ) 
  , m_valid ( true ) 
  , m_hsvc  ( svc  ) 
  , m_cntx  ( 0    ) 
{}
// ============================================================================
LoKi::Histo::Histo 
( const std::string&       path , 
  const Gaudi::Histo1DDef& hdef , 
  IHistogramSvc*           svc  ) 
  : m_path  ( path )
  , m_id    (      ) 
  , m_hdef  ( hdef ) 
  , m_case  ( true ) 
  , m_valid ( true ) 
  , m_hsvc  ( svc  ) 
  , m_cntx  ( 0    ) 
{}
// ============================================================================
// constructors for Context service  
// ============================================================================
LoKi::Histo::Histo 
( const Gaudi::Histo1DDef& hdef , 
  const std::string&       id   , 
  IAlgContextSvc*          svc  ) 
  : m_path ()
  , m_id    ( id    ) 
  , m_hdef  ( hdef  ) 
  , m_case  ( false ) 
  , m_valid ( true  ) 
  , m_hsvc  ( 0     ) 
  , m_cntx  ( svc   ) 
{}
// ============================================================================
LoKi::Histo::Histo 
( const Gaudi::Histo1DDef& hdef , 
  const int                id   , 
  IAlgContextSvc*          svc  ) 
  : m_path ()
  , m_id    ( id    ) 
  , m_hdef  ( hdef  ) 
  , m_case  ( false ) 
  , m_valid ( true  ) 
  , m_hsvc  ( 0     ) 
  , m_cntx  ( svc   ) 
{}
// ============================================================================
LoKi::Histo::Histo 
( const Gaudi::Histo1DDef& hdef , 
  const GaudiAlg::ID&      id   , 
  IAlgContextSvc*          svc  ) 
  : m_path  ()
  , m_id    ( id    ) 
  , m_hdef  ( hdef  ) 
  , m_case  ( false ) 
  , m_valid ( true  ) 
  , m_hsvc  ( 0     ) 
  , m_cntx  ( svc   ) 
{}
// ============================================================================
std::string Gaudi::Utils::toCpp ( const GaudiAlg::ID& o )
{
  return "GaudiAlg::ID(" + 
    ( o.numeric() ?  
      toCpp ( o.numericID() ) :
      o.literal() ? 
      toCpp ( o.literalID() ) : "" ) + ") " ;
}
// ============================================================================
std::string Gaudi::Utils::toCpp ( const Gaudi::Histo1DDef& o )
{
  return "Gaudi::Histo1DDef(" 
    + toCpp ( o.lowEdge  () ) + ", "
    + toCpp ( o.highEdge () ) + ", " 
    + toCpp ( o.bins     () ) + ", " 
    + toCpp ( o.title    () ) + ") " ;
}
// ============================================================================
std::string Gaudi::Utils::toCpp ( const LoKi::Histo& o ) 
{
  //
  std::string s = "LoKi::Histo(" ;
  if ( !o.valid() ) { return s + ")"; }  // RETURN 
  //
  if  ( o.hcase() ) 
  {
    s += toCpp ( o.path() ) + ", " ;
    const GaudiAlg::ID& id = o.id() ;
    if   ( id.undefined() ) {} 
    else { s += toCpp( id ) + "; " ; }
  }
  //
  s += toCpp ( o.hdef() ) ;
  if  ( !o.hcase() ) { s += ", " + toCpp( o.id() ) ; }
  //
  return s + ")" ;
}
// ============================================================================
LoKi::CounterDef::CounterDef 
( const std::string&           group ,
  const std::string&           name  , 
  const LoKi::Monitoring::Flag flag  ) 
  : m_group ( group ) 
  , m_name  ( name  ) 
  , m_flag  ( flag  ) 
{}
// ============================================================================
LoKi::CounterDef::CounterDef 
( const std::string&           name , 
  const LoKi::Monitoring::Flag flag ) 
  : m_group (       ) 
  , m_name  ( name  ) 
  , m_flag  ( flag  ) 
{}
// ============================================================================
std::string Gaudi::Utils::toCpp ( const LoKi::CounterDef& o ) 
{
  //
  std::string s = "LoKi::CounterDef(" ;
  if ( !o.valid() ) { return s + ")"; }  // RETURN 
  //
  if ( !o.group().empty() ) { s += toCpp ( o.group () ) + "," ; }
  s += toCpp ( o.name  () ) + "," ; 
  //
  switch ( o.flag() ) 
  {
  case LoKi::Monitoring::ContextSvc : 
    s += "LoKi::Monitoring::ContextSvc" ;  break ;
  case LoKi::Monitoring::StatSvc : 
    s += "LoKi::Monitoring::StatSvc"    ;  break ;
  case LoKi::Monitoring::CounterSvc : 
    s += "LoKi::Monitoring::CounterSvc" ;  break ;    
  default:
    s += toCpp ( (int) o.flag()  )      ;  break ;
  }
  //
  return s + ")" ;
}


// ============================================================================
// The END 
// ============================================================================
