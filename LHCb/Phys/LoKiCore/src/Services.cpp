// $Id: Services.cpp,v 1.10 2009-03-22 17:55:24 ibelyaev Exp $
// ===========================================================================
// Include files 
// ===========================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IStatSvc.h"
#include "GaudiKernel/ICounterSvc.h"
#include "GaudiKernel/IChronoSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
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
  , m_randSvc    ( 0 ) 
  , m_histoSvc   ( 0 ) 
  , m_evtSvc     ( 0 ) 
  , m_statSvc    ( 0 ) 
  , m_cntSvc     ( 0 ) 
  , m_chronoSvc  ( 0 ) 
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
  if ( 0 != m_chronoSvc  ) { m_chronoSvc  -> release () ; m_chronoSvc  = 0 ; }
  // release services 
  if ( 0 != m_cntSvc     ) { m_cntSvc     -> release () ; m_cntSvc     = 0 ; }
  // release services 
  if ( 0 != m_statSvc    ) { m_statSvc    -> release () ; m_statSvc    = 0 ; }
  // release services 
  if ( 0 != m_histoSvc   ) { m_histoSvc   -> release () ; m_histoSvc   = 0 ; }
  // 'release' the service 
  if ( 0 != m_evtSvc     ) { m_evtSvc     -> release () ; m_evtSvc     = 0 ; }
  // release services 
  if ( 0 != m_ppSvc      ) { m_ppSvc      -> release () ; m_ppSvc      = 0 ; }
  // release services 
  if ( 0 != m_randSvc    ) { m_randSvc    -> release () ; m_randSvc    = 0 ; }
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
  //
  return lokiSvc();
}
// ===========================================================================
// accessor to particle properties service
// ===========================================================================
LHCb::IParticlePropertySvc* LoKi::Services::ppSvc     () const 
{
  if ( 0 != m_ppSvc ) { return m_ppSvc ; }
  SmartIF<LHCb::IParticlePropertySvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( " LHCb::IParticlePropertySvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_ppSvc = svc ;
  m_ppSvc -> addRef () ;
  //
  return m_ppSvc ;
}
// ===========================================================================
// accessor to context service
// ===========================================================================
IAlgContextSvc* LoKi::Services::contextSvc () const 
{
  if ( 0 != m_contextSvc ) { return m_contextSvc ; }
  SmartIF<IAlgContextSvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( " IAlgContextSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_contextSvc = svc ;
  m_contextSvc -> addRef () ;
  //
  return m_contextSvc ;
}
// ===========================================================================
// accessor to histogram service
// ===========================================================================
IHistogramSvc* LoKi::Services::histoSvc () const 
{
  if ( 0 != m_histoSvc ) { return m_histoSvc ; }
  SmartIF<IHistogramSvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( " IHistogramSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_histoSvc = svc ;
  m_histoSvc -> addRef () ;
  //
  return m_histoSvc  ;
}
// ===========================================================================
// accessor to Random Numbers Service
// ===========================================================================
IRndmGenSvc* LoKi::Services::randSvc () const 
{
  if ( 0 != m_randSvc ) { return m_randSvc ; }
  // get the service form LoKi 
  SmartIF<IRndmGenSvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( "IRndmGenSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_randSvc = svc ;
  m_randSvc -> addRef() ;
  //
  return m_randSvc  ;
}
// ===========================================================================
// accessor to Event Data Service 
// ===========================================================================
IDataProviderSvc* LoKi::Services::evtSvc     () const 
{
  if ( 0 != m_evtSvc ) { return m_evtSvc ; }
  // get the service form LoKi 
  SmartIF<IDataProviderSvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( "IDataProviderSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_evtSvc = svc ;
  m_evtSvc -> addRef() ;
  //
  return m_evtSvc  ;
}
// ===========================================================================
// accessor to Statistical Service 
// ===========================================================================
IStatSvc* LoKi::Services::statSvc     () const 
{
  if ( 0 != m_statSvc ) { return m_statSvc ; }
  // get the service form LoKi 
  SmartIF<IStatSvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( "IStatSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_statSvc = svc ;
  m_statSvc -> addRef() ;
  //
  return m_statSvc  ;
}
// ===========================================================================
// accessor to Counter Service 
// ===========================================================================
ICounterSvc* LoKi::Services::cntSvc     () const 
{
  if ( 0 != m_cntSvc ) { return m_cntSvc ; }
  // get the service form LoKi 
  SmartIF<ICounterSvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( "ICounterSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_cntSvc = svc ;
  m_cntSvc -> addRef() ;
  //
  return m_cntSvc  ;
}
// ===========================================================================
// accessor to Chrono Service 
// ===========================================================================
IChronoSvc* LoKi::Services::chronoSvc     () const 
{
  if ( 0 != m_chronoSvc ) { return m_chronoSvc ; }
  // get the service form LoKi 
  SmartIF<IChronoSvc> svc ( m_lokiSvc ) ;
  if ( !svc ) 
  {
    Error ( "IChronoSvc* points to NULL, return NULL" ) ;
    return 0 ;
  }
  //
  m_chronoSvc = svc ;
  m_chronoSvc -> addRef() ;
  //
  return m_chronoSvc  ;
}
// ===========================================================================
// The END 
// ===========================================================================
