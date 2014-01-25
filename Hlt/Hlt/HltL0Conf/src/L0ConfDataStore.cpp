// $Id: L0ConfDataStore.cpp,v 1.6 2009-10-29 09:04:25 pkoppenb Exp $
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
DECLARE_TOOL_FACTORY( L0ConfDataStore )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0ConfDataStore::L0ConfDataStore( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , nL0Candidates(0)
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
  
  std::vector<Tf::IStationSelector*>::iterator it;
  for( it=trackHypo.begin();it!=trackHypo.end();it++)delete *it;
  trackHypo.clear();
  
}

bool L0ConfDataStore::checkConsistency()
{
  bool consistent = true;
  
  const unsigned int n=nL0Candidates+1;
  
  if( n != seedingTime.size() ) consistent = false;
  if( n !=  decodingTime.size() ) consistent = false;
  if( n !=  trackingTime.size() ) consistent = false;
  if( n !=  nTHits.size() ) consistent = false;
  if( n !=  region.size() ) consistent = false;
  if( n !=  trackHypo.size() ) consistent = false;

  if( !consistent ){
    warning()<<"Collect, seed prep:           "<<endmsg;
    warning()<<"nL0Candidates          "<<nL0Candidates <<" (should be one less)"<<endmsg;
    warning()<<"seedingTime.size()     "<<seedingTime.size()<<endmsg;
    warning()<<"region.size()          "<<region.size() <<endmsg;
    warning()<<"L0 confirmWithT:          "<<endmsg;
    warning()<<" decodingTime.size()   "<<decodingTime.size() <<endmsg;
    warning()<<"trackingTime.size()    "<<trackingTime.size() <<endmsg;
    warning()<<"nTHits.size()          "<<nTHits.size() <<endmsg;
    
    warning()<<"trackHypo.size()       "<<trackHypo.size() <<endmsg;
    
  }
  return consistent;
}


void L0ConfDataStore::handle ( const Incident& incident )
{
  
  if ( IncidentType::BeginEvent == incident.type() ) { 
  
    this->clearStore();
  }
}
