// $Id: Listener.cpp,v 1.1 2010-04-03 22:19:38 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <string>
// ============================================================================
// GaudiKernrel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/Listener.h"
#include "LoKi/ILoKiSvc.h"
// ============================================================================
/** @file Listener.cpp
 *  Implementation file for class : Listener
 *  Helepr class to listen incidents 
 *  @see IIncidentSvc
 *  @see IIncidentListener
 *  @see  Incident
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2010-04-03
 */
// ============================================================================
// default constructor (empty)
// ============================================================================
LoKi::Listener::Listener() 
  : LoKi::AuxFunBase() 
  , m_incSvc    ()
  , m_incidents () 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Listener::~Listener()
{
  while ( !m_incidents.empty()  && m_incSvc.validPointer() )
  {
    m_incSvc->removeListener ( this , m_incidents.back() ) ;
    m_incidents.pop_back() ;
  }
}
// ============================================================================
// subscribe the incident 
// ============================================================================
StatusCode LoKi::Listener::subscribe  ( const std::string& incident ) 
{
  // specific incident ? 
  Incidents::const_iterator ifind = std::find 
    ( m_incidents.begin() , m_incidents.end() , incident ) ;
  if ( m_incidents.end() != ifind ) 
  {
    return Warning ( "subscribe: Incident '" + incident + "' alsready in the list", 
                     StatusCode( StatusCode::SUCCESS , true ) ) ;
    
  }
  // 
  if ( !m_incSvc ) 
  {
    SmartIF<IIncidentSvc> iis ( lokiSvc().getObject() ) ;
    m_incSvc = iis ;
  }
  Assert ( !(!m_incSvc) , "Unable to get IIncident Service" ) ;
  //
  m_incSvc->addListener ( this , incident ) ;
  m_incidents.push_back ( incident ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// unsibscrive the incident
// ============================================================================
StatusCode LoKi::Listener::unsubscribe ( const std::string& incident ) 
{
  // no incidents? 
  if ( m_incidents.empty() ) 
  {
    StatusCode sc ( 810 , true ) ;
    return Error ( "unsubscribe: Empty list of incidents!", sc ) ;
  }
  // all incidents? 
  if ( incident.empty() ) 
  {
    //
    if ( !m_incSvc ) 
    {
      SmartIF<IIncidentSvc> iis ( lokiSvc().getObject() ) ;
      m_incSvc = iis ;
    }
    Assert ( !(!m_incSvc) , "Unable to get IIncident Service" ) ;
    //
    while ( !m_incidents.empty() )
    {
      m_incSvc->removeListener ( this , m_incidents.back() ) ;
      m_incidents.pop_back() ;
    }
  }
  // specific incident ? 
  Incidents::iterator ifind = std::find 
    ( m_incidents.begin() , m_incidents.end() , incident ) ;
  if ( m_incidents.end() == ifind ) 
  {
    StatusCode sc ( 811 , true ) ;
    return Error ( "unsubscribe: Unknown incident type: '" + incident + "'", sc ) ;
  }
  //
  if ( !m_incSvc ) 
  {
    SmartIF<IIncidentSvc> iis ( lokiSvc().getObject() ) ;
    m_incSvc = iis ;
  }
  Assert ( !(!m_incSvc) , "Unable to get IIncident Service" ) ;
  //
  m_incSvc->removeListener ( this , incident ) ;
  m_incidents.erase ( ifind ) ;
  //
  return StatusCode::SUCCESS ;
}
// ========================================================================
/* IInterface::release need to be modify to avoid ``suicidal''-behaviour
 * @see IInterface 
 */
// ========================================================================
unsigned long LoKi::Listener::release() 
{
  if ( m_refCount ) { --m_refCount ; }
  // no suicide here!!!
  return m_refCount ;
}
// ========================================================================

// ============================================================================
// The END
// ============================================================================
