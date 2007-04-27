// $Id: L0ConfDataStore.cpp,v 1.2 2007-04-27 13:26:13 albrecht Exp $
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
// Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  return StatusCode::SUCCESS;
}

void L0ConfDataStore::clearStore()
{

  region = -1;
  nModulesToDecode = 0;
  decodingTime = 0;
  collectTime = 0;
  trackingTime = 0;
  nStClusters = 0;
  nOtClusters = 0;
  nTHits = 0;
}


void L0ConfDataStore::handle ( const Incident& incident )
{
  
  if ( IncidentType::BeginEvent == incident.type() ) { 
  
    this->clearStore();
  }
}
