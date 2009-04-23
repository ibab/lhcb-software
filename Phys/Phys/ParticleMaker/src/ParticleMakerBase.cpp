// $Id: ParticleMakerBase.cpp,v 1.2 2009-04-23 10:39:31 pkoppenb Exp $
// Include files

#include "GaudiKernel/DeclareFactoryEntries.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "ParticleMakerBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleMakerBase
//
// 2009-04-21 P. Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleMakerBase::ParticleMakerBase( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ) 
  , m_pid    ( "UNDEFINED" )
  , m_apid   (   ) 
  , m_pp     ( 0 ) 
  , m_app    ( 0 )
{
  declareProperty ( "Input"   , m_input = LHCb::ProtoParticleLocation::Charged ) ; 
  setProperty( "AvoidSelResult", "true").ignore();
  setProperty( "DecayDescriptor", "\"null\"").ignore();
  declareProperty ( "Particle" , m_pid , "Particle to create : pion, kaon, muon..."   ) ;
}
//=========================================================================
//  
//=========================================================================
StatusCode ParticleMakerBase::initialize ( ) {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;
  const std::string Upper ( to_upper( m_pid ) ) ;
  if      ( "PIONS"     == Upper ) { m_pid = "pi+"    ; } 
  else if ( "PI"        == Upper ) { m_pid = "pi+"    ; } 
  else if ( "PION"      == Upper ) { m_pid = "pi+"    ; } 
  else if ( "KAONS"     == Upper ) { m_pid = "K+"     ; } 
  else if ( "KAON"      == Upper ) { m_pid = "K+"     ; } 
  else if ( "K"         == Upper ) { m_pid = "K+"     ; } 
  else if ( "MUONS"     == Upper ) { m_pid = "mu+"    ; }
  else if ( "MUON"      == Upper ) { m_pid = "mu+"    ; }
  else if ( "MU"        == Upper ) { m_pid = "mu+"    ; }
  else if ( "ELECTRONS" == Upper ) { m_pid = "e+"     ; }
  else if ( "ELECTRON"  == Upper ) { m_pid = "e+"     ; }
  else if ( "POSITRON"  == Upper ) { m_pid = "e+"     ; }
  else if ( "E"         == Upper ) { m_pid = "e+"     ; }
  else if ( "PROTONS"   == Upper ) { m_pid = "p+"     ; }
  else if ( "PROTON"    == Upper ) { m_pid = "p+"     ; }
  else if ( "P"         == Upper ) { m_pid = "p+"     ; }
  sc = setPPs( m_pid ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Particle/Antiparticle are unknown for '"+m_pid+"'",sc);}
  if ( 0 == m_pp || 0 == m_app )
  { return Error ( "Particle/Antiparticle are invalid for '"+m_pid+"'"  );}
  
  if (  m_pp->charge() > m_app->charge() ) { std::swap( m_pp , m_app ) ; }
  m_pid  = m_pp  -> particle () ;
  m_apid = m_app -> particle () ;
  
  info() << " Particle/AntiParticle to be created\t " 
         << "'"   << m_pid 
         << "'/'" << m_apid << endmsg ;

  return sc;
}
// ============================================================================
/// set particle properties for particle and for antiparticle  
// ============================================================================
StatusCode ParticleMakerBase::setPPs( const std::string& pid )
{
  if ( 0 == ppSvc() ) { return StatusCode ( 110 ) ; }
  // get the properties of the particle 
  m_pp = ppSvc  () -> find( pid ) ;
  if ( 0 == m_pp    ) { return StatusCode ( 111 ) ; }
  // get the the antiparticle 
  m_app = m_pp -> antiParticle () ;
  if ( 0 == m_app   ) { return StatusCode ( 112 ) ; }
  m_apid = m_app -> particle () ;  
  return StatusCode::SUCCESS ;
};

//=============================================================================
// Destructor
//=============================================================================
ParticleMakerBase::~ParticleMakerBase() {};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleMakerBase::execute() 
{
  LHCb::Particle::Vector newParts ;
  
  StatusCode sc = makeParticles(newParts);
  if (!sc) return sc;

  LHCb::Particle::ConstVector constParts ; /// @todo this is a hack due to CaloParticle...
  for (LHCb::Particle::Vector::const_iterator i = newParts.begin() ; i!= newParts.end() ; ++i) constParts.push_back(*i);
  
  sc = desktop()->saveTrees(constParts);
  if ( sc.isFailure() ){ return Error( "Not able to save desktop" );}

  if ( msgLevel(MSG::DEBUG) ){
    // Log number of vertices and particles
    debug() << "Number of particles in desktop = " 
            << desktop()->particles().size() << endmsg;
    debug() << "Number of primary vertices in desktop = " 
            << desktop()->primaryVertices()->size() << endmsg;
    debug() << "Number of secondary vertices in desktop = " 
            <<desktop()->secondaryVertices().size() << endmsg;
  }

  setFilterPassed((!newParts.empty()));

  return sc;
}
