// $Id: L0MuonMonitorBase.cpp,v 1.10 2010-03-08 15:00:34 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/IRndmGenSvc.h"


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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitorBase::L0MuonMonitorBase( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_h_online(0)
{
  std::vector<int> time_slots;
  //for (int i=-7;i<=7;++i) time_slots.push_back(i);
  time_slots.push_back(0);
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

  declareProperty( "Prescale" , m_prescale = 1.);
  
  declareProperty( "Online"  , m_online = true);
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

  sc = m_rnd.initialize(randSvc(), Rndm::Flat(0.,1.));
  if ( !sc.isSuccess() ) {
    return sc;
  }
  
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

void L0MuonMonitorBase::activeTS(std::vector<int> & active_slots)
{
  // get list of active time slots in TAE
  active_slots.clear();
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    active_slots.push_back(*it_ts);
  }
}

StatusCode L0MuonMonitorBase::getOlsInError(std::vector<LHCb::MuonTileID> & ols)
{
  StatusCode sc=StatusCode::SUCCESS;
  
  std::string location;
  location=LHCb::L0MuonErrorLocation::ProcPU+context();

  if (exist<LHCb::L0MuonErrors> (location)) {
    LHCb::L0MuonErrors *perrors = get<LHCb::L0MuonErrors> (location);
    LHCb::L0MuonErrors::const_iterator iterr;
    for (iterr=perrors->begin(); iterr!=perrors->end(); ++iterr){ // Loop over L0MuonError in container
      const LHCb::L0MuonError *err = (*iterr);
      LHCb::MuonTileID pu=err->key();
      int qua = pu.quarter();
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
          olid.setQuarter(qua);
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

StatusCode L0MuonMonitorBase::storeOlsInError() {
  
  StatusCode sc=StatusCode::SUCCESS;

  m_err_ols.clear();
  
  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    sc=setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
    
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    // Get optical links in error
    std::vector<LHCb::MuonTileID> ol_in_error;
    sc = getOlsInError(ol_in_error);
    if (sc==StatusCode::FAILURE) {
      return StatusCode::SUCCESS;
    }
    for (std::vector<LHCb::MuonTileID>::iterator itol=ol_in_error.begin(); itol<ol_in_error.end(); ++itol){
      std::pair<LHCb::MuonTileID,int> ol_ts=std::pair<LHCb::MuonTileID,int> (*itol,*it_ts);
      m_err_ols[ol_ts]=true;
      m_err_ols_stored[*itol]+=1;
    }

  } // End of loop over time slots
  
  return sc;
  
}

void L0MuonMonitorBase::olsErrorSummary(MsgStream & msg) const{

  msg<<"--- L0Muon input optical links error summary : "<<m_err_ols_stored.size()<<" errors detected"<<endmsg;
  std::map<LHCb::MuonTileID,int>::const_iterator itol;
  for (itol=m_err_ols_stored.begin(); itol!=m_err_ols_stored.end(); ++itol) {
    LHCb::MuonTileID ol=itol->first;
    int nerrors=itol->second;
    LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
    int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
    LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
    int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
    
    msg<<"-- "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
       <<" PB"<<ipb<<" PU"<<ipu
       <<"\t ol= "<<ol.toString()
       <<"\t : "<<nerrors<<" errors"<<endmsg;
  }
  msg<<"---"<<endmsg;

}

void L0MuonMonitorBase::olsErrorSummary(std::ofstream &fout) const{

  fout<<"--- L0Muon input optical links error summary : "<<m_err_ols_stored.size()<<" errors detected"<<"\n";
  std::map<LHCb::MuonTileID,int>::const_iterator itol;
  for (itol=m_err_ols_stored.begin(); itol!=m_err_ols_stored.end(); ++itol) {
    LHCb::MuonTileID ol=itol->first;
    int nerrors=itol->second;
    LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
    int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
    LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
    int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
    
    fout<<"-- "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
       <<" PB"<<ipb<<" PU"<<ipu
       <<"\t ol= "<<ol.toString()
       <<"\t : "<<nerrors<<" errors"<<"\n";
  }
  fout<<"---"<<"\n";

}

StatusCode L0MuonMonitorBase::getL0MuonTiles(std::vector<LHCb::MuonTileID> & l0muontiles)
{

  StatusCode sc=StatusCode::SUCCESS;

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
            info()<<"PU "<<mkey.toString()<<" tile is not valid : "<<itol->toString()<<endmsg;
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
  return sc;
}

StatusCode L0MuonMonitorBase::getL0MuonPads(std::vector<LHCb::MuonTileID> & l0muonpads)
{

  l0muonpads.clear();

  StatusCode sc=StatusCode::SUCCESS;

  std::string location = LHCb::L0MuonDataLocation::Default +context();
  if (  exist<LHCb::L0MuonDatas>(location ) ) 
  {
    LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( location );
    LHCb::L0MuonDatas::const_iterator itdata;
    for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
      LHCb::MuonTileID mkey = (*itdata)->key();    
      std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
      if (ols.size()>0) {
        std::vector<LHCb::MuonTileID> tiles;
        for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
          if (!itol->isValid()){
            info()<<"PU "<<mkey.toString()<<" tile is not valid : "<<itol->toString()<<endmsg;
          }

          if (!quarterInUse(itol->quarter())) continue;
          if (!regionInUse(itol->region())  ) continue;
          if (!stationInUse(itol->station())) continue;
            
          tiles.push_back(*itol);
        }
        std::vector<LHCb::MuonTileID> pads;
        L0Muon::MonUtilities::makePads(tiles,pads);
        debug()<<"PU "<<mkey.toString()<<" # of tiles "<<tiles.size()<<"# of pads "<<pads.size()<<endmsg;
        for (std::vector<LHCb::MuonTileID>::iterator itpads=pads.begin(); itpads<pads.end(); ++itpads) {
          l0muonpads.push_back(*itpads);
        }
        //l0muonpads.insert(l0muonpads.end(),pads.begin(),pads.end());
      }
    }
  } 
  else 
  {
    std::stringstream errstr("");
    errstr<<"getL0muonPads : L0MuonDatas not found @"<<rootInTES()<<"/"<<LHCb::L0MuonDataLocation::Default+context();
    return Warning(errstr.str(),StatusCode::FAILURE,5);
  }
  return sc;
}

StatusCode L0MuonMonitorBase::getL0MuonPadsTAE(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles_and_time)
{
  StatusCode sc=StatusCode::SUCCESS;
  // Loop over time slots (for l0muon hits)
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    
    sc=setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
    
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    
    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    sc = getL0MuonTiles(l0muontiles);
    if ( sc==StatusCode::FAILURE ) {
      return Error("L0muon tiles not found",StatusCode::SUCCESS,50);
    }
    
    // Build logical channels 
    std::vector<LHCb::MuonTileID> l0muonpads;
    L0Muon::MonUtilities::makePads(l0muontiles,l0muonpads);
    
    std::vector<LHCb::MuonTileID >::iterator itl0muon;
    for(itl0muon=l0muonpads.begin();itl0muon<l0muonpads.end();++itl0muon){
      LHCb::MuonTileID mid=*itl0muon;
      std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int > (mid,*it_ts);
      l0muontiles_and_time.push_back(tdt);
    }
    
  } // End of loop over time slots (for l0muon hits)
  
  return sc;
}

StatusCode L0MuonMonitorBase::getL0MuonTilesTAE(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles_and_time)
{
  
  StatusCode sc=StatusCode::SUCCESS;
  // Loop over time slots (for l0muon hits)
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    sc=setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
    
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    sc = getL0MuonTiles(l0muontiles);
    if ( sc==StatusCode::FAILURE ) {
      return Error("L0muon tiles not found",StatusCode::SUCCESS,50);
    }
    
    std::vector<LHCb::MuonTileID >::iterator itl0muon;
    for(itl0muon=l0muontiles.begin();itl0muon<l0muontiles.end();++itl0muon){
      LHCb::MuonTileID mid=*itl0muon;
      std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int > (mid,*it_ts);
      l0muontiles_and_time.push_back(tdt);
    }
    
  } // End of loop over time slots (for l0muon hits)

  return sc;
  
}


bool L0MuonMonitorBase::excludedBx() {

  if (m_excludedBxs.size()==0) return false;

  try {
    LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

    unsigned int bunch = odin->bunchId();
    for (std::vector<unsigned int>::iterator itexcluded=m_excludedBxs.begin(); itexcluded!=m_excludedBxs.end();++itexcluded){
      if (bunch==(*itexcluded) ) return true;
    }
  } catch(GaudiException &x) {
    error() << "excludedBx : " << x << endmsg;
  }
  return false;
}

bool L0MuonMonitorBase::exclusiveBx() {

  if (m_exclusiveBxs.size()==0) return true;
  
  try {
    LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
    
    unsigned int bunch = odin->bunchId();
    for (std::vector<unsigned int>::iterator itexcl=m_exclusiveBxs.begin(); itexcl!=m_exclusiveBxs.end();++itexcl){
      if (bunch==(*itexcl) ) return true;
    }
  } catch(GaudiException &x) {
    error() << "exclusiveBx : " << x << endmsg;
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
