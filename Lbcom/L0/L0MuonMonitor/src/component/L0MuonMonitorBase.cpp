// $Id: L0MuonMonitorBase.cpp,v 1.5 2008-09-15 07:46:40 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 


#include "Kernel/MuonLayout.h"
#include "Kernel/MuonTileID.h"

#include "Event/L0DUReport.h"
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonError.h"

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

  setLayouts();
  
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

StatusCode L0MuonMonitorBase::getOlsInError(std::vector<LHCb::MuonTileID> & ols)
{
  StatusCode sc=StatusCode::SUCCESS;
  
  ols.clear();
  std::string location;
  location=LHCb::L0MuonErrorLocation::ProcPU+context();
  

  if (exist<LHCb::L0MuonErrors> (location)) {
    LHCb::L0MuonErrors *perrors = get<LHCb::L0MuonErrors> (location);
    LHCb::L0MuonErrors::const_iterator iterr;
    for (iterr=perrors->begin(); iterr!=perrors->end(); ++iterr){ // Loop over L0MuonError in container
      const LHCb::L0MuonError *err = (*iterr);
      LHCb::MuonTileID pu=err->key();
      int reg = pu.region();
      unsigned int optlinkError = ( (err->hardware()>>16) &0xFF);
      for (int ib =0; ib<8; ++ib) { // Loop over opt link error bits
        if ( (optlinkError>>ib)&0x1 ) { // If error bit is ON
          LHCb::MuonTileID olid;
          int sta=ib/2;
          if (ib==7) sta=4;
          MuonLayout layOL = m_opt_link_layout.stationLayout(sta).regionLayout(reg);
          pu.setStation(sta);
          std::vector<LHCb::MuonTileID> ols_in_pu=layOL.tiles(pu);
          if (ols_in_pu.size()==1) 
          { 
            olid=ols_in_pu[0];
          } 
          else if (ols_in_pu.size()==2)
          {
            if (sta==0) { // M1 : 2 optical links per PU
              olid=ols_in_pu[ib%2];
            } else if (sta==1 || sta==2) {
              if (reg==0) { // M2-M3 R1 : the layout gives 2 OLs per PU only one was received by this PU                
                if ( (pu.nY()%2)==1 ) {
                  olid=ols_in_pu[0];
                } else {
                  olid=ols_in_pu[1];
                }
              } else if (reg==1) { // M2-M3 R2 : 2 optical links per PU
                olid=ols_in_pu[ib%2];
              } else {
                return Error("Wrong number of optical links M2 M3",StatusCode::FAILURE,50);
              }
            } else {
              return Error("Wrong number of optical links M4 M5",StatusCode::FAILURE,50);
            }
          }
          else
          {
            return Error("Wrong number of optical links >2",StatusCode::FAILURE,50);
          }
          olid.setStation(sta);
          ols.push_back(olid);
          if( msgLevel(MSG::VERBOSE) ) {
            int quarter,iboard,ipu;
            err->index(quarter,iboard,ipu);
            verbose()<<"Error on "<<pu.toString()
                     <<" Q"<<(quarter+1)<<" R"<<(reg+1)<<" M"<<(sta+1)<<" PB"<<iboard<<" PU"<<ipu
                     <<" OL "<<olid.toString()
                     <<endmsg;
          }
        } // End if error bit is ON
      } // End of loop over opt link error bits
    } // End of loop over L0MuonError in container
  } else {
    return Error("L0MuonErrors not found at "+location,StatusCode::FAILURE,50);
  }
  
  return sc;
  

}
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

void L0MuonMonitorBase::setLayouts()
{



  m_opt_link_layout = MuonSystemLayout(MuonStationLayout(MuonLayout(2,4)),
                                       MuonStationLayout(MuonLayout(4,1),
                                                         MuonLayout(4,2),
                                                         MuonLayout(2,2),
                                                         MuonLayout(2,2)),
                                       MuonStationLayout(MuonLayout(4,1),
                                                         MuonLayout(4,2),
                                                         MuonLayout(2,2),
                                                         MuonLayout(2,2)),
                                       MuonStationLayout(MuonLayout(2,2)),
                                       MuonStationLayout(MuonLayout(2,2)));
  
  

}
