// $Id: L0MuonOnlineMonitor.cpp,v 1.6 2008-07-24 09:36:53 jucogan Exp $
// Include files 

#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/ODIN.h"
#include "Event/RawEvent.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

#include "L0MuonKernel/MonUtilities.h"

// local
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
  m_padsHistos     = tool<L0MuonPadsHistos>( "L0MuonPadsHistos" , "Pads" , this );
  m_channelsHistos = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "Channels", this);
  m_info           = tool<L0MuonInfoHistos>( "L0MuonInfoHistos", "Info", this);
  m_error          = tool<L0MuonErrorHistos>( "L0MuonErrorHistos", "Error", this);
  
  setHistoDir("L0Muon/Online");

  // Run info
  m_info->setHistoDir("L0Muon/Online");
  m_info->bookHistos(m_shortnames);
  debug() << "==>   -- RunInfo done" << endmsg;

  // Decoding
#if _ERROR_V2_==1
  m_error->setHistoDir("L0Muon/Online");
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    m_error->bookHistos_ctrl(iq,m_shortnames);
    for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
      int reg = (*itr);
      m_error->bookHistos_proc(iq,reg,m_shortnames);
    }
  }
#else
  // ---- TMP ---- WILL NOT BE USED WITH BANK V2
  for (int iq=0; iq<4;++iq){
    for (int i=0; i<3; ++i) m_decoding_status[iq][i]=NULL;
  }
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    std::string hname;
    int iq = (*itq);
    hname="Decoding_errors_"+L0Muon::MonUtilities::quarterName(iq);
    m_decoding_status[iq][2]=book1D(hname,hname,-0.5,-0.5+16,16);
    hname="Inconsistencies_"+L0Muon::MonUtilities::quarterName(iq);
    m_decoding_status[iq][1]=book1D(hname,hname,-0.5,-0.5+16,16);
    hname="Valid_"+L0Muon::MonUtilities::quarterName(iq);
    m_decoding_status[iq][0]=book1D(hname,hname,-0.5,-0.5+16,16);
  }
  // ---- END TMP ---- WILL NOT BE USED WITH BANK V2
#endif
  debug() << "==>   -- Decoding done" << endmsg;

  // Logical channels
  m_channelsHistos->setHistoDir("L0Muon/Online");
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
      int reg = (*itr);
      for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
        int sta = (*its);
        m_channelsHistos->bookHistos(iq,reg,sta,m_shortnames);
      }
    }
  }
  debug() << "==>   -- Logical Channels done" << endmsg;
  
  // Logical pads
  m_padsHistos->setHistoDir("L0Muon/Online");
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    m_padsHistos->bookHistos(sta,m_shortnames);
  }
  debug() << "==>   -- Logical Pads done" << endmsg;

  // Multiplicity  
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    std::string hname;
    int sta = (*its);
    hname="NPads_per_event_"+L0Muon::MonUtilities::stationName(sta);
    book1D(hname,hname,-0.5,10.5,11);
    hname="NPads_per_bx_"+L0Muon::MonUtilities::stationName(sta);
    book2D(hname,hname,
           (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
           +0.5,10.5,10);
  }
  debug() << "==>   -- Multiplicity done" << endmsg;
  

  // Candidates
  book1D("NCands_per_event","NCands_per_event",-0.5,0.5+2*m_quarters.size(),1+2*m_quarters.size());
  book2D("NCands_per_bx","NCands_per_bx",
         (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
         +0.5,0.5+2*m_quarters.size(),2*m_quarters.size());
  debug() << "==>   -- Candidates done" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonOnlineMonitor::execute() {
  StatusCode sc;
  
  debug() << "==> Execute" << endmsg;

  setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(0));
  if (excludedBx()) return StatusCode::SUCCESS;
  if (!exclusiveBx()) return StatusCode::SUCCESS;
  if (!selectedTrigger()) return StatusCode::SUCCESS;

  int ncand=0;
  int npad[L0Muon::MonUtilities::NStations]; for (int i=0; i<L0Muon::MonUtilities::NStations; ++i) npad[i]=0;

  //Run info
  m_info->getInfo();
  m_info->fillHistos();

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) continue;
    
    std::string location;

#if _ERROR_V2_==1
    m_error->fillHistos();
#else
    // ---- TMP ---- WILL NOT BE USED WITH BANK V2
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
    // ---- END TMP ---- WILL NOT BE USED WITH BANK V2
#endif

    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    sc = getL0MuonTiles(l0muontiles);
    if (sc==StatusCode::FAILURE) continue;

    // Build logical channels 
    std::vector<LHCb::MuonTileID> l0muonpads;
    L0Muon::MonUtilities::makePads(l0muontiles,l0muonpads);

    // Physical channels histos
    m_channelsHistos->fillHistos(l0muontiles,*it_ts);

    // Logical channels histos
    m_padsHistos->fillHistos(l0muonpads,*it_ts);

    // Multiplicity
    int np[L0Muon::MonUtilities::NStations]; for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) np[sta]=0;
    for (std::vector<LHCb::MuonTileID>::iterator itpad=l0muonpads.begin(); itpad<l0muonpads.end();++itpad){
      ++np[itpad->station()];
    }
    for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) {
      std::string hname;
      hname="NPads_per_bx_"+L0Muon::MonUtilities::stationName(sta);
      AIDA::IHistogram2D *hcand2 = histo2D(hname);
      if (hcand2!=NULL) fill(hcand2,(*it_ts),np[sta],1.);
    }
    for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) npad[sta]+=np[sta];

    // Candidates
    location = LHCb::L0MuonCandidateLocation::Default + context();
    if (  exist<LHCb::L0MuonCandidates>(location ) ) {
      LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
      LHCb::L0MuonCandidates::const_iterator itcand;
      int n=0;
      for ( itcand= cands->begin(); itcand<cands->end();++itcand) {
        LHCb::MuonTileID seed = (*itcand)->muonTileIDs(2)[0];
        if (!quarterInUse(seed.quarter())) continue;
        if (!regionInUse(seed.region())  ) continue;
        ++n;
      }
      
      AIDA::IHistogram2D *hcand2 = histo2D(std::string("NCands_per_bx"));
      if (hcand2!=NULL) fill(hcand2,(*it_ts),n,1.);
      ncand+=n;
    }

    
  } // End of loop over time slots

  // Multiplicity
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string hname="NPads_per_event_"+L0Muon::MonUtilities::stationName(sta);
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

