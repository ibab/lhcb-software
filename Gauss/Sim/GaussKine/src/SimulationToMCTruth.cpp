// $Id: SimulationToMCTruth.cpp,v 1.1 2008-10-20 08:08:54 robbep Exp $
// Include files 

// local 
#include "SimulationToMCTruth.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/Vector3DTypes.h"

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
DECLARE_ALGORITHM_FACTORY( SimulationToMCTruth );


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
{ 
  declareProperty( "Particles", 
                   m_particles = LHCb::MCParticleLocation::Default ); 
  declareProperty( "Vertices" , 
                   m_vertices = LHCb::MCVertexLocation::Default ); 
  declareProperty( "GiGaService",    m_gigaSvcName  = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName  = IGiGaCnvSvcLocation::Kine );
}

//=============================================================================
// Destructor
//=============================================================================
SimulationToMCTruth::~SimulationToMCTruth(){};

//=============================================================================
// Initialization
//=============================================================================
StatusCode SimulationToMCTruth::initialize() {
  const StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm 
  
  debug() << "==> Initialize" << endmsg;
  m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc", true );

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!

  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );

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
  m_mcHeader = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);

  // clear set of treated MCParticles
  m_treatedParticles.clear() ;

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
    if( 0 == primvtx ) {
      return Error("Primary vertex not found !");
    }
    
    convert( genpart , primvtx ) ;
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
  
  debug() << "Conversion G4 -> MCTruth is finished" << endmsg ;  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Convert method to fill a MCParticle
//=============================================================================
void SimulationToMCTruth::convert( const HepMC::GenParticle * part , 
				   LHCb::MCVertex * prodvertex ) {
  LHCb::MCParticle * mcpart = 0;
  int barcode = part -> barcode() ;
  
  if ( barcode < MCTruthManager::SplitBarCode ) { 
    // normal case
    mcpart = new LHCb::MCParticle();

    // fill the barcode to pointer map
    GiGaKineRefTable& table = m_gigaKineCnvSvc -> table() ;
    table( barcode )  = GiGaKineRefTableEntry( mcpart , barcode ) ;
    
    m_particleContainer -> insert( mcpart );
    
    mcpart -> setMomentum     ( Gaudi::LorentzVector( part->momentum() ) ) ;
    mcpart -> setParticleID   ( LHCb::ParticleID( part->pdg_id() ) ) ;
    
    // Check if the particle from Geant4 has already a link to a MCParticle
    MCTruthManager * mcmanager = MCTruthManager::GetInstance();
    LHCb::MCParticle * linkedMother = mcmanager -> GetMotherMCParticle( barcode ) ;
    if ( 0 != linkedMother ) {
      // If yes, attach this new MCParticle to the already existing mother
      LHCb::MCVertex * linkedVertex = 
        const_cast< LHCb::MCVertex *>(linkedMother -> endVertices().front().data()) ;
      linkedVertex -> addToProducts( mcpart ) ;
      mcpart -> setOriginVertex( linkedVertex ) ;
      // and link this mother to the prodvertex of the root particle of the linkedMother
      // decay tree, except if this MCParticle has already been treated.
      bool treatTree = ! std::binary_search( m_treatedParticles.begin() , 
		                             m_treatedParticles.end() ,
	                                     linkedMother -> key() ) ;
      if ( treatTree ) m_treatedParticles.insert( linkedMother -> key() ) ;
      while ( treatTree && ( 0 != linkedMother -> mother() ) ) {
        linkedMother = const_cast< LHCb::MCParticle *>( linkedMother -> mother() ) ;
        treatTree = ! std::binary_search( m_treatedParticles.begin() , m_treatedParticles.end() ,
	                                  linkedMother -> key() ) ;
	if ( treatTree ) m_treatedParticles.insert( linkedMother -> key() ) ;
      }
      if ( treatTree ) { 
        linkedMother -> setOriginVertex( prodvertex ) ;
        prodvertex -> addToProducts( linkedMother ) ;
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
    mcpart = const_cast<LHCb::MCParticle*>( prodvertex->mother() );
  }
  
  HepMC::GenVertex * genendvtx = part -> end_vertex();

  LHCb::MCVertex * mcendvtx = new LHCb::MCVertex();
  m_vertexContainer -> insert( mcendvtx );

  mcendvtx -> setPosition ( Gaudi::XYZPoint(genendvtx->point3d()) );
  mcendvtx -> setTime     ( genendvtx->position().t() );
  mcendvtx -> setMother   ( mcpart );
  int typeID = 
    MCTruthManager::GetInstance() -> GetCreatorID( genendvtx -> barcode() );
  mcendvtx -> setType( vertexType( typeID ) ) ;

  mcpart -> addToEndVertices( mcendvtx ) ;

  // now process the daughters
  for ( HepMC::GenVertex::particles_out_const_iterator 
          daughter=genendvtx->particles_out_const_begin();
        daughter!=genendvtx->particles_out_const_end(); ++daughter ) {
    convert( (*daughter) , mcendvtx ) ;
  }
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
  LHCb::Math::Equal_To< double > cmp( 1.e-6 ) ;
  const SmartRefVector< LHCb::MCVertex > pv = m_mcHeader -> primaryVertices() ;
  for ( SmartRefVector< LHCb::MCVertex >::const_iterator it = pv.begin() ;
        it != pv.end() ; ++it ) {
    Gaudi::XYZVector D = vertex - (*it) -> position() ;
    double distance = D.Mag2() ;
    if ( cmp( distance , 0. ) ) return const_cast< LHCb::MCVertex *>( (*it).data() ) ;
  }
  return 0 ;
}
