// $Id: G4HepMCToMCTruth.cpp,v 1.1.1.1 2006-01-31 11:03:51 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from GiGa 
#include "GiGa/IGiGaSvc.h"

// from LHCb
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
//#include "Event/GenHeader.h"

// local 
#include "G4HepMCToMCTruth.h"

// Boost 
//#include "boost/lexical_cast.hpp"

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
  , partcont(0), vtxcont(0)
{ 
  declareProperty( "Particles", 
                   m_particles = LHCb::MCParticleLocation::Default ); 
  declareProperty( "Vertices" , 
                   m_vertices = LHCb::MCVertexLocation::Default ); 
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm 
  
  debug() << "==> Initialize" << endmsg;
  m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc", true );

  return StatusCode::SUCCESS; 
};

// ============================================================================
// Main execution
// ============================================================================
StatusCode G4HepMCToMCTruth::execute() {

  debug() << "==> Execute" << endmsg;

  // Put empty containers in TES
  partcont = new LHCb::MCParticles();
  vtxcont = new LHCb::MCVertices();
  put(partcont,m_particles);
  put(vtxcont,m_vertices);

  particlemap.clear();

  MCTruthManager* mcmanager = MCTruthManager::GetInstance();

  HepMC::GenEvent* ev = mcmanager->GetCurrentEvent();
  std::vector<int> primbar = mcmanager->GetPrimaryBarcodes();

  for( std::vector<int>::const_iterator it=primbar.begin();
       it!=primbar.end(); ++it ) {

    HepMC::GenParticle* genpart = ev->barcode_to_particle(*it);
    HepMC::GenVertex* genprodvtx = genpart->production_vertex();
    
    LHCb::MCVertex* primvtx = new LHCb::MCVertex();
    vtxcont->insert( primvtx );

    Gaudi::XYZPoint pos( genprodvtx->point3d() );
    primvtx->setPosition( pos );
    primvtx->setTime( genprodvtx->position().t() );

    convert(genpart, primvtx);
  }  
  
  // after having converted the particles, we can now convert the hits
  // particle map contains barcode to pointer mapping
  // ...
  // we could use MCTruthManager (or something similar) to access the G4 hits container

  //
  return StatusCode::SUCCESS;
};

// ============================================================================
// Convert method to fill a MCParticle
// ============================================================================
void G4HepMCToMCTruth::convert(HepMC::GenParticle* part, 
                               LHCb::MCVertex* prodvertex) {

  LHCb::MCParticle* mcpart;
  
  if(part->barcode()<100000000) { // normal case
  
    mcpart = new LHCb::MCParticle();
    // fill it the barcode to pointer map
    particlemap[ part->barcode() ] = mcpart;
    
    partcont->insert( mcpart );
    
    Gaudi::LorentzVector momentum( part->momentum() );
    mcpart->setMomentum( momentum );
    mcpart->setParticleID( part->pdg_id() );
    mcpart->setOriginVertex( prodvertex );

    prodvertex->addToProducts( mcpart );
  }
  else {
    // the barcode > 100000000 indicates than the part represents
    // the same physical partical as its mother; it was due to some
    // interaction that the new GenPart was instanciated
    //
    mcpart = prodvertex->mother();
  }
  
  HepMC::GenVertex* genendvtx = part->end_vertex();

  LHCb::MCVertex* mcendvtx = new LHCb::MCVertex();
  vtxcont->insert( mcendvtx );

  Gaudi::XYZPoint endpos( genendvtx->point3d() );
  mcendvtx->setPosition( endpos );
  mcendvtx->setTime( genendvtx->position().t() );
  mcendvtx->setMother( mcpart );

  mcpart->addToEndVertices( mcendvtx );

  // we loop over daughter and call the same method recursively
  for(HepMC::GenVertex::particles_out_const_iterator 
        daughter=genendvtx->particles_out_const_begin();
      daughter!=genendvtx->particles_out_const_end(); daughter++) 
  {
    convert((*daughter), mcendvtx);
  }
  return;
}

// ============================================================================
//  Finalize
// ============================================================================
StatusCode G4HepMCToMCTruth::finalize() { 
  
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


