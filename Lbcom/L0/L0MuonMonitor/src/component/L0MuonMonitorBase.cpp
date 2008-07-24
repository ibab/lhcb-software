// $Id: L0MuonMonitorBase.cpp,v 1.4 2008-07-24 09:36:53 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 


#include "Event/L0DUReport.h"
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"

// local
#include "L0MuonMonitorBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitorBase
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// // Declaration of the Algorithm Factory
// DECLARE_ALGORITHM_FACTORY( L0MuonMonitorBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorBase::L0MuonMonitorBase( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  std::vector<int> time_slots;
  for (int i=-7;i<=7;++i) time_slots.push_back(i);
  declareProperty( "TimeSlots"  , m_time_slots = time_slots);

  std::vector<int> quarters;
  for (int i=0;i<4;++i) quarters.push_back(i);
  declareProperty( "Quarters"  , m_quarters = quarters);
  
  std::vector<int> stations;
  for (int i=0;i<5;++i) stations.push_back(i);
  declareProperty( "Stations"  , m_stations = stations);

  std::vector<int> regions;
  for (int i=0;i<4;++i) regions.push_back(i);
  declareProperty( "Regions"  , m_regions = regions);

  std::vector<unsigned int> excludedBxs;
  declareProperty( "ExcludedBxs"  , m_excludedBxs = excludedBxs);

  std::vector<unsigned int> exclusiveBxs;
  declareProperty( "ExclusiveBxs"  , m_exclusiveBxs = exclusiveBxs);

  declareProperty( "TriggerType"  , m_triggerType = -1);

  declareProperty( "ShortNames", m_shortnames = true);
  
}

//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitorBase::~L0MuonMonitorBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitorBase::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitorBase::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitorBase::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================

StatusCode L0MuonMonitorBase::getL0MuonTiles(std::vector<LHCb::MuonTileID> & l0muontiles)
{
  l0muontiles.clear();
  
  std::string location = LHCb::L0MuonDataLocation::Default +context();
  if (  exist<LHCb::L0MuonDatas>(location ) ) 
  {
    LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( location );
    LHCb::L0MuonDatas::const_iterator itdata;
    for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
      LHCb::MuonTileID mkey = (*itdata)->key();    
      std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
      if (ols.size()>0) {
        for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
          if (!itol->isValid()){
            info()<<"PU "<<mkey.toString()<<" tile is not valid : "<<itol->toString()<<endreq;
          }

          if (!quarterInUse(itol->quarter())) continue;
          if (!stationInUse(itol->station())) continue;
          if (!regionInUse(itol->region())  ) continue;
            
          l0muontiles.push_back(*itol);
        }
      }
    }
  } 
  else 
  {
    std::stringstream errstr("");
    errstr<<"getL0MuonTiles : L0MuonDatas not found @"<<rootInTES()<<"/"<<LHCb::L0MuonDataLocation::Default+context();
    return Warning(errstr.str(),StatusCode::FAILURE,5);
  }
  return StatusCode::SUCCESS;
}


bool L0MuonMonitorBase::excludedBx() {

  if (m_excludedBxs.size()==0) return false;

  if (exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) {
    LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::ODIN );
    if (banks.size()==1){
      std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin();
      LHCb::ODIN odin;
      odin.set(*itBnk);
      unsigned int bunch =odin.bunchId() ;
      for (std::vector<unsigned int>::iterator itexcluded=m_excludedBxs.begin(); itexcluded!=m_excludedBxs.end();++itexcluded){
        if (bunch==(*itexcluded) ) return true;
      }
    } else {
      error()<<"excludedBx : More than 1 ODIN bank  ("<<banks.size()<<")"<<endmsg;
    }
  } else {
    error()<<"excludedBx : "<<LHCb::RawEventLocation::Default<<" not found @"<<rootInTES()<<endmsg;
  }
  return false;
}

bool L0MuonMonitorBase::exclusiveBx() {

  if (m_exclusiveBxs.size()==0) return true;
  
  if (exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) {
    LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::ODIN );
    if (banks.size()==1){
      std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin();
      LHCb::ODIN odin;
      odin.set(*itBnk);
      unsigned int bunch =odin.bunchId() ;
      for (std::vector<unsigned int>::iterator itexcl=m_exclusiveBxs.begin(); itexcl!=m_exclusiveBxs.end();++itexcl){
        if (bunch==(*itexcl) ) return true;
      }
    } else {
      error()<<"exclusiveBx : More than 1 ODIN bank  ("<<banks.size()<<")"<<endmsg;
    }
  } else {
    error()<<"exclusiveBx : "<<LHCb::RawEventLocation::Default<<" not found @"<<rootInTES()<<endmsg;
  }
  return false;
}

bool L0MuonMonitorBase::selectedTrigger()
{
  
  //  if (m_triggerType.size()==0) return true;
  if (m_triggerType<0) return true;
  
  bool trig=false;
  if (exist<LHCb::L0DUReport>( LHCb::L0DUReportLocation::Default +context())) {
    const LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>( LHCb::L0DUReportLocation::Default+context() );
//     //    trig = l0->channelDecisionByName(m_triggerType);
//     trig = l0->channelDecisionByName("CaloCosmicsTrigger");
//     info()<<"selectedTrigger CaloCosmicsTrigger ? "<<trig<<endmsg;
//     trig = l0->channelDecisionByName("MuonCosmicsTrigger");
//     info()<<"selectedTrigger MuonCosmicsTrigger ? "<<trig<<endmsg;
    if (msgLevel( MSG::DEBUG )) {
      debug()<<"selectedTrigger O ? "<<l0->channelDecision(0)<<endmsg;
      trig = l0->channelDecision(1);
      debug()<<"selectedTrigger 1 ? "<<l0->channelDecision(1)<<endmsg;
    }
    
    trig = l0->channelDecision(m_triggerType);
  } else {
    error()<<"Can not access the L0DUReport @ "<<LHCb::L0DUReportLocation::Default+context()<<endmsg;
  }
  return trig;
}

