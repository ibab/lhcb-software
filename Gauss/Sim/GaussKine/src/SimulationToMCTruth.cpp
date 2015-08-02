// $Id: SimulationToMCTruth.cpp,v 1.8 2010-01-26 10:41:30 silviam Exp $
// Include files 

// local 
#include "SimulationToMCTruth.h"
#include "MCInterfaces/IFlagSignalChain.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/Vector3DTypes.h"
// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// from GiGa 
#include "GiGa/IGiGaSvc.h"

// from GiGaCnv
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// From Geant4 
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

// From GaussTools
#include "GaussTools/MCTruthManager.h"

// from LHCb
#include "LHCbMath/LHCbMath.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHeader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimulationToMCTruth
//
// 2005-09-23 : Witold Pokorski
// 2006-01-31 : Gloria Corti
// 2008-10-07 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SimulationToMCTruth )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimulationToMCTruth::SimulationToMCTruth(const std::string& name, 
                                         ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm( name , pSvcLocator) 
  , m_gigaSvc          ( 0 )
  , m_gigaKineCnvSvc   ( 0 )
  , m_particleContainer( 0 )
  , m_vertexContainer  ( 0 )
  , m_mcHeader         ( 0 )  
  , m_ppSvc            ( 0 )
  , m_setSignalFlagTool( 0 )
  , m_intermediatePDG  ( 0 )
{ 
  declareProperty( "Particles", 
                   m_particles = LHCb::MCParticleLocation::Default ); 
  declareProperty( "Vertices" , 
                   m_vertices = LHCb::MCVertexLocation::Default ); 
  declareProperty( "GiGaService",    m_gigaSvcName  = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName  = IGiGaCnvSvcLocation::Kine );
  declareProperty( "CheckForUnknownParticle" , m_checkUnknown = false ) ;
  declareProperty( "MCHeader" , m_mcHeaderLocation = LHCb::MCHeaderLocation::Default ) ;
}

//=============================================================================
// Destructor
//=============================================================================
SimulationToMCTruth::~SimulationToMCTruth(){}

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimulationToMCTruth::initialize() {
  const StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm 
  
  debug() << "==> Initialize" << endmsg;
  m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc", true );
  
  const LHCb::ParticleProperty * interm = m_ppSvc -> find( "Intermediate" ) ;
  if ( 0 != interm ) m_intermediatePDG = interm -> pdgID().pid() ;

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!

  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );

  // get tool to set signal flag
  m_setSignalFlagTool = tool< IFlagSignalChain >( "FlagSignalChain" );

  return sc; 
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SimulationToMCTruth::execute() {
  debug() << "==> Execute" << endmsg;
  debug() << "Start conversion G4 -> MCTruth" << endmsg ;
  
  // Get containers in TES:
  if ( exist< LHCb::MCParticles >( m_particles ) ) 
    m_particleContainer = get< LHCb::MCParticles >( m_particles ) ;
  else {
    // Put empty containers in TES
    m_particleContainer = new LHCb::MCParticles();
    put(m_particleContainer , m_particles ) ;
  }
  
  // Same with vertices
  if ( exist< LHCb::MCVertices >( m_vertices ) ) 
    m_vertexContainer = get< LHCb::MCVertices >( m_vertices ) ;
  else {
    m_vertexContainer = new LHCb::MCVertices();
    put( m_vertexContainer , m_vertices ) ;
  }
  
  // Retrieve MCHeader (filled initially in SimInit)
  m_mcHeader = get<LHCb::MCHeader>( m_mcHeaderLocation );

  // clear set of treated MCParticles
  m_treatedParticles.clear() ;
  
  // store in an intermediate vector the particles already created
  // then at the end of the processing, these particles with a null production
  // vertex must be deleted
  std::vector< LHCb::MCParticle * > particleVector ;
  LHCb::MCParticles::const_iterator ip;
  for( ip = m_particleContainer -> begin() ; m_particleContainer -> end() != ip;
       ++ip ) particleVector.push_back( (*ip) ) ;

  // clear reference table
  GiGaKineRefTable& table = m_gigaKineCnvSvc->table();
  table.clear();

  // retrieve primary event from Geant4 
  const G4Event * g4Event = 0 ;
  m_gigaSvc -> retrieveEvent( g4Event ) ;

  // Check if event is empty
  if( g4Event -> GetNumberOfPrimaryVertex() == 0 ) {
    warning() << "The g4Event has zero primary vertices!" << endreq ;
    return StatusCode::SUCCESS;
  }

  const G4PrimaryVertex * g4Vertex = g4Event -> GetPrimaryVertex() ;
  std::vector<LHCb::MCVertex*> primariesVtx ;
  
  while( 0 != g4Vertex ) {
    LHCb::MCVertex * primVtx = findPrimaryVertex( Gaudi::XYZPoint( g4Vertex -> GetPosition() ) ) ;
    if ( 0 == primVtx ) {
      warning() << "The MCVertex primary vertex does not exist already !" << endreq ;
      warning() << "It will be recreated now but this is not normal." << endreq ;
      primVtx = new LHCb::MCVertex();
      m_vertexContainer -> insert( primVtx ) ;
      primariesVtx.push_back( primVtx );
    
      primVtx->setPosition( Gaudi::XYZPoint( g4Vertex -> GetPosition() ) );
      primVtx->setTime( g4Vertex->GetT0() );
      primVtx->setType( LHCb::MCVertex::ppCollision );

      // if this is a b- or c-quark, give it a different vertex type
      
      m_mcHeader -> addToPrimaryVertices( primVtx ) ;
    }  
    
    g4Vertex = g4Vertex->GetNext();
  }

  // Now fill all the particles and vertices via HepMC from G4
  MCTruthManager * mcmanager = MCTruthManager::GetInstance();

  HepMC::GenEvent * ev = mcmanager -> GetCurrentEvent() ;
  std::vector< int > primbar = mcmanager -> GetPrimaryBarcodes() ;

  for ( std::vector<int>::const_iterator it=primbar.begin() ;
        primbar.end()!=it ; ++it ) {
    HepMC::GenParticle * genpart    = ev -> barcode_to_particle( *it ) ;
    HepMC::GenVertex   * genprodvtx = genpart -> production_vertex(  ) ;
    
    // Check to which primary vertex they belong to
    LHCb::MCVertex * primvtx = findPrimaryVertex( Gaudi::XYZPoint( genprodvtx -> point3d() ) ) ;
    if( 0 == primvtx ) return Error("Primary vertex not found !");
    
    convert( genpart , primvtx , true ) ;
  }
  
  // Reset oscillation flags
  std::vector< int > oscillbar = mcmanager -> GetOscillatedBarcodes() ;
  for ( std::vector< int >::const_iterator iosc = oscillbar.begin() ;
        oscillbar.end() != iosc ; ++iosc ) {   
    LHCb::MCParticle * mcp = table[ *iosc ].particle() ;
    for( SmartRefVector<LHCb::MCVertex>::const_iterator endV = mcp->endVertices().begin();
         mcp->endVertices().end() != endV ; ++endV ) {
      if( (*endV)->type() == LHCb::MCVertex::DecayVertex ) {
        // Note - const const needed here to allow "addToEndVertices" below
        const LHCb::MCVertex* mcvc = *endV ;
        LHCb::MCVertex* mcv = const_cast< LHCb::MCVertex * >( mcvc ) ;
        mcv -> setType( LHCb::MCVertex::OscillatedAndDecay );
      }
    }
  }

  // Look for signal in G4 and propagate the information to its MCParticle
  std::vector< int > sigBar = mcmanager -> GetSignalBarcodes();
  for ( std::vector< int >::const_iterator isig = sigBar.begin();
        sigBar.end() != isig; ++isig ) {
    LHCb::MCParticle* mcp = table[ *isig ].particle();
    mcp->setFromSignal( true );
  } 
 
  // Now eliminates all decay trees which have no production vertices
  // They have been eliminated by Geant4 (for example a Lambda0 has
  // interacted then the decay products don't exist anymore)
  std::vector< LHCb::MCParticle *>::const_iterator ipart;
  for( ipart = particleVector.begin() ; particleVector.end() != ipart;
       ++ipart ) {
    if ( 0 == (*ipart) -> originVertex() ) deleteParticle( (*ipart) ) ;
  }

  // Now propagate the signal information to all MCParticles in decay tree. 
  // Need to loop over MCParticles for cases where signal is not passed to 
  // Geant4 and so cannot have flag. Here so that it is in final container
  for ( ip = m_particleContainer->begin(); 
        ip != m_particleContainer->end(); ip++ ) {
    if ( (*ip)->fromSignal() ) {
      m_setSignalFlagTool->setFromSignalFlag( *ip );
    }
  }

  debug() << "Conversion G4 -> MCTruth is finished" << endmsg ;  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Delete a particle and all decay tree
//=============================================================================
void SimulationToMCTruth::deleteParticle( LHCb::MCParticle * P ) {
  for( SmartRefVector<LHCb::MCVertex>::const_iterator endV = P -> endVertices().begin();
       P -> endVertices().end() != endV ; ++endV ) {
    for ( SmartRefVector<LHCb::MCParticle>::const_iterator prod = (*endV) -> products().begin();
	  prod != (*endV) -> products().end() ; ++prod ) {
      const LHCb::MCParticle * constParticle = *prod ;
      LHCb::MCParticle * Particle = const_cast<LHCb::MCParticle *>( constParticle ) ;
      deleteParticle( Particle ) ;
    }
    const LHCb::MCVertex * constV = *endV ;
    LHCb::MCVertex * V = const_cast<LHCb::MCVertex *>( constV ) ;
    m_vertexContainer -> erase( V ) ;
  }
  m_particleContainer -> erase( P ) ;
}

//=============================================================================
// Convert method to fill a MCParticle
//=============================================================================
void SimulationToMCTruth::convert( const HepMC::GenParticle * part , 
				   LHCb::MCVertex * prodvertex , 
				   bool isPrimary ) {
  LHCb::MCParticle * mcpart = 0;
  int barcode = part -> barcode() ;
  
  // Intermediate particles can have undefined vertex types: correct here
  if ( LHCb::MCVertex::Unknown == prodvertex -> type() ) {
    if ( m_intermediatePDG == part -> pdg_id() ) 
      prodvertex -> setType( LHCb::MCVertex::GenericInteraction ) ;
  }
  
  if ( barcode < MCTruthManager::SplitBarCode ) { 
    // normal case
    mcpart = new LHCb::MCParticle();

    // fill the barcode to pointer map
    GiGaKineRefTable& table = m_gigaKineCnvSvc -> table() ;
    table( barcode )  = GiGaKineRefTableEntry( mcpart , barcode ) ;
    
    m_particleContainer -> insert( mcpart );
    
    mcpart -> setMomentum     ( Gaudi::LorentzVector( part->momentum() ) ) ;
    mcpart -> setParticleID   ( LHCb::ParticleID( part->pdg_id() ) ) ;
    
    if ( m_checkUnknown ) {
      if ( 0 == m_ppSvc -> find( LHCb::ParticleID( part->pdg_id() ) ) ) 
	  warning() << "The particle with pdg_id " << part->pdg_id()
		    << " is not known to LHCb. " 
		    << "Mass is " << part -> generated_mass() << endreq ;
    } 
    
    // Check if the particle from Geant4 has already a link to a MCParticle
    MCTruthManager * mcmanager = MCTruthManager::GetInstance();
    LHCb::MCParticle * linkedMother = mcmanager -> GetMotherMCParticle( barcode ) ;
    if ( 0 != linkedMother ) {
      // If yes, attach this new MCParticle to the already existing mother
      LHCb::MCVertex * linkedVertex = 
        const_cast< LHCb::MCVertex *>(linkedMother -> endVertices().front().data()) ;
      linkedVertex -> addToProducts( mcpart ) ;
      mcpart -> setOriginVertex( linkedVertex ) ;
      
      // if the particle is a primary particle, then the tree has already been
      // attached to the primary vertex in GenerationToSimulation
      if ( ! isPrimary ) {
        // update vertex position because the root particle may be a long lived
        // charged particle traveling in the magnetic field
        linkedVertex -> setPosition ( Gaudi::XYZPoint( part -> production_vertex() ->point3d()) ) ;
        // and link this mother to the prodvertex of the root particle of the linkedMother
        // decay tree, except if this MCParticle has already been treated.
        bool treatTree = ! std::binary_search( m_treatedParticles.begin() , 
                                               m_treatedParticles.end() ,
                                               linkedMother -> key() ) ;
        if ( treatTree ) m_treatedParticles.insert( linkedMother -> key() ) ;
        while ( treatTree && ( 0 != linkedMother -> mother() ) ) {
          linkedMother = const_cast< LHCb::MCParticle *>( linkedMother -> mother() ) ;
          treatTree = ! std::binary_search( m_treatedParticles.begin() , 
                                            m_treatedParticles.end() ,
                                            linkedMother -> key() ) ;
          if ( treatTree ) {
            m_treatedParticles.insert( linkedMother -> key() ) ;
            linkedVertex = 
              const_cast< LHCb::MCVertex *>(linkedMother -> endVertices().front().data()) ;
            linkedVertex -> setPosition ( Gaudi::XYZPoint( part -> production_vertex() ->point3d()) ) ;
          }
        }
        if ( treatTree ) { 
          linkedMother -> setOriginVertex( prodvertex ) ;
          prodvertex -> addToProducts( linkedMother ) ;
        }
      }  
    } else {
      mcpart -> setOriginVertex ( prodvertex ) ;
      prodvertex->addToProducts( mcpart );
    }
  }
  else {
    // the barcode > MCTruthManager::SplitBarCode indicates than the 
    // part represents
    // the same physical partical as its mother; it was due to some
    // interaction that the new GenPart was instanciated
    //
    // Note - const const needed here to allow "addToEndVertices" below
    // in general this should be avoid but here it provides a quick and 
    // reasonably neat solution
    mcpart = const_cast< LHCb::MCParticle* >( prodvertex -> mother() );
  }

  HepMC::GenVertex * genendvtx = part -> end_vertex();

  if ( ! isEndOfWorldVertex( genendvtx ) ) {

    LHCb::MCVertex * mcendvtx = new LHCb::MCVertex();
    m_vertexContainer -> insert( mcendvtx );

    mcendvtx -> setPosition ( Gaudi::XYZPoint(genendvtx->point3d()) );
    mcendvtx -> setTime     ( genendvtx->position().t() );
    mcendvtx -> setMother   ( mcpart );
    
    if ( 0 != genendvtx -> particles_out_size() ) {
      int typeID = MCTruthManager::GetInstance() -> GetCreatorID( genendvtx -> barcode() );
      LHCb::MCVertex::MCVertexType vType = vertexType( typeID ) ;
      mcendvtx -> setType( vType ) ;
    } else {
      mcendvtx -> 
        setType( vertexType( LHCb::MCVertex::KinematicLimit ) ) ;
    }
    
    mcpart -> addToEndVertices( mcendvtx ) ;
    
    // now process the daughters
    for ( HepMC::GenVertex::particles_out_const_iterator 
	  daughter=genendvtx->particles_out_const_begin();
	  daughter!=genendvtx->particles_out_const_end(); ++daughter ) {
      convert( (*daughter) , mcendvtx , false ) ;
    }
  }
}

//=============================================================================
// Check if the HepMC vertex is at the end of the world
//=============================================================================
bool SimulationToMCTruth::isEndOfWorldVertex( HepMC::GenVertex * ev ) {
  if ( 0 == ev ) return true ;
  if ( 0 != ev -> particles_out_size() ) return false ;
  HepMC::ThreeVector V = ev -> point3d() ;
  if ( ( 50.0 * m == std::fabs(V.x()) ) || ( 50.0 * m == std::fabs(V.y()) ) || 
      ( 50.0 * m == std::fabs(V.z()) ) ) 
    return true ;
  return false ;
}

//=============================================================================
// vertexType
//=============================================================================
LHCb::MCVertex::MCVertexType SimulationToMCTruth::vertexType( int id ) {
  LHCb::MCVertex::MCVertexType vType ;
  switch ( id ) {
    case LHCb::MCVertex::ppCollision: vType = LHCb::MCVertex::ppCollision; break;
    case LHCb::MCVertex::DecayVertex: vType = LHCb::MCVertex::DecayVertex; break;
    case LHCb::MCVertex::OscillatedAndDecay: vType = LHCb::MCVertex::OscillatedAndDecay; break ;
    case LHCb::MCVertex::HadronicInteraction: vType = LHCb::MCVertex::HadronicInteraction; break;
    case LHCb::MCVertex::Bremsstrahlung: vType = LHCb::MCVertex::Bremsstrahlung; break ;
    case LHCb::MCVertex::PairProduction: vType = LHCb::MCVertex::PairProduction; break ;
    case LHCb::MCVertex::Compton: vType = LHCb::MCVertex::Compton; break ;
    case LHCb::MCVertex::DeltaRay: vType = LHCb::MCVertex::DeltaRay; break ;
    case LHCb::MCVertex::PhotoElectric: vType = LHCb::MCVertex::PhotoElectric; break;
    case LHCb::MCVertex::Annihilation: vType = LHCb::MCVertex::Annihilation; break;
    case LHCb::MCVertex::RICHPhotoElectric: vType = LHCb::MCVertex::RICHPhotoElectric; break ;
    case LHCb::MCVertex::Cerenkov: vType = LHCb::MCVertex::Cerenkov; break ;
    case LHCb::MCVertex::LHCHalo: vType = LHCb::MCVertex::LHCHalo; break ;
    case LHCb::MCVertex::MuonBackground: vType = LHCb::MCVertex::MuonBackground; break ;
    case LHCb::MCVertex::MuonBackgroundFlat: vType = LHCb::MCVertex::MuonBackgroundFlat; break ;
    case LHCb::MCVertex::MuonBackgroundSpillover: vType =  LHCb::MCVertex::MuonBackgroundSpillover; break ;
    default: vType = LHCb::MCVertex::Unknown;
  }

  return vType;
}

//==============================================================================
// Find Primary vertex
//==============================================================================
LHCb::MCVertex * SimulationToMCTruth::findPrimaryVertex( const Gaudi::XYZPoint & vertex ) const { 
  //LHCb::Math::Equal_To< double > cmp( 1.e-6 ) ;
  const SmartRefVector< LHCb::MCVertex > pv = m_mcHeader -> primaryVertices() ;
  for ( SmartRefVector< LHCb::MCVertex >::const_iterator it = pv.begin() ;
        it != pv.end() ; ++it ) {
    Gaudi::XYZVector D = vertex - (*it) -> position() ;
    double distance = D.Mag2() ;
    //    if ( cmp( distance , 0. ) ) return const_cast< LHCb::MCVertex *>( (*it).data() ) ;
    if ( LHCb::Math::knuth_equal_to_double( distance , 0. ) ) return const_cast< LHCb::MCVertex *>( (*it).data() ) ;
  }
  return 0 ;
}

