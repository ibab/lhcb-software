/// $Id: GaussEventActionHepMC.cpp,v 1.7 2009-10-21 15:31:04 silviam Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ISvcLocator.h"

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// local 
#include "GaussEventActionHepMC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaussEventActionHepMC
//
// 2005-09-06 : Witek Pokorski
// 2007-09-03 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GaussEventActionHepMC );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaussEventActionHepMC::GaussEventActionHepMC
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaEventActionBase( type , name , parent )
  , m_ppSvc( NULL )
{  
  m_mcManager = MCTruthManager::GetInstance();
}

//=============================================================================
// Destructor
//=============================================================================
GaussEventActionHepMC::~GaussEventActionHepMC() {}

//=============================================================================
// initialize
//=============================================================================
StatusCode GaussEventActionHepMC::initialize() 
{

  StatusCode sc = GiGaEventActionBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by base class

  m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc", true );

  return StatusCode::SUCCESS;

}

//=============================================================================
// finalize
//=============================================================================
StatusCode GaussEventActionHepMC::finalize() 
{

  debug() << "==> Finalize" << endmsg;
  return GiGaEventActionBase::finalize();  // must be called after all other actions

}

//=============================================================================
// performs the action at the begin of each G4 event 
//=============================================================================
void GaussEventActionHepMC::BeginOfEventAction( const G4Event* /* event */ )
{
  m_mcManager->NewEvent();
}


//=============================================================================
// performs the action at the end of each G4 event
//=============================================================================
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
  
}

//=============================================================================
// DumpTree
//=============================================================================
void GaussEventActionHepMC::DumpTree(HepMC::GenParticle* particle, 
                                     std::string offset) {

  const LHCb::ParticleProperty* p = m_ppSvc->find( LHCb::ParticleID(particle->pdg_id()) );
  
  std::string name;  
  if( !p ) {
    name = "XXXXXXX";
  } else {
    name = p->particle();
  }

#ifndef WIN32
#warning REMEMBER: remove temporary fix for problem in compilation on slc5 operator<<
#endif
  std::cout << offset << "--- " << name << " barcode: " << particle->barcode() 
            << " pdg: " << particle->pdg_id() 
            << " energy: " << particle->momentum().e()
    //        << " ProdVtx " << particle->production_vertex()->position()
            << " ProdVtx x" << particle->production_vertex()->position().x()
            << " ProdVtx y" << particle->production_vertex()->position().y()
            << " ProdVtx z" << particle->production_vertex()->position().z()
            << " ProdVtx t" << particle->production_vertex()->position().t()
    //        << " EndVtx " << particle->end_vertex()->position() << std::endl;
            << " EndVtx x" << particle->end_vertex()->position().x() 
            << " EndVtx y" << particle->end_vertex()->position().y() 
            << " EndVtx z" << particle->end_vertex()->position().z() 
            << " EndVtx t" << particle->end_vertex()->position().t() << std::endl;

  for(HepMC::GenVertex::particles_out_const_iterator 
        it=particle->end_vertex()->particles_out_const_begin();
        it!=particle->end_vertex()->particles_out_const_end(); 
      it++) {

    std::string deltaoffset = "";
    
    if( fmod(double((*it)->barcode()),10000000) != 
        fmod(double(particle->barcode()),10000000) ) {
      deltaoffset = " | ";
    }
    
    DumpTree((*it), offset + deltaoffset);
  }
}
