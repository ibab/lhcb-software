// $Id: GenerationToSimulation.cpp,v 1.9 2009-03-26 21:42:04 robbep Exp $
// Include files 
// local
#include "GenerationToSimulation.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform4DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"

// from GiGa
#include "GiGa/IGiGaSvc.h" 
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"

// from LHCb
#include "Event/HepMCEvent.h"
#include "Event/MCHeader.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// From Geant4
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleTable.hh"
#include "G4ParticlePropertyTable.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : GenerationToSimulation
//
// 2008-09-24 : Gloria CORTI, Patrick ROBBE
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GenerationToSimulation );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenerationToSimulation::GenerationToSimulation( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm     ( name , pSvcLocator )
  , m_gigaSvc          ( 0 )
  , m_particleContainer( 0 )
  , m_vertexContainer  ( 0 ) {

  declareProperty( "GiGaService",    m_gigaSvcName    = "GiGa" );
  declareProperty( "HepMCEventLocation", 
                   m_generationLocation = LHCb::HepMCEventLocation::Default );
  declareProperty( "Particles" ,
                   m_particlesLocation = LHCb::MCParticleLocation::Default ) ;
  declareProperty( "Vertices" ,
                   m_verticesLocation = LHCb::MCVertexLocation::Default ) ;  
  declareProperty( "TravelLimit" , m_travelLimit = 1e-10*m ) ;
  declareProperty( "LookForUnknownParticles" , m_lookForUnknownParticles = false ) ;
  declareProperty( "SkipGeant" , m_skipGeant4 = false ) ;
  declareProperty( "UpdateG4ParticleProperties" , m_updateG4ParticleProperties = true ) ;
  declareProperty( "MCHeader" , m_mcHeader = LHCb::MCHeaderLocation::Default) ;
}

//=============================================================================
// Destructor
//=============================================================================
GenerationToSimulation::~GenerationToSimulation() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GenerationToSimulation::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endmsg;
  if ( ! m_skipGeant4 ) {
    m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName, true ); // Create if necessary
  
    // If requested by option, update Geant4 particle properties
    // from Gaudi/LHCb ParticlePropertySvc
    if ( m_updateG4ParticleProperties ) {
      LHCb::IParticlePropertySvc * ppSvc = 
        svc< LHCb::IParticlePropertySvc >( "LHCb::ParticlePropertySvc", true );
	
      G4ParticlePropertyTable* PPT = G4ParticlePropertyTable::GetParticlePropertyTable();
      G4ParticleTable * particleTable = G4ParticleTable::GetParticleTable() ;
      // switch off output so that not to print unknown pdg from Gauss
      particleTable -> SetVerboseLevel( 0 ) ;
      for ( int i = 0 ; i < particleTable -> size() ; ++i ) {
        G4ParticleDefinition * PDef = particleTable -> GetParticle( i ) ;
        const LHCb::ParticleProperty * pp = ppSvc->find( LHCb::ParticleID( PDef->GetPDGEncoding() ) ) ;
        if ( 0 != pp ) {
          G4ParticlePropertyData * PPData = PPT -> GetParticleProperty( PDef ) ;
          PPData -> SetPDGMass( pp -> mass() ) ;
          PPData -> SetPDGLifeTime( pp -> lifetime() ) ;
          PPT -> SetParticleProperty( *PPData ) ;
        }
      }
      release( ppSvc ) ;
    }  
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GenerationToSimulation::execute() {
  debug() << "==> Execute" << endmsg;
  
  // Obtain events generation in generation phase
  LHCb::HepMCEvents* generationEvents = 
    get<LHCb::HepMCEvents> ( m_generationLocation );

  // Create containers in TES for MCParticles and MCVertices
  m_particleContainer = new LHCb::MCParticles() ;
  put( m_particleContainer , m_particlesLocation ) ;
  m_vertexContainer = new LHCb::MCVertices() ;
  put( m_vertexContainer , m_verticesLocation ) ;

  if( generationEvents -> size() == 0 ) {
    *gigaSvc() << NULL;
    return StatusCode::SUCCESS ;
  }

  // Obtain the MCHeader info
  LHCb::MCHeader * mcHeader = get<LHCb::MCHeader>( m_mcHeader ) ;

  // Loop over the events (one for each pile-up interaction)
  for( LHCb::HepMCEvents::const_iterator genEvent = generationEvents -> begin() ; 
       generationEvents -> end() != genEvent ; ++genEvent ) {
    HepMC::GenEvent * ev = (*genEvent) -> pGenEvt() ;
    // New event: empty the map of converted particles
    m_g4ParticleMap.clear() ;
    m_mcParticleMap.clear() ;
    m_particlesToDelete.clear() ;
    
    // Determine the position of the primary vertex
    Gaudi::LorentzVector thePV = primaryVertex( ev ) ;
    
    // Set G4PrimaryVertex to this position
    // The Assumption is that all particles transferred to Geant4 for this
    // pile up interaction come from this primary vertex.
    // This means that all particles with non zero lifetimes must be
    // passed to Geant4 and then must be declared to Geant4 so that
    // they can be transported correctly
    G4PrimaryVertex* origVertex = 
      new G4PrimaryVertex( thePV.X() , thePV.Y() , thePV.Z() , thePV.T() ) ;

    // Create at the same time the Primary MCVertex
    LHCb::MCVertex * primaryVertex = new LHCb::MCVertex() ;
    m_vertexContainer -> insert( primaryVertex ) ;
    primaryVertex -> setPosition( Gaudi::XYZPoint( thePV.Vect() ) ) ;
    primaryVertex -> setTime( thePV.T() ) ;
    primaryVertex -> setType( LHCb::MCVertex::ppCollision ) ;

    mcHeader -> addToPrimaryVertices( primaryVertex ) ;
  
    // Set ID of all vertices to 0
    for ( HepMC::GenEvent::vertex_iterator itV = ev -> vertices_begin() ;
          itV != ev -> vertices_end() ; ++itV ) {
      (*itV) -> set_id( 0 ) ;
    }

    std::vector< HepMC::GenParticle *> mctruthList ;
    // Now extract the particles we want to keep in the MC Truth
    for ( HepMC::GenEvent::particle_const_iterator itP = ev -> particles_begin() ;
          itP != ev -> particles_end() ; ++itP ) {
      if ( keep( *itP ) ) { 
        mctruthList.push_back( *itP ) ;
        // Set Id of End Vertex to one, then the particle with 
        // production vertex of id 0 and end vertex of id 1 (or without end vertex)
        // will be the start point of a decay tree to convert to MCTruth
        HepMC::GenVertex * endVertex = (*itP) -> end_vertex() ;
        if ( 0 != endVertex ) endVertex -> set_id ( 1 ) ;
      }
    }
    
    // Send to conversion the good particles 
    std::vector< HepMC::GenParticle * >::iterator it ;
    for ( it = mctruthList.begin() ; mctruthList.end() != it ; ++it ) {
      HepMC::GenVertex * prodVertex = (*it) -> production_vertex() ;
      if ( 0 == prodVertex ) 
        warning() << "The particle has no production vertex !!" << endreq ;
      else if ( 0 == prodVertex -> id() )
        convert( *it , origVertex , primaryVertex , 0 , 0 ) ;
    }
    
    // now remove from primary vertex the particles to delete
    if ( ! m_particlesToDelete.empty() ) {
      std::vector< G4PrimaryParticle * >::const_iterator itDel ;
      for ( itDel = m_particlesToDelete.begin() ; itDel != m_particlesToDelete.end() ; 
            ++itDel ) { 
        removeFromPrimaryVertex( origVertex , *itDel ) ;
        delete *itDel ;
      }
    }
    if ( ! m_skipGeant4 ) *gigaSvc() << origVertex;
  }

  return StatusCode::SUCCESS;
}

//==================================================================================
// Decides if the particle should appear in the MCTruth history
//==================================================================================
bool GenerationToSimulation::keep( const HepMC::GenParticle * particle ) const {
  LHCb::ParticleID pid( particle -> pdg_id() ) ;  
  switch ( particle -> status() ) {
  case LHCb::HepMCEvent::StableInProdGen: return true ;
  case LHCb::HepMCEvent::DecayedByDecayGen: return true ;
  case LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen: return true ;
  case LHCb::HepMCEvent::SignalInLabFrame: return true ;
  case LHCb::HepMCEvent::StableInDecayGen: return true ;
  case LHCb::HepMCEvent::DocumentationParticle: 
    // for some processes, the interesting particle has status 3, in Pythia6
    if ( 24 == particle -> parent_event() -> signal_process_id() ) {
      if ( 23 == pid.abspid() ) return true ;
      else if ( 25 == pid.abspid() ) return true ;
    }
    else if ( 26 == particle -> parent_event() -> signal_process_id() ) {
      if ( 24 == pid.abspid() ) return true ;
      else if ( 25 == pid.abspid() ) return true ;
    } else if ( 102 == particle -> parent_event() -> signal_process_id() ) {
      if ( 25 == pid.abspid() ) return true ;
    }
    return false ;
  case LHCb::HepMCEvent::Unknown: return false ;
  case LHCb::HepMCEvent::DecayedByProdGen: 
    if ( pid.isHadron() ) return true ;
    if ( pid.isLepton() ) return true ;
    if ( pid.isNucleus() ) return true ;
    if ( pid.isDiQuark() ) return false ;
      
    switch ( pid.abspid() ) {
      // quarks
    case LHCb::ParticleID::down    : return false ;
    case LHCb::ParticleID::up      : return false ;
    case LHCb::ParticleID::strange : return false ;
    case LHCb::ParticleID::charm   : return false ;
    case LHCb::ParticleID::bottom  : return false ;
    case LHCb::ParticleID::top     : return false ;
      //
    case                   21      : return false ;  // gluon
      //
    case                   22      : return true  ;  // photon
    case                   23      : // Z0 
      // for some processes, the Z0 to transfer has status 3 instead of 2 (Pythia6)
      if ( 24 == particle -> parent_event() -> signal_process_id() ) {
        return false ;
      }
      return true  ;
    case                   24      : // W+-
      // for some processes, the W+ to transfer has status 3 instead of 2 (Pythia6)
      if ( 26 == particle -> parent_event() -> signal_process_id() ) {
        return false ;
      }
      return true  ;
    case                   25      :   // H_10
      if ( 24 == particle -> parent_event() -> signal_process_id() || 
           26 == particle -> parent_event() -> signal_process_id() || 
           102 == particle -> parent_event() -> signal_process_id() ) {
        return false ;
      }
      return true ;
    case                   32      : return true  ;  // Z'
    case                   33      : return true  ;  // Z''
    case                   34      : return true  ;  // W'+-
    case                   35      : return true  ;  // H_20
    case                   36      : return true  ;  // H_30
    case                   37      : return true  ;  // H+-
      //
    case                   90      : return false ;  // system
    case                   91      : return false ;  // cluster
    case                   92      : return false ;  // string
    case                   93      : return false ;  // indep
    case                   94      : return false ;  // CMshower
    default: return true ;
    }
    return true ;
  default: return false ;
  }
  return false ;
}

//==================================================================================
// Convert a decay tree into either MCParticle directly or to G4PrimaryParticle
// which will be given to Geant4 through the primary vertex
//==================================================================================
void GenerationToSimulation::convert( HepMC::GenParticle *& particle , 
                                      G4PrimaryVertex * pvertexg4 ,
                                      LHCb::MCVertex * originVertex ,
                                      G4PrimaryParticle * motherg4 ,
                                      LHCb::MCParticle * mothermcp ) {
  // Decide if we convert to G4 (1), to MCTruth directly (2) or if
  // we skip the particle (3)
  unsigned char conversionCode = transferToGeant4( particle ) ;
  switch ( conversionCode ) {
  case 1:
    { // convert to G4Primary
      // check first if the particle was already converted (it will happen
      // for particles decaying to string,quarks, ...)
      // if yes and it has no mother, then destroy it and recreate it with 
      // correct mother links [map result is *false*]
      // if yes and it has a mother, then skip because it has already been treated
      // [map result is *true*]
      const int pBarcode = particle -> barcode() ;
      std::map< int , std::pair< bool , G4PrimaryParticle * > >::const_iterator result = 
        m_g4ParticleMap.find( pBarcode ) ;
      if ( result != m_g4ParticleMap.end() ) {
        if ( result -> second.first ) return ;
        else {
          m_g4ParticleMap.erase( pBarcode ) ;
          G4PrimaryParticle * particleToDelete = result -> second.second ;
          m_particlesToDelete.push_back( particleToDelete ) ;
        }
      }
      
      G4PrimaryParticle * g4P = makeG4Particle( particle , mothermcp ) ;
      if ( 0 == motherg4 ) {
        if ( 0 == mothermcp ) m_g4ParticleMap.insert( std::make_pair( pBarcode ,
                                                                      std::make_pair( false , g4P ) ) ) ;
        else m_g4ParticleMap.insert( std::make_pair( pBarcode ,
                                                     std::make_pair( true , g4P ) ) ) ;
        // root particle -> attach to G4 primary vertex
        if ( 0 != pvertexg4 ) pvertexg4 -> SetPrimary( g4P ) ;
        else error() << "Primary vertex points to NULL !" << endreq ;
      } else {
        m_g4ParticleMap.insert( std::make_pair( pBarcode ,
                                                std::make_pair( true , g4P ) ) ) ;
        motherg4 -> SetDaughter( g4P ) ;
      }
      
      pvertexg4 = 0 ;
      originVertex = 0 ;
      motherg4 = g4P ;
      mothermcp = 0 ; }
    break ;
  case 2:
    { // convert to MCParticle
      // check first it the particle has already been treated. In this case skip it
      const int pBarcode = particle -> barcode() ;
      std::map< int , bool >::const_iterator result = m_mcParticleMap.find( pBarcode ) ;
      if ( result != m_mcParticleMap.end() ) return ;
      
      LHCb::MCVertex * endVertex = 0 ;
      LHCb::MCParticle * mcP = makeMCParticle( particle , endVertex ) ;
      if ( 0 != originVertex ) {
        mcP -> setOriginVertex( originVertex ) ;
        originVertex -> addToProducts( mcP ) ;
      }
      m_mcParticleMap.insert( std::make_pair( pBarcode , true ) ) ;
      mothermcp = mcP ;
      originVertex = endVertex ; }
    break ;
  case 3: 
  default:
    // just skip the particle
    break ;
  }
  // now convert all daughters of the HepMC particle, if any
  HepMC::GenVertex * ev = particle -> end_vertex() ;
  if ( 0 != ev ) {
    HepMC::GenVertex::particle_iterator itD ;
    for ( itD = ev -> particles_begin( HepMC::children ) ; 
          itD != ev -> particles_end( HepMC::children ) ; ++itD ) {
      HepMC::GenParticle * P = (*itD) ;
      convert( P , pvertexg4 , originVertex , motherg4 , mothermcp ) ;
    }
  }
}

//===========================================================================================
// decide how to treat the particle
// 1 --> convert to Geant4 for tracking in detector
// 2 --> convert to MCParticle directly, the particle is not sent to Geant4
// 3 --> skip the particle completely
//===========================================================================================
unsigned char GenerationToSimulation::transferToGeant4( const HepMC::GenParticle * p ) const {
  if ( ! keep( p ) ) return 3 ;
  if ( m_skipGeant4 ) return 2 ;
  // decide on the travel length of the particle
  HepMC::GenVertex * ev = p -> end_vertex() ;
  if ( 0 == ev ) { 
    if ( m_lookForUnknownParticles ) 
      if ( 0 == G4ParticleTable::GetParticleTable() -> FindParticle ( p -> pdg_id() ) ) 
        warning() << "The particle " << p -> pdg_id() << " is not known to Geant4 "
                  << "but travels a finite distance" << endreq ;
    return 1 ;
  }
  HepMC::GenVertex * pv = p -> production_vertex() ;
  Gaudi::XYZVector E( ev -> point3d() ) ;
  Gaudi::XYZVector P( pv -> point3d() ) ;
  double dist = ( E - P ).R() ;
  if ( dist < m_travelLimit ) return 2 ;
  // if requested check if the particle we give to G4 is known to it
  if ( m_lookForUnknownParticles ) 
    if ( 0 == G4ParticleTable::GetParticleTable() -> FindParticle ( p -> pdg_id() ) ) 
      warning() << "The particle " << p -> pdg_id() << " is not known to Geant4 "
                << "but travels a finite distance" << endreq ;
  return 1 ;
}

//==============================================================================================
// Create a G4PrimaryParticle from a HepMC Particle
//==============================================================================================
G4PrimaryParticle * GenerationToSimulation::makeG4Particle( HepMC::GenParticle *& particle , 
                                                            LHCb::MCParticle * mcp ) const {
  HepMC::FourVector mom = particle -> momentum() ;
  G4PrimaryParticle * g4P = 
    new G4PrimaryParticle( particle -> pdg_id() , mom.x() , mom.y() , mom.z() ) ;
  g4P -> SetMass( particle -> generated_mass() ) ;
  
  // create a new User information to contain the link to the HepMC
  // particle and signal information for particles with 889 status
  bool isSignalParticle = ( LHCb::HepMCEvent::SignalInLabFrame == ( particle -> status() ) ) ;
  GiGaPrimaryParticleInformation * gInfo =
    new GiGaPrimaryParticleInformation( isSignalParticle , particle -> barcode() , 0 ) ;
  
  HepMC::GenVertex * ev = particle -> end_vertex() ;
  
  if ( 0 != ev ) {
    // Set propertime of the particle
    double life = lifetime( mom , particle -> production_vertex() , ev ) ;
    g4P -> SetProperTime( life ) ;
    
    // check if particle has oscillated, if yes set information accordingly
    const HepMC::GenParticle * B = hasOscillated( particle ) ;
    if ( 0 != B ) { 
      gInfo -> setHasOscillated( true ) ;
      // then skip the particle
      particle = const_cast< HepMC::GenParticle *>( B ) ;
    }
  }
  
  // Now set the link to MCParticle to be able to rebuild history later
  if ( 0 != mcp ) gInfo -> setMotherMCParticle( mcp ) ;
  g4P -> SetUserInformation( gInfo ) ;
  
  return g4P ;
}

//==============================================================================================
// Create a MCParticle from HepMC
//==============================================================================================
LHCb::MCParticle * GenerationToSimulation::makeMCParticle( HepMC::GenParticle *& particle , 
                                                           LHCb::MCVertex *& endVertex ) const {
  LHCb::MCParticle * mcp = new LHCb::MCParticle() ;
  m_particleContainer -> insert( mcp ) ;
  Gaudi::LorentzVector mom( particle -> momentum() ) ;
  LHCb::ParticleID pid( particle -> pdg_id() ) ;
  
  mcp -> setMomentum( mom ) ;
  mcp -> setParticleID( pid ) ;
  
  HepMC::GenVertex * V = particle -> end_vertex() ;
  
  if ( 0 != V ) {
    endVertex = new LHCb::MCVertex() ;
    m_vertexContainer -> insert( endVertex ) ;
    endVertex -> setPosition( Gaudi::XYZPoint( V -> point3d() ) ) ;
    endVertex -> setTime( V -> position().t() ) ;
    endVertex -> setMother( mcp ) ;
    const HepMC::GenParticle * B = hasOscillated( particle ) ;
    if ( 0 != B ) {
      endVertex -> setType( LHCb::MCVertex::OscillatedAndDecay ) ;
      // then skip the daughter particle
      particle = const_cast< HepMC::GenParticle * >( B ) ;
    }      
    else endVertex -> setType( LHCb::MCVertex::DecayVertex ) ;
    mcp -> addToEndVertices( endVertex ) ;
  }
  
  return mcp ;
}

//==============================================================================================
// Determine the primary vertex for this event
//==============================================================================================
Gaudi::LorentzVector GenerationToSimulation::primaryVertex( const HepMC::GenEvent * genEvent ) const {
  Gaudi::LorentzVector result( 0. , 0. , 0. , 0. ) ;
  // First option: get the beam particle and use the decay vertex if it exists
  if ( genEvent -> valid_beam_particles() ) {
    HepMC::GenParticle * P = genEvent -> beam_particles().first ;
    HepMC::GenVertex   * V = P -> end_vertex() ;
    if ( 0 != V ) result = V -> position() ; 
    else error() << "The beam particles have no end vertex !" << endreq ;
  } else if ( 0 != genEvent -> signal_process_vertex() ) { 
    // Second option, use the signal vertex stored in HepMC 
    HepMC::GenVertex * V = genEvent -> signal_process_vertex() ;
    result = V -> position() ;
  } else {
    // Last option, take the production or end vertex of the particle with bar code 1
    HepMC::GenParticle * P = genEvent -> barcode_to_particle( 1 ) ;
    HepMC::GenVertex   * V = 0 ;
    if ( 0 != P ) {
      V = P -> production_vertex() ;
      if ( 0 != V ) result = V -> position() ;
      else {
        V = P -> end_vertex() ;
        if ( 0 != V ) result = V -> position() ;
        else error() << "The first particle has no production vertex and no end vertex !" 
                     << endreq ;
      }
    } else error() << "No particle with barcode equal to 1 !" << endreq ;
  }

  return result ;
}

//===============================================================================================
// Compute the lifetime of a particle
//===============================================================================================
double GenerationToSimulation::lifetime( const HepMC::FourVector mom , 
                                         const HepMC::GenVertex * P ,
                                         const HepMC::GenVertex * E  ) const {
  if ( 0 == E ) return 0. ;
  Gaudi::LorentzVector A( P -> position() ) ;
  Gaudi::LorentzVector B( E -> position() ) ;
  Gaudi::LorentzVector AB = B - A ;
  // switch to mm for time
  AB.SetE( AB.T() * Gaudi::Units::c_light ) ;
  // boost displacement 4-vector to rest frame of particle
  Gaudi::LorentzVector M( mom ) ;
  ROOT::Math::Boost theBoost( M.BoostToCM() ) ;
  Gaudi::LorentzVector ABStar = theBoost( AB ) ;
  
  // switch back to time
  return ABStar.T() / Gaudi::Units::c_light ;
}

//==============================================================================================
// Check if a particle has oscillated
//==============================================================================================
const HepMC::GenParticle * GenerationToSimulation::hasOscillated( const HepMC::GenParticle * P ) const {
  const HepMC::GenVertex* ev = P -> end_vertex () ;
  if ( 0 == ev ) return 0 ; 
  if ( 1 != ev -> particles_out_size () ) return 0 ;
  const HepMC::GenParticle * D = *(ev->particles_out_const_begin()) ;
  if ( 0 == D ) return 0 ;
  if ( -P -> pdg_id() != D -> pdg_id() ) return 0 ;
  return D ;
}

//==============================================================================================
// Remove a particle from a primary vertex
//==============================================================================================
void GenerationToSimulation::removeFromPrimaryVertex( G4PrimaryVertex *& pvertexg4 , 
                                                      const G4PrimaryParticle * particleToDelete ) 
  const {
  // This should be very rare so put a warning to see when it happens
  warning() << "A G4PrimaryParticle will be removed from the G4PrimaryVertex" << endmsg ;
  particleToDelete -> Print() ;
  // Make a new vertex
  G4PrimaryVertex * newVertex = 
    new G4PrimaryVertex( pvertexg4 -> GetX0() , pvertexg4 -> GetY0() , 
                         pvertexg4 -> GetZ0() , pvertexg4 -> GetT0() ) ;

  // copy particle to new vertex, except the one to remove
  G4PrimaryParticle * particle = pvertexg4 -> GetPrimary() ;
  while ( 0 != particle ) {
    if ( particle != particleToDelete ) newVertex -> SetPrimary( particle ) ;
    particle = particle -> GetNext() ;
  }

  delete pvertexg4 ;
  pvertexg4 = newVertex ;
} 
