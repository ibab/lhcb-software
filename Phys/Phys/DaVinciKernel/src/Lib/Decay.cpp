// $Id: Decay.cpp,v 1.5 2008-04-10 15:46:53 ibelyaev Exp $
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
#include "Kernel/IDecodeSimpleDecayString.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/Decay.h"
// ============================================================================
/** @file
 *  Implementation file for class DaVinci::Decay
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-31
 */
// ============================================================================
// the constructor from the particle property 
// ============================================================================
DaVinci::Decay::Item::Item ( const ParticleProperty* pp ) 
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
DaVinci::Decay::Item::Item ( const std::string& name )
  : m_name ( name ) 
  , m_pid  (   ) 
  , m_pp   ( 0 )
{}
// ============================================================================
// the constructor from the particle PID
// ============================================================================
DaVinci::Decay::Item::Item ( const LHCb::ParticleID& pid  )
  : m_name (     ) 
  , m_pid  ( pid ) 
  , m_pp   (  0  )
{}
// ============================================================================
// the constructor from the particle PID
// ============================================================================
DaVinci::Decay::Item::Item ( const int               pid  )
  : m_name (     ) 
  , m_pid  ( pid ) 
  , m_pp   (  0  )
{}
// ============================================================================    
// validate the item using the service 
// ============================================================================    
StatusCode DaVinci::Decay::Item::validate 
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
StatusCode DaVinci::Decay::Item::validate 
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
DaVinci::Decay::Decay ()
  : m_mother    () 
  , m_daughters ()
{}
// ============================================================================
// the constructor from mother and daughters 
// ============================================================================
DaVinci::Decay::Decay 
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
DaVinci::Decay::Decay 
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
DaVinci::Decay::Decay 
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
DaVinci::Decay::Decay 
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
StatusCode DaVinci::Decay::validate ( IParticlePropertySvc* svc ) const
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
DaVinci::Decay::~Decay () {}
// ============================================================================
// the default printout 
// ============================================================================
std::ostream& DaVinci::Decay::fillStream 
( std::ostream& s ) const { return s << toString () ; }
// ============================================================================
// the conversion to the string
// ============================================================================
std::string DaVinci::Decay::toString () const
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
void DaVinci::Decay::setDaughters 
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
void DaVinci::Decay::setDaughters 
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
void DaVinci::Decay::setDaughters 
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
void DaVinci::Decay::setDaughters 
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
/*  create decay object from the descriptor
 *  @attention only "main" decay is created!
 *  @param descriptor the decay descriptor 
 *  @param decode the decoder tool
 *  @return the constructed decay
 */
// ============================================================================
DaVinci::Decay DaVinci::decay 
( const std::string&        descriptor , 
  IDecodeSimpleDecayString* decoder    ) 
{
  if ( 0 == decoder   ) { return DaVinci::Decay () ; }      // RETURN 
  StatusCode sc = decoder -> setDescriptor ( descriptor ) ;
  if ( sc.isFailure() ) { return DaVinci::Decay () ;  }     // RETURN 
  DaVinci::Decay dec ;
  sc = decoder -> getDecay ( dec ) ;
  if ( sc.isFailure() ) { return DaVinci::Decay () ;  }     // RETURN
  return dec ;
}
// ============================================================================
/*  create all decay objects from the descriptor
 *  @param descriptor the decay descriptor 
 *  @param decode the decoder tool
 *  @return the constructed decay
 */
// ============================================================================
DaVinci::Decays DaVinci::decays
( const std::string&        descriptor , 
  IDecodeSimpleDecayString* decoder    ) 
{
  if ( 0 == decoder   ) { return DaVinci::Decays () ; }      // RETURN 
  StatusCode sc = decoder -> setDescriptor ( descriptor ) ;
  if ( sc.isFailure() ) { return DaVinci::Decays () ;  }     // RETURN 
  DaVinci::Decay dec ;
  sc = decoder -> getDecay ( dec ) ;
  if ( sc.isFailure() ) { return DaVinci::Decays () ;  }     // RETURN
  DaVinci::Decays result ( 1 , dec ) ;
  if ( decoder->is_cc()  )
  {
    DaVinci::Decay dec_cc ;
    sc = decoder -> getDecay_cc ( dec_cc ) ;
    if ( sc.isFailure() ) { return DaVinci::Decays () ;  }    // RETURN 
    result.push_back ( dec_cc ) ;
  }
  return result ;
}
// ============================================================================
/* create all decay objects from the descriptor
 *  @param descriptors the decay descriptors 
 *  @param decode the decoder tool
 *  @return the constructed decay
 */
// ============================================================================
DaVinci::Decays DaVinci::decays
( const std::vector<std::string>& descriptors , 
  IDecodeSimpleDecayString*       decoder     ) 
{
  if ( 0 == decoder ) { return DaVinci::Decays () ; }             // RETURN 
  //
  DaVinci::Decays result ;
  for ( std::vector<std::string>::const_iterator idesc = descriptors.begin() ;
        descriptors.end() != idesc ; ++idesc ) 
  {
    DaVinci::Decays res = decays ( *idesc , decoder ) ;
    if ( res.empty() ) { return DaVinci::Decays () ; }           // RETURN
    result.insert ( result.end() , res.begin() , res.end() ) ;
  }
  return result ;
}

// ============================================================================
// The END
// ============================================================================
