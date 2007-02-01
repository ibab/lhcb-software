// $Id: PhysSelEntry.cpp,v 1.1 2007-02-01 18:35:54 hernando Exp $
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 


// local
#include "PhysSelEntry.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : PhysSelEntry
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PhysSelEntry );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhysSelEntry::PhysSelEntry( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("PhysSelLocation", 
                  m_physSelLocation = LHCb::SelResultLocation::Default );
 
  declareProperty("PhysSelNames", m_physSelNames);
  
}
//=============================================================================
// Destructor
//=============================================================================
PhysSelEntry::~PhysSelEntry() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PhysSelEntry::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_decays = m_physSelNames.value();
  for (std::vector<std::string>::iterator it = m_decays.begin();
       it != m_decays.end(); ++it)
    info() << " accepting entry of decay " << (*it) << endreq;

  // initializeHisto(m_histoPhysSel,"PhysSelEntry",0.,1.,1);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PhysSelEntry::execute() {  

  bool ok = HltAlgorithm::beginExecute();
  if (!ok) return stop(" No inputs");
  
  if (!retrieve(m_physSels,m_physSelLocation)) 
    return stop(" No PhysSel report");
  
  ok = false;
  for (SelResults::const_iterator it = m_physSels->begin();
       it != m_physSels->end(); ++it) {
    const SelResult& sel = *(*it);
    if (sel.found()) {
      const std::string decay = sel.decay();
      for (std::vector< std::string >::iterator it2 = m_decays.begin();
           it2 != m_decays.end(); ++it2)
        if ((*it2) == decay) ok = true;
    } 
  }
  
  if (!ok) return stop(" No Selection Found");

  // fillHisto(m_histoPhysSel,1.,1.);
  
  HltAlgorithm::endExecute();
  debug() << " accepted Selection entry " << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PhysSelEntry::finalize() {

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
