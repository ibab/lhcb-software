// $Id: Decay.cpp,v 1.1 2008-12-03 17:35:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleID.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
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
Decays::Decay::Item::Item ( const LHCb::ParticleProperty* pp ) 
  : m_name () 
  , m_pid  () 
  , m_pp ( pp ) 
{
  if ( 0 != m_pp ) { m_name = pp -> particle   () ; }
  if ( 0 != m_pp ) { m_pid  = pp -> particleID () ; }
}
// ============================================================================
// the constructor from the particle name 
// ============================================================================
Decays::Decay::Item::Item ( const std::string& name )
  : m_name ( name ) 
  , m_pid  (   ) 
  , m_pp   ( 0 )
{}
// ============================================================================
// the constructor from the particle PID
// ============================================================================
Decays::Decay::Item::Item ( const LHCb::ParticleID& pid  )
  : m_name (     ) 
  , m_pid  ( pid ) 
  , m_pp   (  0  )
{}
// ============================================================================    
// validate the item using the service 
// ============================================================================    
StatusCode Decays::Decay::Item::validate 
( const LHCb::IParticlePropertySvc* svc  ) const
{
  if ( 0 != m_pp ) 
  {
    m_name = m_pp -> particle   () ; 
    m_pid  = m_pp -> particleID () ;
    return StatusCode::SUCCESS  ;                                   // RETURN 
  }  
  // it not possible to validate it!
  if ( 0 == svc )    { return StatusCode::FAILURE ; }                 // RETURN 
  // check by name  
  if ( !m_name.empty() ) 
  {
    m_pp = svc -> find ( m_name ) ;
    if ( 0 == m_pp ) { return StatusCode::FAILURE ; }                 // RETURN 
    m_pid  = m_pp -> particleID () ;
    return StatusCode::SUCCESS ;                                     // RETURN 
  }
  // check by PID 
  if ( LHCb::ParticleID() != m_pid ) 
  {
    m_pp = svc -> find ( m_pid  ) ;
    if ( 0 == m_pp ) { return StatusCode::FAILURE ; }                 // RETURN 
    m_name = m_pp -> particle ()    ; 
    return StatusCode::SUCCESS ;                                     // RETURN 
  }
  return StatusCode::FAILURE ;                                        // RETURN 
}
// ============================================================================
// validate the item using the particle property object
// ============================================================================    
StatusCode Decays::Decay::Item::validate 
( const LHCb::ParticleProperty* pp  ) const
{
  if      ( 0 != m_pp && 0 == pp ) 
  {
    m_name = m_pp -> particle   () ; 
    m_pid  = m_pp -> particleID () ;
    return StatusCode::SUCCESS  ;                                   // RETURN 
  }
  else if ( 0 != pp ) 
  {
    m_pp   = pp ;
    m_name = m_pp -> particle   () ; 
    m_pid  = m_pp -> particleID () ;
    return StatusCode::SUCCESS ;                                   // RETURN
  }
  return StatusCode::FAILURE ;                                     // RETURN  
}
// ============================================================================
// the default constructor 
// ============================================================================
Decays::Decay::Decay ()
  : m_mother    () 
  , m_daughters ()
{}
// ============================================================================
// the constructor from mother and daughters 
// ============================================================================
Decays::Decay::Decay 
( const                   LHCb::ParticleProperty*   mother    ,   // the mother 
  const std::vector<const LHCb::ParticleProperty*>& daughters )   // the daughtrers 
  : m_mother    ( mother ) 
  , m_daughters (        ) 
{
  setDaughters ( daughters ) ;
}
// ============================================================================
// the constructor from mother and daughters 
// ============================================================================
Decays::Decay::Decay 
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
Decays::Decay::Decay 
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
Decays::Decay::Decay 
( const Decays::Decay::Item&              mother    ,   // the mother 
  const std::vector<Decays::Decay::Item>& daughters )   // the daughters 
  : m_mother    ( mother    ) 
  , m_daughters ( daughters ) 
{}
// ============================================================================
// validate the decay using the service 
// ============================================================================
StatusCode Decays::Decay::validate ( const LHCb::IParticlePropertySvc* svc ) const
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
// the default printout 
// ============================================================================
std::ostream& Decays::Decay::fillStream 
( std::ostream& s ) const 
{
  s << " " << m_mother.name() << " -> " ;
  //
  // loop over daughters 
  for ( Items::const_iterator idau = m_daughters.begin() ; 
        m_daughters.end() != idau ;  ++idau ) { s << " " << idau->name() ; }
  //
  return s << " " ; 
}
// ============================================================================
// the conversion to the string
// ============================================================================
std::string Decays::Decay::toString () const
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// set daughters 
// ============================================================================
void Decays::Decay::setDaughters 
( const std::vector<const LHCb::ParticleProperty*>& daugs ) 
{
  m_daughters.clear() ;
  for ( std::vector<const LHCb::ParticleProperty*>::const_iterator ipp  = 
          daugs.begin() ; daugs.end() != ipp ; ++ipp ) 
  {
    Item child ( *ipp ) ;
    m_daughters.push_back ( child ) ;
  }
}
// ============================================================================
// set daughters 
// ============================================================================
void Decays::Decay::setDaughters 
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
void Decays::Decay::setDaughters 
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

// ============================================================================
// The END
// ============================================================================
