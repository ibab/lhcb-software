// $Id: LoadDDDB.cpp,v 1.3 2007-05-29 16:32:59 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/MsgStream.h"

// from LHCb
#include "Kernel/ICondDBInfo.h"

// local
#include "LoadDDDB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LoadDDDB
//
// 2005-10-14 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LoadDDDB );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LoadDDDB::LoadDDDB( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("Node", m_treeToLoad = "/dd*");
}
//=============================================================================
// Destructor
//=============================================================================
LoadDDDB::~LoadDDDB() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LoadDDDB::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;  

  std::vector<LHCb::CondDBNameTagPair> tmp;
  svc<ICondDBInfo>("CondDBCnvSvc")->defaultTags(tmp);

  std::vector<LHCb::CondDBNameTagPair>::iterator db;
  for ( db = tmp.begin(); db != tmp.end(); ++db ) {
    info() << "Database " << db->first << " tag " << db->second << endmsg;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode LoadDDDB::execute() {

  debug() << "==> Execute" << endmsg;

  info() << "Loading the DDDB" << endmsg;

  try {
    detSvc()->addPreLoadItem(m_treeToLoad);
    detSvc()->preLoad();
  } catch (...) {
    warning() << "Exception!!" << endmsg;
  }  
  
  info() << "done." << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LoadDDDB::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
