// $Id: MIBackground.cpp,v 1.2 2009-09-18 14:52:22 gcorti Exp $
// Include files
 
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
 
// STL
#include <fstream>
//#include <cmath>

// LHCb
#include "Event/GenHeader.h"
 
// local
#include "MIBackground.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MIBackground
//
// 2007-08-10 : Gloria Corti, split of algorithm part from original 
//              CollimatorSourceAlg from Magnus Lieng to allow multiple
//              tools
//-----------------------------------------------------------------------------
 
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MIBackground );
 
 
//===========================================================================
// Constructor
//===========================================================================
MIBackground::MIBackground(const std::string& name, 
                           ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm(name, pSvcLocator)
{
  // TS locations
  declareProperty("HepMCEventLocation", 
                  m_eventLoc = LHCb::HepMCEventLocation::Default );
  declareProperty("GenHeaderLocation", 
                  m_headerLoc = LHCb::GenHeaderLocation::Default );
  declareProperty("GenCollisionLocation", 
                  m_collLoc = LHCb::GenCollisionLocation::Default );


  // Value to save in header
  declareProperty("Luminosity", 
                  m_luminosity = 2.e32 /Gaudi::Units::cm2/Gaudi::Units::s);
  declareProperty("EventType", m_evtType = 0);

  // Sources
  m_sourcesNames.clear();
  declareProperty( "MIBSources", m_sourcesNames );
  
}

//===========================================================================
// Destructor
//===========================================================================
MIBackground::~MIBackground() { } 

//===========================================================================
// Initialization
//===========================================================================
StatusCode MIBackground::initialize() {

  debug() << "Initialize" << endmsg;

  // Initialize the base class
  StatusCode sc = GaudiAlgorithm::initialize( ) ;  
  if ( sc.isFailure( ) ) return sc ;

  // Check if event type has been set
  if ( m_evtType == 0 ) {
    warning() << "Event type not set. For stand-alone use 6xx5xxxx."
              << endmsg;
  } else {
    info() << "Event type " << m_evtType << endmsg;
  }

  // Retrieve list of tools for different sources
  if ( m_sourcesNames.empty() ) {
    std::string text = "You must specify the tools for the sources, ";
    text += "eg. CollimatorSource/TCTVStagedShieldMuons";
    return Error(text);
  }
  
  m_sources.clear();
  for ( std::vector<std::string>::iterator iName = m_sourcesNames.begin();
        m_sourcesNames.end() != iName; ++iName ) {
    IMIBSource* aSource = tool< IMIBSource > ( *iName, this );
    m_sources.push_back( aSource );
    info() << "Will use MIB source " << aSource->name() << endmsg;
  }
  
  return sc ;
}

//===========================================================================
// Execute method (Generate one event)
//===========================================================================
StatusCode MIBackground::execute() {

  debug() << "Execute" << endmsg;

  // MC Event
  LHCb::HepMCEvents* hepMCVector = 
    getOrCreate<LHCb::HepMCEvents,LHCb::HepMCEvents>( m_eventLoc );

  // Collisions
  LHCb::GenCollisions* collVector = 
    getOrCreate<LHCb::GenCollisions,LHCb::GenCollisions>( m_collLoc );

  // Update the GenHeader
  LHCb::GenHeader* genHead = get< LHCb::GenHeader >( m_headerLoc ) ;
  if( !genHead->evType() ) genHead->setEvType( m_evtType );

  // Now generate events
  return generate( genHead, collVector, hepMCVector ); 

}


//=============================================================================
//  Generate an "event" with all sources and keep count of how many are empty
//  and how many have at least one particle
//=============================================================================
StatusCode MIBackground::generate( LHCb::GenHeader* theHeader,
                                   LHCb::GenCollisions* theCollisions,
                                   LHCb::HepMCEvents* theEvents ) {
  
  counter("NCalls") += 1;

  int nGenParticles = 0;
  int nParts = 0;
  StatusCode scMIB = StatusCode::SUCCESS;

  // Loop over various sources, and generate event
  for( std::vector<IMIBSource*>::iterator iMIB = m_sources.begin();
       m_sources.end() != iMIB; ++iMIB ){
    

    scMIB = (*iMIB)->generateEvent( theHeader, theCollisions, theEvents, 
                                    nParts );
    if ( ! scMIB.isSuccess() ) {
      return Error("Particle generation failure", scMIB);
    }

    nGenParticles += nParts;
    debug() << "nParts/nGenParticles = " << nParts << "/" << nGenParticles
            << endmsg;
    
  }
  
  if( nGenParticles == 0 ) {
    counter("NEmptyEvents") += 1;
    return generate( theHeader, theCollisions, theEvents );
  }

  counter("NFilledEvents") += 1;
  counter("NParticles") += nGenParticles;
  
  return StatusCode::SUCCESS;  
}


//===========================================================================
// Finalize
//===========================================================================
StatusCode MIBackground::finalize() {

  debug() << "Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions

} 












