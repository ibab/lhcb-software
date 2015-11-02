// Include files

// from Gaudi
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

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
DECLARE_ALGORITHM_FACTORY( LoadDDDB )


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
LoadDDDB::~LoadDDDB() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode LoadDDDB::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "==> Initialize" << endmsg;

  std::vector<LHCb::CondDBNameTagPair> tmp;
  svc<ICondDBInfo>("CondDBCnvSvc",true)->defaultTags(tmp);

  std::vector<LHCb::CondDBNameTagPair>::iterator db;
  for ( db = tmp.begin(); db != tmp.end(); ++db ) {
    info() << "Database " << db->first << " tag " << db->second << endmsg;
  }

  updMgrSvc(); // trigger the initialization of the Condition Update sub-system
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LoadDDDB::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  info() << "Loading the DDDB" << endmsg;

  try {
    detSvc()->addPreLoadItem(m_treeToLoad);
    detSvc()->preLoad();
  } catch (GaudiException &x) {
    fatal() << "Gaught GaudiException" << endmsg;
    int i = 0;
    for ( GaudiException *ex = &x; 0 != ex; ex = ex->previous() ) {
      fatal() << std::string(i++,' ') << " ==> " << ex->what() << endmsg;
    }
    return StatusCode::FAILURE;
  } catch (std::exception &x) {
    fatal() << "Gaught exception '" << System::typeinfoName(typeid(x)) << "'"
              << endmsg;
    fatal() << " ==> " << x.what() << endmsg;
    return StatusCode::FAILURE;
  } catch (...) {
    fatal() << "Gaught unknown exception!!" << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "done." << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LoadDDDB::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
