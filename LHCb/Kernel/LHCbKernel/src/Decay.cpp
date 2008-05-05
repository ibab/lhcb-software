// $Id: Decay.cpp,v 1.2 2008-05-05 14:28:07 cattanem Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/Decay.h"
// ============================================================================
/** @file
 *  Implementation file for class LHCb::Decay
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-31
 */
// ============================================================================
// the constructor from the particle property 
// ============================================================================
LHCb::Decay::Item::Item ( const ParticleProperty* pp ) 
  : m_name () 
  , m_pid  () 
  , m_pp ( pp ) 
{
  if ( 0 != m_pp ) { m_name =                    pp -> particle ()   ; }
  if ( 0 != m_pp ) { m_pid  = LHCb::ParticleID ( pp -> pdgID    () ) ; }
}
// ============================================================================
// the constructor from the particle name 
// ============================================================================
LHCb::Decay::Item::Item ( const std::string& name )
  : m_name ( name ) 
  , m_pid  (   ) 
  , m_pp   ( 0 )
{}
// ============================================================================
// the constructor from the particle PID
// ============================================================================
LHCb::Decay::Item::Item ( const LHCb::ParticleID& pid  )
  : m_name (     ) 
  , m_pid  ( pid ) 
  , m_pp   (  0  )
{}
// ============================================================================
// the constructor from the particle PID
// ============================================================================
LHCb::Decay::Item::Item ( const int               pid  )
  : m_name (     ) 
  , m_pid  ( pid ) 
  , m_pp   (  0  )
{}
// ============================================================================    
// validate the item using the service 
// ============================================================================    
StatusCode LHCb::Decay::Item::validate 
(       IParticlePropertySvc* svc  ) const
{
  if ( 0 != m_pp ) 
  { 
    m_name =                    m_pp -> particle ()    ; 
    m_pid  = LHCb::ParticleID ( m_pp -> pdgID    ()  ) ;
    return StatusCode::SUCCESS ;                                      // RETURN 
  }
  // it not possible to validate it!
  if ( 0 == svc )    { return StatusCode::FAILURE ; }                 // RETURN 
  // check by name  
  if ( !m_name.empty() ) 
  {
    m_pp = svc -> find ( m_name ) ;
    if ( 0 == m_pp ) { return StatusCode::FAILURE ; }                 // RETURN 
    m_pid  = LHCb::ParticleID ( m_pp -> pdgID    ()  ) ;
    return StatusCode::SUCCESS ;                                     // RETURN 
  }
  // check by PID 
  if ( LHCb::ParticleID() != m_pid ) 
  {
    m_pp = svc->findByStdHepID ( m_pid.pid()  ) ;
    if ( 0 == m_pp ) { return StatusCode::FAILURE ; }                 // RETURN 
    m_name =                    m_pp -> particle ()    ; 
    return StatusCode::SUCCESS ;                                     // RETURN 
  }
  return StatusCode::FAILURE ;                                        // RETURN 
}
// ============================================================================
// validate the item using the particle property object
// ============================================================================    
StatusCode LHCb::Decay::Item::validate 
( const ParticleProperty* pp  ) const
{
  if      ( 0 != m_pp && 0 == pp ) 
  {
    m_name =                    m_pp -> particle ()    ; 
    m_pid  = LHCb::ParticleID ( m_pp -> pdgID    ()  ) ;
    return StatusCode::SUCCESS  ;                                   // RETURN 
  }
  else if ( 0 != pp ) 
  {
    m_pp   = pp ;
    m_name =                    m_pp -> particle ()    ; 
    m_pid  = LHCb::ParticleID ( m_pp -> pdgID    ()  ) ;
    return StatusCode::SUCCESS ;                                   // RETURN
  }
  return StatusCode::FAILURE ;                                     // RETURN  
}
// ============================================================================
// the default constructor 
// ============================================================================
LHCb::Decay::Decay ()
  : m_mother    () 
  , m_daughters ()
{}
// ============================================================================
// the constructor from mother and daughters 
// ============================================================================
LHCb::Decay::Decay 
( const                   ParticleProperty*   mother    ,   // the mother 
  const std::vector<const ParticleProperty*>& daughters )   // the daughtrers 
  : m_mother    ( mother ) 
  , m_daughters (        ) 
{
  setDaughters ( daughters ) ;
}
// ============================================================================
// the constructor from mother and daughters 
// ============================================================================
LHCb::Decay::Decay 
( const             std::string&              mother    ,   // the mother 
  const std::vector<std::string>&             daughters )   // the daughters 
  : m_mother    ( mother ) 
  , m_daughters (        ) 
{ 
  setDaughters ( daughters ) ;
}
// ============================================================================
// the constructor from mother and daughters 
// ============================================================================
LHCb::Decay::Decay 
( const             LHCb::ParticleID&         mother    ,   // the mother 
  const std::vector<LHCb::ParticleID>&        daughters )   // the daughters 
  : m_mother    ( mother ) 
  , m_daughters (        ) 
{ 
  setDaughters ( daughters ) ;
}
// ============================================================================
// the constructor from mother and daughters 
// ============================================================================
LHCb::Decay::Decay 
( const             int   mother    ,   // the mother 
  const std::vector<int>& daughters )   // the daughters 
  : m_mother    ( mother )
  , m_daughters (        ) 
{ 
  setDaughters ( daughters ) ;
}
// ============================================================================
// validate the decay using the service 
// ============================================================================
StatusCode LHCb::Decay::validate ( IParticlePropertySvc* svc ) const
{
   // validate the mother
  StatusCode sc = m_mother.validate ( svc ) ;
  if ( sc.isFailure() ) { return sc ; }                         // RETURN 
  if ( m_daughters.empty() ) { return StatusCode::FAILURE ; }   // RETURN   
  // loop over daughters 
  for ( Items::iterator idau = m_daughters.begin() ; 
        m_daughters.end() != idau ;  ++idau )
  { 
     sc = idau->validate ( svc ) ; 
    if ( sc.isFailure() ) { return sc ; }                       // RETURN 
  }
  //
   return sc ;                                                   // RETURN 
}
// ============================================================================
// virtual destructor 
// ============================================================================
LHCb::Decay::~Decay () {}
// ============================================================================
// the default printout 
// ============================================================================
std::ostream& LHCb::Decay::fillStream 
( std::ostream& s ) const { return s << toString () ; }
// ============================================================================
// the conversion to the string
// ============================================================================
std::string LHCb::Decay::toString () const
{
  std::string result = " " + m_mother.name() ;
  result += " ->" ;
  // loop over daughters 
  for ( Items::const_iterator idau = m_daughters.begin() ; 
        m_daughters.end() != idau ;  ++idau ) { result += " " + idau->name() ; }
  //
  return result + " ";
}
// ============================================================================
// set daughters 
// ============================================================================
void LHCb::Decay::setDaughters 
( const std::vector<const ParticleProperty*>& daugs ) 
{
  m_daughters.clear() ;
  for ( std::vector<const ParticleProperty*>::const_iterator ipp  = 
          daugs.begin() ; daugs.end() != ipp ; ++ipp ) 
  {
    Item child ( *ipp ) ;
    m_daughters.push_back ( child ) ;
  }
}
// ============================================================================
// set daughters 
// ============================================================================
void LHCb::Decay::setDaughters 
( const std::vector<std::string>& daugs ) 
{
  m_daughters.clear() ;
  for ( std::vector<std::string>::const_iterator ipp  = 
          daugs.begin() ; daugs.end() != ipp ; ++ipp ) 
  {
    Item child ( *ipp ) ;
    m_daughters.push_back ( child ) ;
  }
}
// ============================================================================
// set daughters 
// ============================================================================
void LHCb::Decay::setDaughters 
( const std::vector<LHCb::ParticleID>& daugs ) 
{
  m_daughters.clear() ;
  for ( std::vector<LHCb::ParticleID>::const_iterator ipp  = 
          daugs.begin() ; daugs.end() != ipp ; ++ipp ) 
  {
    Item child ( *ipp ) ;
    m_daughters.push_back ( child ) ;
  }
}
// ============================================================================
// set daughters 
// ============================================================================
void LHCb::Decay::setDaughters 
( const std::vector<int>& daugs ) 
{
  m_daughters.clear() ;
  for ( std::vector<int>::const_iterator ipp  = 
          daugs.begin() ; daugs.end() != ipp ; ++ipp ) 
  {
    Item child ( *ipp ) ;
    m_daughters.push_back ( child ) ;
  }
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
