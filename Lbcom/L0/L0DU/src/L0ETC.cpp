// $Id: L0ETC.cpp,v 1.1 2007-07-13 18:01:38 pkoppenb Exp $

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IRegistry.h" 
#include "GaudiKernel/IAlgManager.h"

// from RecEvent
#include "Event/SelResult.h"

// the data
#include "Event/RecHeader.h"
#include "Event/ProtoParticle.h"
#include "Event/RecVertex.h"

// local
#include "L0ETC.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : L0ETC
//
// 01/05/2004 : Hugo Ruiz
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0ETC );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0ETC::L0ETC( const std::string& name,
            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ) 
    , m_events(0)
{
    declareProperty( "CollectionName", m_collectionName = "<not set>");
    declareProperty( "ToLookAt", m_toLookAt );
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0ETC::initialize() {
  
  // Initialisation of ETC
  StatusCode sc = GaudiTupleAlg::initialize();
  if (!sc) return sc;
  
  
  // Prepare a list of instances of the PostScaler Algorithm
  // from the list of selections
  
  // Make a list of candidate instance names
  std::vector<std::string> nameList;
  for (std::vector<std::string>::iterator iName=m_toLookAt.begin();
       iName!=m_toLookAt.end(); ++iName ) {
    nameList.push_back("Print"+(*iName));
  }
  
  return StatusCode::SUCCESS ;
  
};
//=============================================================================
// Main execution
//=============================================================================
StatusCode L0ETC::execute() {
  debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  m_events++;
  
  // always pass, will use selection on tag to reduce later...
  setFilterPassed(true);
  
  // This is what it is about...
  Tuple tup = evtCol(m_collectionName);
  
  // pick up the location of the event --
  // this is what makes the tag collection a collection...
  DataObject* pObject = get<DataObject>("/Event");
  if (pObject!=0) {
    sc = tup->column("Address", pObject->registry()->address() ); 
  } else {
    Error("    not able to retrieve IOpaqueAddress");
  }
  
  return sc ;
    
};
