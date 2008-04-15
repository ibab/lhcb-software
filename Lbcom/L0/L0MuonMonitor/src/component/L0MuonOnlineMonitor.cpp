// $Id: L0MuonOnlineMonitor.cpp,v 1.3 2008-04-15 09:47:37 jucogan Exp $
// Include files 

#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

// local
#include "L0MuonMonUtils.h"
#include "L0MuonOnlineMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOnlineMonitor
//
// 2008-04-02 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonOnlineMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOnlineMonitor::L0MuonOnlineMonitor( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonOnlineMonitor::~L0MuonOnlineMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonOnlineMonitor::initialize() {
  StatusCode sc = L0MuonMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg


  debug() << "==> Initialize" << endmsg;

  // Tools
  m_padsMaps =  tool<LogicalPads2DMaps>( "LogicalPads2DMaps" , "Pads" , this );
  m_channelHist = tool<PhysicalChannelsHist>( "PhysicalChannelsHist", "Channels", this);
  
  setHistoDir("L0Muon/Online");

  // Decoding
  std::string name;
  for (int iq=0; iq<4;++iq){
    for (int i=0; i<3; ++i) m_decoding_status[iq][i]=NULL;
  }
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    name="Decoding_errors_"+L0MuonMonUtils::quarterName(iq);
    m_decoding_status[iq][2]=book1D(name,name,-0.5,-0.5+16,16);
    name="Inconsistencies_"+L0MuonMonUtils::quarterName(iq);
    m_decoding_status[iq][1]=book1D(name,name,-0.5,-0.5+16,16);
    name="Valid_"+L0MuonMonUtils::quarterName(iq);
    m_decoding_status[iq][0]=book1D(name,name,-0.5,-0.5+16,16);
  }
  
  // Physical channels
  m_channelHist->setHistoDir("L0Muon/Online");
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
      int reg = (*itr);
      for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
        int sta = (*its);
        m_channelHist->bookHistos(iq,reg,sta);
      }
    }
  }
  
  // Logical channels
  m_padsMaps->setHistoDir("L0Muon/Online");
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    m_padsMaps->bookHistos(sta);
  }

  // Multiplicity  
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    name="NPads_per_event_"+L0MuonMonUtils::stationName(sta);
    book1D(name,name,-0.5,10.5,11);
    name="NPads_per_bx_"+L0MuonMonUtils::stationName(sta);
    book2D(name,name,
           (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
           +0.5,10.5,10);
  }
  

  // Candidates
  book1D("NCands_per_event","NCands_per_event",-0.5,0.5+2*m_quarters.size(),1+2*m_quarters.size());
  book2D("NCands_per_bx","NCands_per_bx",
         (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
         +0.5,0.5+2*m_quarters.size(),2*m_quarters.size());
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonOnlineMonitor::execute() {
  StatusCode sc;
  
  debug() << "==> Execute" << endmsg;

  int ncand=0;
  int npad[L0MuonMonUtils::NStations]; for (int i=0; i<L0MuonMonUtils::NStations; ++i) npad[i]=0;

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    setProperty("RootInTes",L0MuonMonUtils::timeSlot(*it_ts));
    //     if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    
    std::string location;
    std::string name;
    
    location = LHCb::L0MuonCtrlErrorLocation::Default ;
    if (exist<LHCb::L0MuonCtrlErrors>(location) ){ //If container found in TES
      LHCb::L0MuonCtrlErrors* perrors = get<LHCb::L0MuonCtrlErrors>(location);
      LHCb::L0MuonCtrlErrors::iterator iterror;
      for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over ctrl errors
        int iq = (*iterror)->key();
        int header=(*iterror)->header();
        for (int i=0;i<3;++i){
          if ( ((header>>i)&1) ) {
            fill( m_decoding_status[iq][i] , 15.  , 1.);
          }
        }
      }// End of loop over ctrl errors
    } // End if container found in TES
    
    location =  LHCb::L0MuonProcErrorLocation::Default ;
    if (exist<LHCb::L0MuonProcErrors>(location) ){ // If container found in TES
      LHCb::L0MuonProcErrors* perrors = get<LHCb::L0MuonProcErrors>(location);
      LHCb::L0MuonProcErrors::iterator iterror;
      for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over proc errors
        LHCb::MuonTileID mid = (*iterror)->key();
        int iq = mid.quarter();
        int ib = (*iterror)->board_index();
        int header=(*iterror)->header();
        for (int i=0;i<3;++i){
          if ( ((header>>i)&1) ) {
            if (m_decoding_status[iq][i]!=NULL) fill( m_decoding_status[iq][i] , ib  , 1.);
          }
          
        }
      }// End of loop over proc errors
    } // End if container found in TES
    
    
    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    location = LHCb::L0MuonDataLocation::Default ;
    if (  exist<LHCb::L0MuonDatas>(location ) ) {
      LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( location );
      LHCb::L0MuonDatas::const_iterator itdata;
      for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
        LHCb::MuonTileID mkey = (*itdata)->key();    
        std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
        if (ols.size()>0) {
          for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
            if (!itol->isValid()){
              info()<<"PU "<<mkey.toString()<<" tile is not valid"<<endreq;
            }

            if (!quarterInUse(itol->quarter())) continue;
            if (!stationInUse(itol->station())) continue;
            if (!regionInUse(itol->region())  ) continue;
            
            l0muontiles.push_back(*itol);
          }
        }
      }
    } // End if container found in TES
    

    // Build logical channels 
    std::vector<LHCb::MuonTileID> l0muonpads;
    L0MuonMonUtils::makePads(l0muontiles,l0muonpads);

    // Physical channels histos
    m_channelHist->fillHistos(l0muontiles);

    // Logical channels histos
    m_padsMaps->fillHistos(l0muonpads);

    // Multiplicity
    int np[L0MuonMonUtils::NStations]; for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) np[sta]=0;
    for (std::vector<LHCb::MuonTileID>::iterator itpad=l0muonpads.begin(); itpad<l0muonpads.end();++itpad){
      ++np[itpad->station()];
    }
    for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) {
      name="NPads_per_bx_"+L0MuonMonUtils::stationName(sta);
      AIDA::IHistogram2D *hcand2 = histo2D(name);
      if (hcand2!=NULL) fill(hcand2,(*it_ts),np[sta],1.);
    }
    for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) npad[sta]+=np[sta];


    // Candidates
    location = LHCb::L0MuonCandidateLocation::Default ;
    if (  exist<LHCb::L0MuonCandidates>(location ) ) {
      LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
      AIDA::IHistogram2D *hcand2 = histo2D(std::string("NCands_per_bx"));
      if (hcand2!=NULL) fill(hcand2,(*it_ts),cands->size(),1.);
      ncand+=cands->size();
    }

    
  } // End of loop over time slots

  // Multiplicity
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string hname="NPads_per_event_"+L0MuonMonUtils::stationName(sta);
    AIDA::IHistogram1D *hmulti=histo1D(hname);
    if (hmulti!=NULL) fill(hmulti,npad[sta],1.);
  }
  
  // Candidates
  AIDA::IHistogram1D *hcand=histo1D(std::string("NCands_per_event"));
  if (hcand!=NULL) fill(hcand,ncand,1.);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonOnlineMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

