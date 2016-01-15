// $Id:$ 
// ============================================================================
// STD
// ============================================================================
#include <algorithm>
// ============================================================================
// local
// ============================================================================
#include "MessagingBase.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
/** @file 
 *  Implementation file for class : MessagingBase
 *  @see LoKi::MessagingBase
 *  @date 2014-10-21 
 *  @author Chris Jones
 *  Verson            $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
LoKi::MessagingBase::MessagingBase
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type , name , parent )
  , m_myAlg      (       )  
  , m_printMyAlg ( false ) 
  , m_prints     ( 2     )  
    //
#ifndef NDEBUG 
  , m_timing         ( true  )
#else 
  , m_timing         ( false )
#endif   
    //
{ 
  //
  declareProperty 
    ( "PrintMyAlg"        ,
      m_printMyAlg        ,
      "Print the name of ``associated'' algorithm" ) ;
  declareProperty 
    ( "MaxPrints"         ,
      m_prints            ,
      "Maximum number of prints" ) ;
  declareProperty 
    ( "MeasureCPUPerformance"   , 
      m_timing                  , 
      "Measure CPU perormance"  ) ;  
  //
  if ( parent )
  {
    SmartIF<IAlgorithm> alg ( const_cast<IInterface*>(parent) ) ;
    if ( alg.isValid() ) { m_myAlg = alg->name() ; }
  }
}
// ============================================================================
// Destructor
// ============================================================================
LoKi::MessagingBase::~MessagingBase() {}
// ============================================================================
// Initialize
// ============================================================================
StatusCode LoKi::MessagingBase::initialize()
{
  /// initialize the base
  const StatusCode sc = GaudiTool::initialize () ;
  if ( sc.isFailure() ) { return sc ; }           
  //
  if ( msgLevel ( MSG::DEBUG ) &&  0 == m_prints )
  {
    m_prints = 10 ;
    warning () << "Redefine 'MaxPrints' property to " << m_prints << endmsg ;
  }
  //
  if ( parent() )
  {
    SmartIF<IAlgorithm> alg ( const_cast<IInterface*> ( parent() ) ) ;
    if ( alg.isValid() ) { m_myAlg = alg->name() ; }
  }
  //
  // is message level is low enough, enforce CPU performance measurements
  if ( msgLevel ( MSG::DEBUG ) ) { m_timing = true ; }
  //
  return sc;
}
// ============================================================================
// Finalize
// ============================================================================
StatusCode LoKi::MessagingBase::finalize() { return GaudiTool::finalize () ; }
// ============================================================================

// ============================================================================
// get the correct algorithm context
// ============================================================================
bool LoKi::MessagingBase::getMyAlg () const
{
  m_myAlg = "" ;
  //
  const IAlgContextSvc* asvc = contextSvc () ;
  if ( NULL == asvc    ) { return false ; }
  //
  const IAlgorithm* current = asvc->currentAlg() ;
  if ( NULL == current ) { return false ; }
  //
  m_myAlg = " [" + current->name() + "]" ;
  //
  return true ;
}

// ============================================================================
// The END 
// ============================================================================
