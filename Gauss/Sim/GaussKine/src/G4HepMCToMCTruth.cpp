// $Id: G4HepMCToMCTruth.cpp,v 1.3 2006-04-12 19:02:49 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from GiGa 
#include "GiGa/IGiGaSvc.h"

// from GiGaCnv
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// From Geant4 
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

// from LHCb
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHeader.h"

// local 
#include "G4HepMCToMCTruth.h"

// Boost 
//#include "boost/lexical_cast.hpp"

// Little auxiliary class
class compareTheBarCodes {
public:
  bool operator() (HepMC::GenParticle* partA, HepMC::GenParticle* partB) {
    return partA->barcode() < partB->barcode();
  }
};


//-----------------------------------------------------------------------------
// Implementation file for class : Test
//
// 2005-09-23 : Witold Pokorski
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<G4HepMCToMCTruth>         s_Factory;
const        IAlgFactory&G4HepMCToMCTruthFactory = s_Factory;

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
G4HepMCToMCTruth::G4HepMCToMCTruth(const std::string& name, 
                                   ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm( name , pSvcLocator) 
  , m_gigaSvc      ( 0 )
  , m_gigaKineCnvSvc ( 0 )
  , partcont(0), vtxcont(0)
{ 
  declareProperty( "Particles", 
                   m_particles = LHCb::MCParticleLocation::Default ); 
  declareProperty( "Vertices" , 
                   m_vertices = LHCb::MCVertexLocation::Default ); 
  declareProperty( "GiGaService",    m_gigaSvcName  = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName  = IGiGaCnvSvcLocation::Kine );
};

// ============================================================================
// Destructor
// ============================================================================
G4HepMCToMCTruth::~G4HepMCToMCTruth(){};

// ============================================================================
// Initialization
// ============================================================================
StatusCode G4HepMCToMCTruth::initialize() 
{
  const StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm 
  
  debug() << "==> Initialize" << endmsg;
  m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc", true );

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!

  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );

  return sc; 
}

// ============================================================================
// Main execution
// ============================================================================
StatusCode G4HepMCToMCTruth::execute() {

  debug() << "==> Execute" << endmsg;

  // Put empty containers in TES
  partcont = new LHCb::MCParticles();
  put(partcont,m_particles);
  vtxcont = new LHCb::MCVertices();
  put(vtxcont,m_vertices);

  // fill reference table
  GiGaKineRefTable& table = m_gigaKineCnvSvc->table();
  table.clear();

  // retrieve primary 
  const G4Event* g4Event = 0 ;
  m_gigaSvc->retrieveEvent(g4Event);
  
  const G4PrimaryVertex* g4Vertex = g4Event->GetPrimaryVertex();
  std::vector<LHCb::MCVertex*> primariesVtx;
  while( 0 != g4Vertex ) {
    LHCb::MCVertex* primvtx = new LHCb::MCVertex();
    vtxcont->insert( primvtx );
    primariesVtx.push_back( primvtx );
    
    primvtx->setPosition( Gaudi::XYZPoint( g4Vertex->GetX0() , 
                                           g4Vertex->GetY0() , 
                                           g4Vertex->GetZ0() ) );
    primvtx->setTime( g4Vertex->GetT0() );
    primvtx->setType( LHCb::MCVertex::ppCollision );
    
    g4Vertex = g4Vertex->GetNext();
  }
  info() << "Number of primaries vtx " << vtxcont->size() << "/ "
         << primariesVtx.size() << endmsg;

  // Update the MCHeader info
  LHCb::MCHeader* mcHead = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
  for(std::vector<LHCb::MCVertex*>::const_iterator ivp = primariesVtx.begin();
        primariesVtx.end() != ivp; ++ivp ) {
    mcHead->addToPrimaryVertices(*ivp);
  }

  // Now fill all the particles and vertices via HepMC internal to G4
  MCTruthManager* mcmanager = MCTruthManager::GetInstance();

  HepMC::GenEvent* ev = mcmanager->GetCurrentEvent();
  std::vector<int> primbar = mcmanager->GetPrimaryBarcodes();

  for ( std::vector<int>::const_iterator it=primbar.begin();
        primbar.end()!=it; ++it ) {

    HepMC::GenParticle* genpart = ev->barcode_to_particle(*it);
    HepMC::GenVertex* genprodvtx = genpart->production_vertex();
    
    // Check to which primary vertex they belong to
    LHCb::MCVertex* primvtx = NULL;
    for(std::vector<LHCb::MCVertex*>::const_iterator iv = primariesVtx.begin();
        primariesVtx.end() != iv; ++iv ) {
      if( genprodvtx->position().x() == (*iv)->position().x() &&
          genprodvtx->position().y() == (*iv)->position().y() &&
          genprodvtx->position().z() == (*iv)->position().z() ) {
        primvtx = *iv;
        break;
      }
    }
    if( NULL == primvtx ) {
      return Error("Primary vertex not found, adding one more");
    }
    
    convert(genpart, primvtx);
 
  }
  
  // Reset oscillation flags
  std::vector<int> oscillbar = mcmanager->GetOscillatedBarcodes();
  for( std::vector<int>::const_iterator iosc = oscillbar.begin();
       oscillbar.end() != iosc; ++iosc ) {   
    LHCb::MCParticle* mcp = table[*iosc].particle();
    for(std::vector<LHCb::MCParticle*>::const_iterator ip = partcont->begin();
        partcont->end() != ip; ++ip ) {
      if( *ip == mcp ) {
        for( SmartRefVector<LHCb::MCVertex>::const_iterator 
               endV = mcp->endVertices().begin();
             mcp->endVertices().end() != endV; ++endV ) {
          if( (*endV)->type() == LHCb::MCVertex::DecayVertex ) {
            // Note - const const needed here to allow "addToEndVertices" below
            const LHCb::MCVertex* mcvc = *endV;
            LHCb::MCVertex* mcv = const_cast<LHCb::MCVertex*>( mcvc );
            mcv->setType(LHCb::MCVertex::OscillatedAndDecay);
          }
        }
        break;
      }
    }
  }

  //
  return StatusCode::SUCCESS;
}

// ============================================================================
// Convert method to fill a MCParticle
// ============================================================================
void G4HepMCToMCTruth::convert(HepMC::GenParticle* part, 
                               LHCb::MCVertex* prodvertex) 
{

  LHCb::MCParticle* mcpart = 0;
  
  if(part->barcode()<100000000) { // normal case but the code should be one less zero
  
    mcpart = new LHCb::MCParticle();

    // fill it the barcode to pointer map
    GiGaKineRefTable& table = m_gigaKineCnvSvc->table();
    table( part->barcode() )  = GiGaKineRefTableEntry( mcpart, part->barcode() );
    
    partcont->insert( mcpart );
    
    mcpart->setMomentum     ( Gaudi::LorentzVector(part->momentum()) );
    mcpart->setParticleID   ( part->pdg_id() );
    mcpart->setOriginVertex ( prodvertex );

    prodvertex->addToProducts( mcpart );
  }
  else {
    // the barcode > 10000000 indicates than the part represents
    // the same physical partical as its mother; it was due to some
    // interaction that the new GenPart was instanciated
    //
    // Note - const const needed here to allow "addToEndVertices" below
    // in general this should be avoid but here it provides a quick and 
    // reasonably neat solution
    mcpart = const_cast<LHCb::MCParticle*>( prodvertex->mother() );
  }
  
  HepMC::GenVertex* genendvtx = part->end_vertex();

  LHCb::MCVertex* mcendvtx = new LHCb::MCVertex();
  vtxcont->insert( mcendvtx );

  mcendvtx->setPosition ( Gaudi::XYZPoint(genendvtx->point3d()) );
  mcendvtx->setTime     ( genendvtx->position().t() );
  mcendvtx->setMother   ( mcpart );
  int typeID = 
    MCTruthManager::GetInstance()->GetCreatorID( genendvtx->barcode() );
  mcendvtx->setType( vertexType(typeID) );

  mcpart->addToEndVertices( mcendvtx );

  // we loop over daughter, sort them so that they are always given in
  // same order to the TEE, then we call the same method recursively
  std::vector<HepMC::GenParticle*> outParticles;
  for ( HepMC::GenVertex::particles_out_const_iterator 
          daughter=genendvtx->particles_out_const_begin();
        daughter!=genendvtx->particles_out_const_end(); ++daughter ) {
    outParticles.push_back(*daughter);
  }
  
  std::sort(outParticles.begin(), outParticles.end(), compareTheBarCodes());
    
  for( std::vector<HepMC::GenParticle*>::iterator 
         iOutPart = outParticles.begin();
       outParticles.end() != iOutPart; ++iOutPart ) {
    convert((*iOutPart), mcendvtx);
  }

  return;
}

// ============================================================================
//  vertexType
// ============================================================================
LHCb::MCVertex::MCVertexType G4HepMCToMCTruth::vertexType( int id ) {
  
  LHCb::MCVertex::MCVertexType vType = LHCb::MCVertex::Unknown;
  if( id ==  LHCb::MCVertex::ppCollision ) {
    vType = LHCb::MCVertex::ppCollision;
  } else if( id == LHCb::MCVertex::DecayVertex ) {
    vType = LHCb::MCVertex::DecayVertex;
  } else if( id == LHCb::MCVertex::OscillatedAndDecay ) {
    vType = LHCb::MCVertex::OscillatedAndDecay;
  } else if( id == LHCb::MCVertex::HadronicInteraction ) {
    vType = LHCb::MCVertex::HadronicInteraction;
  } else if( id == LHCb::MCVertex::Bremsstrahlung ) {
    vType = LHCb::MCVertex::Bremsstrahlung;
  } else if( id == LHCb::MCVertex::PairProduction ) {
    vType = LHCb::MCVertex::PairProduction;
  } else if( id == LHCb::MCVertex::Compton ) {
    vType = LHCb::MCVertex::Compton;
  } else if( id == LHCb::MCVertex::DeltaRay ) {
    vType = LHCb::MCVertex::DeltaRay;
  } else if( id == LHCb::MCVertex::PhotoElectric ) {
    vType = LHCb::MCVertex::PhotoElectric;
  } else if( id == LHCb::MCVertex::Annihilation ) {
    vType = LHCb::MCVertex::Annihilation;
  } else if( id == LHCb::MCVertex::RICHPhotoElectric ) {
    vType = LHCb::MCVertex::RICHPhotoElectric;
  } else if( id == LHCb::MCVertex::Cerenkov ) {
    vType = LHCb::MCVertex::Cerenkov;
  } else if( id == LHCb::MCVertex::LHCHalo ) {
    vType = LHCb::MCVertex::LHCHalo;
  } else if( id == LHCb::MCVertex::MuonBackground ) {
    vType = LHCb::MCVertex::MuonBackground;
  } else if( id == LHCb::MCVertex::MuonBackgroundFlat ) {
    vType = LHCb::MCVertex::MuonBackgroundFlat;
  } else if( id == LHCb::MCVertex::MuonBackgroundSpillover ) {
    vType = LHCb::MCVertex::MuonBackgroundSpillover;
  }

  return vType;

}

// ============================================================================
//  Finalize
// ============================================================================
StatusCode G4HepMCToMCTruth::finalize() 
{   
  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


