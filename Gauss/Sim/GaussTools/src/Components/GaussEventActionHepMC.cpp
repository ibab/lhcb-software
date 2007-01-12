/// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
/// GiGa 
//#include "GiGa/GiGaMACROs.h"

/// Local 
#include "GaussEventActionHepMC.h"

// ============================================================================
/**  @file 
 *
 *   Implementation file for class : GaussEventActionHepMC
 *
 *   @author Witek Pokorski
 *   @date 06/09/2005 
 */
// ============================================================================

// ============================================================================
// Factory business
// ============================================================================
DECLARE_TOOL_FACTORY( GaussEventActionHepMC );

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussEventActionHepMC::GaussEventActionHepMC
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaEventActionBase( type , name , parent )
  , m_ppSvc( NULL )
{  
  m_mcManager = MCTruthManager::GetInstance();
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GaussEventActionHepMC::~GaussEventActionHepMC()
{
};

// ============================================================================
// initialize
// ============================================================================
StatusCode GaussEventActionHepMC::initialize() 
{

  StatusCode sc = GiGaEventActionBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by base class

  m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc", true );

  return StatusCode::SUCCESS;

}

// ============================================================================
// finalize
// ============================================================================
StatusCode GaussEventActionHepMC::finalize() 
{

  debug() << "==> Finalize" << endmsg;
  return GiGaEventActionBase::finalize();  // must be called after all other actions

}

// ============================================================================
/** performs the action at the begin of each event 
 *  @param event pointer to Geant4 event object 
 */
// ============================================================================
void GaussEventActionHepMC::BeginOfEventAction( const G4Event* /* event */ )
{
  m_mcManager->NewEvent();
};

// ============================================================================

// ============================================================================
/** performs the action at the end of each event 
 *  @param event pointer to Geant4 event object 
 */
// ============================================================================
void GaussEventActionHepMC::EndOfEventAction( const G4Event* /* event */) {

  debug() << "Current event: " << endmsg;

  // Print with manager method
  // m_mcManager->PrintEvent();

  if( msgLevel( MSG::DEBUG ) ) { 
    const std::vector<int>& primaries = m_mcManager->GetPrimaryBarcodes();

    for(std::vector<int>::const_iterator it = primaries.begin(); 
        primaries.end()!=it; it++) {
    
      debug() << "Primary number: " << *it << endmsg;
    
      HepMC::GenParticle* primary = 
        m_mcManager->GetCurrentEvent()->barcode_to_particle(*it);
      
      DumpTree(primary, " | ");
    }    
  }  
  
};

// ============================================================================
// DumpTree
// ============================================================================

void GaussEventActionHepMC::DumpTree(HepMC::GenParticle* particle, std::string offset) {

  ParticleProperty* p = m_ppSvc->findByStdHepID( particle->pdg_id() );
  
  std::string name;  
  if( !p ) {
    name = "XXXXXXX";
  } else {
    name = p->particle();
  }

  std::cout << offset << "--- " << name << " barcode: " << particle->barcode() 
            << " pdg: " << particle->pdg_id() << " energy: " << particle->momentum().e()
            << " ProdVtx " << particle->production_vertex()->position()
            << " EndVtx " << particle->end_vertex()->position() << std::endl;

  for(HepMC::GenVertex::particles_out_const_iterator 
        it=particle->end_vertex()->particles_out_const_begin();
        it!=particle->end_vertex()->particles_out_const_end(); 
      it++) {

    std::string deltaoffset = "";
    
    if( fmod((*it)->barcode(),10000000) != fmod(particle->barcode(),10000000) ) {
        deltaoffset = " | ";
    }
    
    DumpTree((*it), offset + deltaoffset);
  }
}
