// Include files 

#include "boost/format.hpp"

// from Event
#include "Event/RawEvent.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonInfo.h"


// local
#include "L0MuonOnlineMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOnlineMonitor
//
// 2008-04-02 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonOnlineMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOnlineMonitor::L0MuonOnlineMonitor( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
  , m_padsHistos(0)
  , m_channelsHistos(0)
  , m_info(0)
  , m_error(0)
  , m_candHistosFinal(0)
  , m_candHistosPU(0)
{
  declareProperty( "FullMonitoring"  , m_fullMonitoring  = false ) ;
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
  m_candHistosFinal = tool<L0MuonCandHistos>( "L0MuonCandHistos", "FinalCand", this);
  m_candHistosPU    = tool<L0MuonCandHistos>( "L0MuonCandHistos", "PUCand", this);
  if (m_fullMonitoring) {
    m_padsHistos      = tool<L0MuonPadsHistos>( "L0MuonPadsHistos" , "Pads" , this );
    m_channelsHistos  = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "Channels", this);
    m_info            = tool<L0MuonInfoHistos>( "L0MuonInfoHistos", "Info", this);
    m_error           = tool<L0MuonErrorHistos>( "L0MuonErrorHistos", "Error", this);
  }
  
  if (!m_shortnames) setHistoDir("L0Muon/Online");

  // Candidates
  if (!m_shortnames) m_candHistosFinal->setHistoDir("L0Muon/Online");
  m_candHistosFinal->bookHistos(8,m_shortnames);
  if (!m_shortnames) m_candHistosPU->setHistoDir("L0Muon/Online/PU");
  m_candHistosPU->bookHistos(100,m_shortnames);
  debug() << "==>   -- Candidates done" << endmsg;

  if (m_fullMonitoring) { // If full monitoring
    // Run info
    if (!m_shortnames) m_info->setHistoDir("L0Muon/Online");
    m_info->bookHistos(m_shortnames);
    debug() << "==>   -- RunInfo done" << endmsg;

    // Decoding
    if (!m_shortnames) m_error->setHistoDir("L0Muon/Online");
    m_error->bookHistos_multi(m_shortnames);
    m_error->bookHistos_gen(m_shortnames);
    for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
      int iq = (*itq);
      m_error->bookHistos_quarter(iq,m_shortnames);
      for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
        int reg = (*itr);
        m_error->bookHistos_board(iq,reg,m_shortnames);
      }
    }
    debug() << "==>   -- Decoding done" << endmsg;

    // Logical channels
    if (!m_shortnames) m_channelsHistos->setHistoDir("L0Muon/Online");
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
    if (!m_shortnames) m_padsHistos->setHistoDir("L0Muon/Online");
    for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
      int sta = (*its);
      m_padsHistos->bookHistos(sta,m_shortnames);
    }
    debug() << "==>   -- Logical Pads done" << endmsg;

  } // End if full monitoring
  
    
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonOnlineMonitor::execute() {
  
  StatusCode sc;
  sc = setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(0));
  if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);

  if (excludedBx()) return StatusCode::SUCCESS;
  if (!exclusiveBx()) return StatusCode::SUCCESS;
  if (!selectedTrigger()) return StatusCode::SUCCESS;

  int ncand=0;
  int ncandPU=0;
  int npad[L0Muon::MonUtilities::NStations]; for (int i=0; i<L0Muon::MonUtilities::NStations; ++i) npad[i]=0;
  
  int bid_ts0=-1;

  if (m_fullMonitoring) { // If full monitoring
    //Run info
    sc = m_info->setProperty( "RootInTES", rootInTES() );
    if ( sc==StatusCode::SUCCESS ) {
      sc = m_info->getInfo();
      if ( sc==StatusCode::SUCCESS ) {
        m_info->fillHistos();
        bid_ts0 = m_info->bunchId();
      }
    }
  }
  
  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    sc=setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
    
    if (!exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) continue;
    
    std::string location;

    
    int bid=-1;
    if (bid_ts0>-1) bid=bid_ts0+(*it_ts);

    if (m_fullMonitoring) { // If full monitoring
    
    // Error
      sc = m_error->setProperty( "RootInTES", rootInTES() );
      if ( sc==StatusCode::SUCCESS ) {
        m_error->fillHistos();
      }
    
      // Get L0Muon Tiles
      std::vector<LHCb::MuonTileID> l0muontiles;
      sc = getL0MuonTiles(l0muontiles);
      if (sc==StatusCode::SUCCESS) {
        // Logical channels histos
        m_channelsHistos->fillHistos(l0muontiles,*it_ts);
//         std::vector<LHCb::MuonTileID> l0muonpads_2;
//         L0Muon::MonUtilities::makePads(l0muontiles,l0muonpads_2);
//         // Logical pads histos
//         m_padsHistos->fillHistos(l0muonpads_2,*it_ts);
      }
   
      // Build logical channels 
      std::vector<LHCb::MuonTileID> l0muonpads;
      sc = getL0MuonPads(l0muonpads);
      if (sc==StatusCode::SUCCESS) {
        // Logical channels histos
        m_padsHistos->fillHistos(l0muonpads,*it_ts);
        // Multiplicity
        for (std::vector<LHCb::MuonTileID>::iterator itpad=l0muonpads.begin(); itpad<l0muonpads.end();++itpad){
          ++npad[itpad->station()];
        }
      }
      
    } // End if full monitoring

    // Candidates
    location = LHCb::L0MuonCandidateLocation::Default + context();
    if (  exist<LHCb::L0MuonCandidates>(location ) ) {
      LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
      m_candHistosFinal->fillHistos(cands,(*it_ts),bid);
      ncand+=cands->size();
    }

    location = LHCb::L0MuonCandidateLocation::PU + context();
    if (  exist<LHCb::L0MuonCandidates>(location ) ) {
      LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
      m_candHistosPU->fillHistos(cands,(*it_ts),bid);
      ncandPU+=cands->size();
    }
    
  } // End of loop over time slots

  if (m_fullMonitoring) {
    // Multiplicity
    m_padsHistos->fillHistos(npad);
  }

  // Candidates
  m_candHistosFinal->fillHistos(ncand,bid_ts0);
  m_candHistosPU->fillHistos(ncandPU,bid_ts0);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonOnlineMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

