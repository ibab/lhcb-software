// $Id: TES.cpp,v 1.1 2010-02-13 16:31:06 ibelyaev Exp $
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
// LoKi
// ============================================================================
#include "LoKi/TES.h"
#include "LoKi/ILoKiSvc.h"
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
{}
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
{}
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
// The END 
// ============================================================================
