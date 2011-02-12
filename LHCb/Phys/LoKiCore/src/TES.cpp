// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/ObjectContainerBase.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GetAlgs.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/Counters.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TES.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Constants.h"
// ============================================================================
/** @file 
 *  Implementation file for classes from namespace LoKi::TES 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2020-02-13
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// the invalid algorithm name 
  const std::string s_INVALID = "-<NON-YET-LOCATED-ALGORITHM>-" ;
  // ==========================================================================
}
// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Exists::Exists 
( const std::string& location     , 
  const bool         useRootInTes ) 
  : LoKi::Functor<void,bool> () 
  , m_location     ( location     ) 
  , m_useRootInTes ( useRootInTes ) 
  , m_algorithm    ( )
{
  if ( !m_algorithm ) 
  {
    ILoKiSvc* l = lokiSvc() ;
    Assert ( 0 != l   , "ILoKiSvc*       points to NULL!" ) ;
    SmartIF<IAlgContextSvc> cntx ( l ) ;
    Assert ( !(!cntx) , "IAlgContextSvc* points to NULL!" ) ;
    m_algorithm = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  } 
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::TES::Exists::~Exists(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Exists* LoKi::TES::Exists::clone() const 
{ return new LoKi::TES::Exists ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::TES::Exists::result_type 
LoKi::TES::Exists::operator() ( /* LoKi::TES::Exists::argument */ ) const 
{
  // check/locate the algorithm
  if ( !m_algorithm ) 
  {
    ILoKiSvc* l = lokiSvc() ;
    Assert ( 0 != l   , "ILoKiSvc*       points to NULL!" ) ;
    SmartIF<IAlgContextSvc> cntx ( l ) ;
    Assert ( !(!cntx) , "IAlgContextSvc* points to NULL!" ) ;
    m_algorithm = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  } 
  // the final check
  Assert ( !(!m_algorithm) , "GaudiAlgorithm* points to NULL" ) ;
  // 
  return m_algorithm->exist<DataObject> ( m_location , m_useRootInTes ) ;
}
// ============================================================================
// get algorithm name 
// ============================================================================
const std::string& LoKi::TES::Exists::algName() const 
{ return !m_algorithm ? s_INVALID : m_algorithm->name() ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::TES::Exists::fillStream ( std::ostream& s ) const 
{
  s << " EXISTS( " ;
  Gaudi::Utils::toStream ( m_location , s ) ;
  if ( !m_useRootInTes ) { s << " , False" ; }
  return s << " ) " ;
}
// ============================================================================


// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Contains::Contains 
( const std::string& location     , 
  const bool         useRootInTes ) 
  : LoKi::Functor<void,double> () 
  , m_location     ( location     ) 
  , m_useRootInTes ( useRootInTes ) 
  , m_algorithm    ( )
{
  if ( !m_algorithm ) 
  {
    ILoKiSvc* l = lokiSvc() ;
    Assert ( 0 != l   , "ILoKiSvc*       points to NULL!" ) ;
    SmartIF<IAlgContextSvc> cntx ( l ) ;
    Assert ( !(!cntx) , "IAlgContextSvc* points to NULL!" ) ;
    m_algorithm = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  } 
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::TES::Contains::~Contains(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Contains* LoKi::TES::Contains::clone() const 
{ return new LoKi::TES::Contains ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::TES::Contains::result_type 
LoKi::TES::Contains::operator() ( /* LoKi::TES::Contains::argument */ ) const 
{
  // check/locate the algorithm
  if ( !m_algorithm ) 
  {
    ILoKiSvc* l = lokiSvc() ;
    Assert ( 0 != l   , "ILoKiSvc*       points to NULL!" ) ;
    SmartIF<IAlgContextSvc> cntx ( l ) ;
    Assert ( !(!cntx) , "IAlgContextSvc* points to NULL!" ) ;
    m_algorithm = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  } 
  // the final check
  Assert ( !(!m_algorithm) , "GaudiAlgorithm* points to NULL" ) ;
  // 
  const bool ok = 
    m_algorithm->exist<ObjectContainerBase> 
    ( m_location , m_useRootInTes )  ;
  //
  if ( !ok ) { return -1 ; } // REUTRN 
  //
  const ObjectContainerBase *obj = 
    m_algorithm->get<ObjectContainerBase> 
    ( m_location , m_useRootInTes )  ;
  //
  return obj -> numberOfObjects () ;
}
// ============================================================================
// get algorithm name 
// ============================================================================
const std::string& LoKi::TES::Contains::algName() const 
{ return !m_algorithm ? s_INVALID : m_algorithm->name() ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::TES::Contains::fillStream ( std::ostream& s ) const 
{
  s << " CONTAINS( " ;
  Gaudi::Utils::toStream ( m_location , s ) ;
  if ( !m_useRootInTes ) { s << " , False" ; }
  return s << " ) " ;
}
// ============================================================================


// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Counter::Counter 
( const std::string& location     , 
  const std::string& counter      , 
  const double       bad          ,
  const bool         useRootInTes ) 
  : LoKi::TES::Contains ( location , useRootInTes ) 
  , m_counter ( counter ) 
  , m_bad     ( bad     )
{}
// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Counter::Counter 
( const std::string& location     , 
  const std::string& counter      ) 
  : LoKi::TES::Contains ( location ) 
  , m_counter ( counter ) 
  , m_bad     ( LoKi::Constants::NegativeInfinity )
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::TES::Counter::~Counter(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Counter* LoKi::TES::Counter::clone() const 
{ return new LoKi::TES::Counter ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::TES::Counter::result_type 
LoKi::TES::Counter::operator() ( /* LoKi::TES::Contains::argument */ ) const 
{
  
  if ( !m_algorithm ) 
  {
    ILoKiSvc* l = lokiSvc() ;
    Assert ( 0 != l   , "ILoKiSvc*       points to NULL!" ) ;
    SmartIF<IAlgContextSvc> cntx ( l ) ;
    Assert ( !(!cntx) , "IAlgContextSvc* points to NULL!" ) ;
    m_algorithm = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  } 
  // the final check
  Assert ( !(!m_algorithm) , "GaudiAlgorithm* points to NULL" ) ;
  //
  //
  if ( !m_algorithm -> exist<Gaudi::Numbers> ( location     () , 
                                             useRootInTes () ) )
  {
    Error ("No valid object is found for TES location, return 'bad'") ;
    return m_bad ;
  }
  //
  const Gaudi::Numbers* data = m_algorithm -> get<Gaudi::Numbers> 
    ( location() , useRootInTes() ) ;
  //
  const Gaudi::Numbers::Map& m = data->counters() ;
  Gaudi::Numbers::Map::const_iterator ifind = m.find ( counter() ) ;
  if ( m.end() == ifind ) 
  {
    Error ( "No counter is found, return 'bad'") ;
    return m_bad ;     // RETURN 
  }
  //
  return ifind->second ;  
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::TES::Counter::fillStream ( std::ostream& s ) const 
{
  s << " COUNTER( " ;
  Gaudi::Utils::toStream ( location () , s ) ;
  s << "," ;
  Gaudi::Utils::toStream ( counter  () , s ) ;
  //
  if ( LoKi::Constants::NegativeInfinity != m_bad  ) { s << ", " << m_bad  ; }
  if ( !useRootInTes ()                            ) { s << " , False" ; }
  //
  return s << " ) " ;
}
// ============================================================================



// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Stat::Stat 
( const std::string&            location     , 
  const std::string&            counter      , 
  LoKi::TES::Stat::Stat::PMF    function     ,
  const double                  bad          ,
  const bool                    useRootInTes ) 
  : LoKi::TES::Counter ( location , counter , bad , useRootInTes ) 
  , m_fun     ( function ) 
  , m_entries ( false    ) 
{
  Assert ( 0 != function , "Invalid pointer t memebr function!" ) ;
}
// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Stat::Stat 
( const std::string&            location     , 
  const std::string&            counter      , 
  LoKi::TES::Stat::Stat::PMF    function     )
  : LoKi::TES::Counter ( location , counter ) 
  , m_fun     ( function ) 
  , m_entries ( false    ) 
{
  Assert ( 0 != function , "Invalid pointer t memebr function!" ) ;
}
// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Stat::Stat 
( const std::string&            location     , 
  const std::string&            counter      , 
  const std::string&            fun          ,
  const double                  bad          ,
  const bool                    useRootInTes ) 
  : LoKi::TES::Counter ( location , counter , bad , useRootInTes ) 
  , m_fun     ( 0     ) 
  , m_entries ( false ) 
{ 
  // basic 
  if      ( "sum"     == fun ) { m_fun = &StatEntity::sum     ; } 
  else if ( "sum2"    == fun ) { m_fun = &StatEntity::sum2    ; }
  else if ( "mean"    == fun ) { m_fun = &StatEntity::mean    ; }
  else if ( "rms"     == fun ) { m_fun = &StatEntity::rms     ; }
  else if ( "meanErr" == fun ) { m_fun = &StatEntity::meanErr ; }
  else if ( "min"     == fun ) { m_fun = &StatEntity::min     ; }
  else if ( "max"     == fun ) { m_fun = &StatEntity::max     ; }
  // derived 
  else if ( "efficiency"    == fun ) { m_fun = &StatEntity::eff    ; }
  else if ( "efficiencyErr" == fun ) { m_fun = &StatEntity::effErr ; }
  else if ( "eff"           == fun ) { m_fun = &StatEntity::eff    ; }
  else if ( "effErr"        == fun ) { m_fun = &StatEntity::effErr ; }
  // a'la ROOT
  else if ( "Sum"     == fun ) { m_fun = &StatEntity::sum     ; } 
  else if ( "Sum2"    == fun ) { m_fun = &StatEntity::sum2    ; }
  else if ( "Mean"    == fun ) { m_fun = &StatEntity::mean    ; }
  else if ( "Rms"     == fun ) { m_fun = &StatEntity::rms     ; }
  else if ( "RMS"     == fun ) { m_fun = &StatEntity::rms     ; }
  else if ( "MeanErr" == fun ) { m_fun = &StatEntity::meanErr ; }
  else if ( "Min"     == fun ) { m_fun = &StatEntity::min     ; }
  else if ( "Max"     == fun ) { m_fun = &StatEntity::max     ; }
  else if ( "Eff"     == fun ) { m_fun = &StatEntity::eff    ; }
  else if ( "EffErr"  == fun ) { m_fun = &StatEntity::effErr ; }
  //
  else if ( "nEntries" == fun || 
            "N"        == fun || 
            "Entries"  == fun ||
            "entries"  == fun ) { m_entries = true ; }
  else { Exception ("Invalid function-name is specified!") ; }
}
// ============================================================================
// constructor from TES location 
// ============================================================================
LoKi::TES::Stat::Stat 
( const std::string&            location     , 
  const std::string&            counter      , 
  const std::string&            fun          )
  : LoKi::TES::Counter ( location , counter ) 
  , m_fun     ( 0     ) 
  , m_entries ( false ) 
{ 
  // basic 
  if      ( "sum"     == fun ) { m_fun = &StatEntity::sum     ; } 
  else if ( "sum2"    == fun ) { m_fun = &StatEntity::sum2    ; }
  else if ( "mean"    == fun ) { m_fun = &StatEntity::mean    ; }
  else if ( "rms"     == fun ) { m_fun = &StatEntity::rms     ; }
  else if ( "meanErr" == fun ) { m_fun = &StatEntity::meanErr ; }
  else if ( "min"     == fun ) { m_fun = &StatEntity::min     ; }
  else if ( "max"     == fun ) { m_fun = &StatEntity::max     ; }
  // derived 
  else if ( "efficiency"    == fun ) { m_fun = &StatEntity::eff    ; }
  else if ( "efficiencyErr" == fun ) { m_fun = &StatEntity::effErr ; }
  else if ( "eff"           == fun ) { m_fun = &StatEntity::eff    ; }
  else if ( "effErr"        == fun ) { m_fun = &StatEntity::effErr ; }
  // a'la ROOT
  else if ( "Sum"     == fun ) { m_fun = &StatEntity::sum     ; } 
  else if ( "Sum2"    == fun ) { m_fun = &StatEntity::sum2    ; }
  else if ( "Mean"    == fun ) { m_fun = &StatEntity::mean    ; }
  else if ( "Rms"     == fun ) { m_fun = &StatEntity::rms     ; }
  else if ( "RMS"     == fun ) { m_fun = &StatEntity::rms     ; }
  else if ( "MeanErr" == fun ) { m_fun = &StatEntity::meanErr ; }
  else if ( "Min"     == fun ) { m_fun = &StatEntity::min     ; }
  else if ( "Max"     == fun ) { m_fun = &StatEntity::max     ; }
  else if ( "Eff"     == fun ) { m_fun = &StatEntity::eff    ; }
  else if ( "EffErr"  == fun ) { m_fun = &StatEntity::effErr ; }
  //
  else if ( "nEntries" == fun || 
            "N"        == fun || 
            "Entries"  == fun ||
            "entries"  == fun ) { m_entries = true ; }
  else { Exception ("Invalid function-name is specified!") ; }
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::TES::Stat::~Stat(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::TES::Stat* LoKi::TES::Stat::clone() const 
{ return new LoKi::TES::Stat ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::TES::Stat::result_type 
LoKi::TES::Stat::operator() ( /* LoKi::TES::Contains::argument */ ) const 
{
  if ( !m_algorithm ) 
  {
    ILoKiSvc* l = lokiSvc() ;
    Assert ( 0 != l   , "ILoKiSvc*       points to NULL!" ) ;
    SmartIF<IAlgContextSvc> cntx ( l ) ;
    Assert ( !(!cntx) , "IAlgContextSvc* points to NULL!" ) ;
    m_algorithm = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  } 
  // the final check
  Assert ( !(!m_algorithm) , "GaudiAlgorithm* points to NULL" ) ;
  //
  //
  if ( counter().empty() && 
       m_algorithm -> exist<Gaudi::Counter> 
       ( location     () , 
         useRootInTes () ) )
  {
    const Gaudi::Counter* cnt = m_algorithm -> get<Gaudi::Counter> 
      ( location     () , 
        useRootInTes () ) ;
    //
    if ( m_entries ) { return cnt->counter().nEntries() ; }
    //
    return (cnt->counter().*m_fun) () ;
  }
  //
  if ( !m_algorithm -> exist<Gaudi::Counters> ( location     () , 
                                                useRootInTes () ) )
  {
    Error ("No valid object is found for TES location, return 'bad'") ;
    return bad ()  ;
  }
  //
  const Gaudi::Counters* data = m_algorithm -> get<Gaudi::Counters> 
    ( location() , useRootInTes() ) ;
  //
  const Gaudi::Counters::Map& m = data->counters() ;
  Gaudi::Counters::Map::const_iterator ifind = m.find ( counter() ) ;
  if ( m.end() == ifind ) 
  {
    Error ( "No counter is found, return 'bad'") ;
    return bad ()  ;     // RETURN 
  }
  //
  if ( m_entries ) { return ifind->second.nEntries() ; }
  //
  return (ifind->second.*m_fun)() ;
  //
}

// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::TES::Stat::fillStream ( std::ostream& s ) const 
{
  s << " STAT( " ;
  Gaudi::Utils::toStream ( location () , s ) ;
  s << "," ;
  Gaudi::Utils::toStream ( counter  () , s ) ;
  //
  if ( m_entries )                           { s << ",'nEntries'"  ; }
  else if (  &StatEntity::sum     == m_fun ) { s << ",'sum'"       ; }
  else if (  &StatEntity::sum2    == m_fun ) { s << ",'sum2'"      ; }
  else if (  &StatEntity::mean    == m_fun ) { s << ",'mean'"      ; }
  else if (  &StatEntity::rms     == m_fun ) { s << ",'rms'"       ; }
  else if (  &StatEntity::meanErr == m_fun ) { s << ",'meanErr'"   ; }
  else if (  &StatEntity::min     == m_fun ) { s << ",'min'"       ; }
  else if (  &StatEntity::max     == m_fun ) { s << ",'max'"       ; }
  else if (  &StatEntity::eff     == m_fun ) { s << ",'eff'"       ; }
  else if (  &StatEntity::effErr  == m_fun ) { s << ",'effErr'"    ; }
  //
  else                                       { s << ",'XXX?'"      ; }
  //
  if ( LoKi::Constants::NegativeInfinity != bad()  ) { s << ", " << bad() ; }
  if ( !useRootInTes ()                            ) { s << " , False" ; }
  //
  return s << " ) " ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
