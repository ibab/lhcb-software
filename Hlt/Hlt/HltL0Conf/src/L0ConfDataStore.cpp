// $Id: L0ConfDataStore.cpp,v 1.3 2007-11-16 11:08:50 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "L0ConfDataStore.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0ConfDataStore
//
// 2007-04-10 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0ConfDataStore );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0ConfDataStore::L0ConfDataStore( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<L0ConfDataStore>(this);

}
//=============================================================================
// Destructor
//=============================================================================
L0ConfDataStore::~L0ConfDataStore() {} 

//=============================================================================

StatusCode L0ConfDataStore::initialize()
{
 StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  debug() << " ==> Initialize "<< endmsg;

// Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  return StatusCode::SUCCESS;
}

  void L0ConfDataStore::clearStore()
{
  //variables per event
  nL0Candidates =-1;

  //variables per L0 cand
  seedingTime.clear();
  decodingTime.clear();
  trackingTime.clear();
  nTHits.clear();
  region.clear();
  refinedStatePos.clear();
  refinedStateNeg.clear();
   
}


void L0ConfDataStore::handle ( const Incident& incident )
{
  
  if ( IncidentType::BeginEvent == incident.type() ) { 
  
    this->clearStore();
  }
}
