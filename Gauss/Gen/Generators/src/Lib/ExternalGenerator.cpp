// $Id: ExternalGenerator.cpp,v 1.5 2005-11-29 15:44:11 robbep Exp $
// Include files 

// local
#include "Generators/ExternalGenerator.h"

// Gaudi
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/HardInfo.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IDecayTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ExternalGenerator
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExternalGenerator::ExternalGenerator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ) , 
    m_decayTool( 0 ) , 
    m_cutTool  ( 0 ) { 
    declareInterface< ISampleGenerationTool >( this ) ;
    declareProperty( "ProductionTool" , 
                     m_productionToolName = "PythiaProduction" ) ; 
    declareProperty( "DecayTool" , m_decayToolName = "EvtGenDecay" ) ;
    declareProperty( "CutTool" , m_cutToolName = "LHCbAcceptance" ) ;
}

//=============================================================================
// Destructor
//=============================================================================
ExternalGenerator::~ExternalGenerator( ) { ; }

//=============================================================================
// Initialization method
//=============================================================================
StatusCode ExternalGenerator::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // retrieve the particle property service
  IParticlePropertySvc * ppSvc = 
    svc< IParticlePropertySvc >( "ParticlePropertySvc" ) ;  

  // obtain the Decay Tool 
  // (ATTENTION: it has to be initialized before the production tool)
  if ( "" != m_decayToolName ) 
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  // obtain the Production Tool
  m_productionTool = tool< IProductionTool >( m_productionToolName , this ) ;

  // update the particle properties of the production tool
  IParticlePropertySvc::const_iterator iter ;
  for ( iter = ppSvc -> begin() ; iter != ppSvc -> end() ; ++iter ) {
    if ( ! m_productionTool -> isSpecialParticle( *iter ) ) 
      m_productionTool -> updateParticleProperties( *iter ) ;
    // set stable in the Production generator all particles known to the
    // decay tool
    if ( 0 != m_decayTool )
      if ( m_decayTool -> isKnownToDecayTool( (*iter)->pdgID() ) ) 
        m_productionTool -> setStable( *iter ) ;    
  }

  release( ppSvc ) ;

  // obtain the cut tool
  if ( "" != m_cutToolName ) 
    m_cutTool = tool< IGenCutTool >( m_cutToolName , this ) ;

  // now debug printout of Production Tool 
  // has to be after all initializations to be sure correct values are printed
  m_productionTool -> printRunningConditions( ) ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Decay heavy excited particles
//=============================================================================
StatusCode ExternalGenerator::decayHeavyParticles( HepMC::GenEvent * theEvent, 
                                                   const double mass ,
                                                   const int pid ) 
  const {
  StatusCode sc ;
  
  m_decayTool -> disableFlip() ;

  HepMC::GenEvent::particle_iterator it ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++ it ) {
    
    if ( ( (*it) -> momentum().m() * GeV > mass ) &&
         ( 1 == (*it) -> status() ) && ( pid != abs( (*it) -> pdg_id() ) ) ) {
      
      if ( m_decayTool -> isKnownToDecayTool( (*it) -> pdg_id() ) ) {
        sc = m_decayTool -> generateDecayWithLimit( theEvent , *it , pid ) ;
        if ( ! sc.isSuccess() ) return sc ;
        // if excited particle is unknown to EvtGen give error to oblige
        // us to define it in EvtGen decay table
      } else return Error( "Unknown undecayed excited particle !" ) ;
      
    }
  }  

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Check the presence of a particle of given type in event
// Attention : pidList must be sorted before begin used in this function
//=============================================================================
bool ExternalGenerator::checkPresence( const PIDs & pidList ,
                                       const HepMC::GenEvent * theEvent ,
                                       ParticleVector & particleList )
{
  particleList.clear( ) ;
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++it ) 
    if ( std::binary_search( pidList.begin() , pidList.end() ,
                             (*it) -> pdg_id() ) ) 
      if ( HepMCUtils::IsBAtProduction( *it ) ) particleList.push_back( *it ) ;

  return ( ! particleList.empty() ) ;
}

//=============================================================================
// invert the event
//=============================================================================
void ExternalGenerator::revertEvent( HepMC::GenEvent * theEvent ) {
  HepMC::GenEvent::vertex_iterator itv ;
  double x, y, z, t ;
  for ( itv = theEvent -> vertices_begin() ;
        itv != theEvent -> vertices_end() ; ++itv ) {
    x = (*itv) -> position().x() ;
    y = (*itv) -> position().y() ;
    z = (*itv) -> position().z() ;
    t = (*itv) -> position().t() ;
    (*itv) -> set_position( HepLorentzVector( x, y, -z, t ) ) ;
  }

  HepMC::GenEvent::particle_iterator itp ;
  double px, py, pz, E ;
  for ( itp = theEvent -> particles_begin() ;
        itp != theEvent -> particles_end() ; ++itp ) {
    px = (*itp) -> momentum().px() ;
    py = (*itp) -> momentum().py() ;
    pz = (*itp) -> momentum().pz() ;
    E  = (*itp) -> momentum().e() ;
    (*itp) -> set_momentum( HepLorentzVector( px, py, -pz, E ) ) ;
  }      
}

//=============================================================================
// count the number of particles with pz > 0
//=============================================================================
unsigned int ExternalGenerator::nPositivePz( const ParticleVector 
                                             & particleList ) {
  ParticleVector::const_iterator iter ;
  unsigned int nP = 0 ;
  for ( iter = particleList.begin() ; iter != particleList.end() ; ++iter ) 
    if ( (*iter)->momentum().pz() > 0 ) nP++ ;
  
  return nP ;
}


//=============================================================================
// Set up event
//=============================================================================
void ExternalGenerator::prepareInteraction( EventVector & theEventVector , 
                                            HardVector & theHardVector , 
                                            HepMC::GenEvent * & theGenEvent ,
                                            HardInfo * & theHardInfo ) const {
  HepMCEvent * theHepMCEvent = new HepMCEvent( m_productionTool -> name() ,
                                               1 , 1 ) ;
  theHardInfo = new HardInfo() ;
  
  theGenEvent = theHepMCEvent -> pGenEvt() ;
  theHardInfo -> setEvent( theHepMCEvent ) ;

  theEventVector.push_back( theHepMCEvent ) ;
  theHardVector.push_back( theHardInfo ) ;
}

