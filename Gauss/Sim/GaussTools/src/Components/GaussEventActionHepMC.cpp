/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"

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
IMPLEMENT_GiGaFactory( GaussEventActionHepMC );

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
{  
  mcmanager = MCTruthManager::GetInstance();
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GaussEventActionHepMC::~GaussEventActionHepMC()
{
};

// ============================================================================
/** performs the action at the begin of each event 
 *  @param event pointer to Geant4 event object 
 */
// ============================================================================
void GaussEventActionHepMC::BeginOfEventAction( const G4Event* event )
{
  mcmanager->NewEvent();
};

// ============================================================================

// ============================================================================
/** performs the action at the end of each event 
 *  @param event pointer to Geant4 event object 
 */
// ============================================================================
void GaussEventActionHepMC::EndOfEventAction( const G4Event* event )
{
  debug() << "Current event: " << endmsg;

  std::vector<int>& primaries = mcmanager->GetPrimaryBarcodes();

  for(std::vector<int>::const_iterator it = primaries.begin();
      it!=primaries.end(); it++) {
    
    debug() << "primary number: " << (*it) << endmsg;
    
    HepMC::GenParticle* primary = 
      mcmanager->GetCurrentEvent()->barcode_to_particle(*it);

    if( msgLevel( MSG::DEBUG ) ) { 
      DumpTree(primary, 0);
    }
    
  }
  
};

// ============================================================================
// DumpTree
// ============================================================================

void GaussEventActionHepMC::DumpTree(HepMC::GenParticle* particle, int level) {
  IParticlePropertySvc* m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc", true );

  ParticleProperty* p = m_ppSvc->findByStdHepID( particle->pdg_id() );
  
  std::string name;
  
  if( !p ) {
    name = "XXXXXXX";
  } else {
    name = p->particle();
  }

  std::string offset(level,'|');

  std::cout << offset << "+--->" << name 
            << " PV " << particle->production_vertex()->position()
            << " EV " << particle->end_vertex()->position() << std::endl;

  HepMC::GenVertex* endvtx = particle->end_vertex();
  for(HepMC::GenVertex::particles_out_const_iterator 
        it=endvtx->particles_out_const_begin();
      it!=endvtx->particles_out_const_end(); it++)
  {
    DumpTree((*it),level+1);
  }
}
