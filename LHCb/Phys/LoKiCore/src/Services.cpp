// $Id: Services.cpp,v 1.7 2007-11-28 14:08:51 ibelyaev Exp $
// ===========================================================================
// Include files 
// ===========================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GetAlg.h"
#include "GaudiAlg/GetAlgs.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// LoKi
// ===========================================================================
#include "LoKi/Services.h"
#include "LoKi/Welcome.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Services
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ===========================================================================
// accessor to unique instance ("Meyer's singleton" pattern)
// ===========================================================================
LoKi::Services& LoKi::Services::instance()
{
  static LoKi::Services s_instance = Services() ;
  return s_instance ;
}
// ===========================================================================
// standard (default) constructor 
// ===========================================================================
LoKi::Services::Services()
  : m_lokiSvc    ( 0 ) 
  , m_ppSvc      ( 0 ) 
  , m_contextSvc ( 0 ) 
  , m_histoSvc   ( 0 ) 
{
  LoKi::Welcome::instance() ;
}
// ===========================================================================
// destructor
// ===========================================================================
LoKi::Services::~Services(){}
// ===========================================================================
// release all services 
// ===========================================================================
StatusCode LoKi::Services::releaseAll() 
{
  // release services 
  if ( 0 != m_histoSvc   ) { m_histoSvc   -> release () ; m_histoSvc   = 0 ; }
  // release services 
  if ( 0 != m_ppSvc      ) { m_ppSvc      -> release () ; m_ppSvc      = 0 ; }
  // 'release' the service 
  if ( 0 != m_contextSvc ) { m_contextSvc -> release () ; m_contextSvc = 0 ; }
  // 'release' the service 
  if ( 0 != m_lokiSvc    ) { m_lokiSvc    -> release () ; m_lokiSvc    = 0 ; }
  //
  return StatusCode::SUCCESS ;
}
// ===========================================================================
/*  Print the error  message, return status code
 *  @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ===========================================================================
StatusCode LoKi::Services::Error     
( const std::string& msg , 
  const StatusCode   st  , 
  const size_t       mx  ) const  
{
  return LoKi::Report::Error(" LoKi::Services " + msg , st , mx );
}
// ===========================================================================
/*  Print the warning  message, return status code 
 *  @param msg    warning message 
 *  @param st     status code  
 *  @return       status code 
 */
// ===========================================================================
StatusCode LoKi::Services::Warning   
( const std::string& msg  , 
  const StatusCode   st   , 
  const size_t       mx   ) const   
{
  return LoKi::Report::Warning(" LoKi::Services " + msg , st , mx ) ;
} 
// ===========================================================================
// accessor to main LoKi algorithm 
// ===========================================================================
LoKi::ILoKiSvc* LoKi::Services::lokiSvc () const { return m_lokiSvc ; };
// ===========================================================================
// set new main LoKi algorithms 
// ===========================================================================
LoKi::ILoKiSvc* LoKi::Services::setLoKi( LoKi::ILoKiSvc* svc )  
{
  // add reference to the new algo 
  if ( 0 != svc ) { svc -> addRef() ; }
  // release all  previously allocated  services  
  releaseAll().ignore() ;
  // set new algorithm
  m_lokiSvc = svc ;
  // get particle properties service from LoKi
  if ( 0 != m_lokiSvc    ) { m_ppSvc      = m_lokiSvc -> ppSvc      () ; }
  if ( 0 != m_ppSvc      ) { m_ppSvc      -> addRef                 () ; }
  // get context service from the LoKi
  if ( 0 != m_lokiSvc    ) { m_contextSvc = m_lokiSvc -> contextSvc () ; }
  if ( 0 != m_contextSvc ) { m_contextSvc -> addRef                 () ; }
  //
  return lokiSvc();
}
// ===========================================================================
// accessor to particle properties service
// ===========================================================================
IParticlePropertySvc* LoKi::Services::ppSvc     () const 
{
  if ( 0 != m_ppSvc ) { return m_ppSvc ; }
  Error ( " IParticlePropertySvc* points to NULL, return NULL" ) ;
  return 0 ;
}
// ===========================================================================
// accessor to context service
// ===========================================================================
IAlgContextSvc* LoKi::Services::contextSvc () const 
{
  if ( 0 != m_contextSvc ) { return m_contextSvc ; }
  Error ( " IParticlePropertySvc* points to NULL, return NULL" ) ;
  return 0 ;
}
// ===========================================================================
// accessor to historgam service
// ===========================================================================
IHistogramSvc* LoKi::Services::histoSvc () const 
{
  if ( 0 != m_histoSvc ) { return m_histoSvc ; }
  if ( 0 == m_lokiSvc  ) 
  {
    Error ( " histoSvc(): LoKi::ILoKiSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  ISvcLocator* loc = m_lokiSvc -> svcLoc () ;
  if ( 0 == loc ) 
  {
    Error ( " histoSvc(): ISvcLocator* points to NULL, return NULL" ) ;
    return 0 ;
  }
  StatusCode sc = loc->service( "HistogramDataSvc" , m_histoSvc ) ;
  if ( sc.isFailure() ) 
  {
    Error ( " histoSvc(): Could not locate 'HistogramDataSvc', return NULL" , sc ) ;
  }
  if ( 0 == m_histoSvc ) 
  {
    Error ( " IHistogramSvc points to NULL, return NULL" , sc ) ;
    return 0 ;
  }
  return m_histoSvc  ;
}
// ===========================================================================
// accessor to Event Data Service 
// ===========================================================================
IDataProviderSvc* LoKi::Services::evtSvc     () const 
{
  // get the service for the last Algorithm:
  IAlgContextSvc* ctx = contextSvc() ;
  if ( 0 == ctx ) 
  {
    Error ( "evtSvc(): no vaild context is established, return NULL" ) ;
    return 0 ;
  }
  { /// get the last GaudiAlgorithm
    GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( ctx ) ;  
    if ( 0 != alg )  { return alg->evtSvc() ; }                 // RETURN 
  }
  { /// get the last Algorithm 
    IAlgorithm* alg = Gaudi::Utils::getAlgorithm 
      ( ctx , Gaudi::Utils::AlgTypeSelector<Algorithm>() ) ;
    if ( 0 != alg ) 
    {
      Algorithm* a = dynamic_cast<Algorithm*> ( alg ) ;
      if ( 0 != a ) { return a -> evtSvc() ; }                   // RETURN 
    }
  }
  Error ( "evtSvc(): no way to get the valid service, return NULL" ) ;  
  return 0 ;
}
// ===========================================================================
// The END 
// ===========================================================================
