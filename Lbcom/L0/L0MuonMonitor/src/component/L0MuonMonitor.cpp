// $Id: L0MuonMonitor.cpp,v 1.4 2008-05-06 12:19:27 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"

// local
#include "L0MuonMonUtils.h"
#include "L0MuonMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitor
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitor::L0MuonMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
{
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitor::~L0MuonMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitor::initialize() {
  StatusCode sc = L0MuonMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Tools
  m_channelHist_l0muon = tool<PhysicalChannelsHist>( "PhysicalChannelsHist", "Channels_l0muon", this);
  m_channelHist_muon = tool<PhysicalChannelsHist>( "PhysicalChannelsHist", "Channels_muon", this);
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRawTool", this);
  m_rate = tool<InstantaneousRate>( "InstantaneousRate", "Rate", this);

  // Run info
  setHistoDir("L0Muon/Analysis");
  std::string name;
  name = "L0EventNumber";
  book1D(name,name,-0.5,-0.5+4096,4096);
  name = "L0_B_Id";
  book1D(name,name,-0.5,-0.5+3564,3564);

  // Rate
  m_rate->setHistoDir("L0Muon/Analysis");
  m_rate->bookHistos();

  // Cosmics
  name= "Cosmic_direction(dY(M5-M4))";
  book1D(name,name,-1.5,1.5,3);
  
  // Physical channels
  m_channelHist_l0muon->setHistoDir("L0Muon/Analysis");
  m_channelHist_muon->setHistoDir("L0Muon/Analysis");
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
      int reg = (*itr);
      for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
        int sta = (*its);
//         info()<<"booking histo for Q"<<(iq+1)<<" R"<<(reg+1)<<" M"<<(sta+1)<<endmsg;
        m_channelHist_l0muon->bookHistos(iq,reg,sta);
        m_channelHist_muon->bookHistos(iq,reg,sta);
      }
    }
  }

  setHistoDir("L0Muon/Analysis");
  // Multiplicity  
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string name;
    name="NPads_per_event_"+L0MuonMonUtils::stationName(sta);
    book1D(name,name,-0.5,10.5,11);
    name="NPads_per_bx_"+L0MuonMonUtils::stationName(sta);
    book2D(name,name,
           (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
           +0.5,10.5,10);
  }
 
  // Multiplicity  
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string name;
    name="Muon_NPads_per_event_"+L0MuonMonUtils::stationName(sta);
    book1D(name,name,-0.5,10.5,11);
    name="Muon_NPads_per_bx_"+L0MuonMonUtils::stationName(sta);
    book2D(name,name,
           (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
           +0.5,10.5,10);
  }
 
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitor::execute() {

  StatusCode sc;
  
  std::string location;
  std::string name;
  
  debug() << "==> Execute" << endmsg;

  setProperty("RootInTes",L0MuonMonUtils::timeSlot(0));
  if (excludedBx()) return StatusCode::SUCCESS;
  if (!exclusiveBx()) return StatusCode::SUCCESS;
  if (!selectedTrigger()) return StatusCode::SUCCESS;

  //Rate
  m_rate->fillHistos();
  
  int npad[L0MuonMonUtils::NStations]; for (int i=0; i<L0MuonMonUtils::NStations; ++i) npad[i]=0;
  int npad_muon[L0MuonMonUtils::NStations]; for (int i=0; i<L0MuonMonUtils::NStations; ++i) npad_muon[i]=0;

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    setProperty("RootInTes",L0MuonMonUtils::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    
    ulonglong event=0;
    unsigned int run=0 ;
    unsigned int bunch=0;
    if (exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) {
      LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
      const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::ODIN );
      if (banks.size()==1){
        std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin();
        LHCb::ODIN odin;
        odin.set(*itBnk);
        run   =odin.runNumber();
        event =odin.eventNumber() ;
        bunch =odin.bunchId() ;
      } else{
        error()<<"More than 1 ODIN bank  ("<<banks.size()<<")"<<endmsg;
      }
    }

    // Run info
    name = "L0EventNumber";
    fill(histo1D(name),event&0xFFF,1,name);
    name = "L0_B_Id";
    fill(histo1D(name),bunch,1,name);

    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    sc = getL0MuonTiles(l0muontiles);
    if (sc==StatusCode::FAILURE) {
      return StatusCode::SUCCESS;
    }
    
    // Get Muon Hits
    std::vector<LHCb::MuonTileID> muontiles;
    if (m_muonBuffer) { // If muon raw buffer tool
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      } else return Error( "Unable to locate IProperty interface of MuonRawBuffer" );

      m_muonBuffer->getTile(muontiles);
      m_muonBuffer->forceReset();
    }// End if muon raw buffer tool

    // Physical channels
    m_channelHist_l0muon->fillHistos(l0muontiles,*it_ts);
    m_channelHist_muon->fillHistos(muontiles,*it_ts);

    // Logical channels
    std::vector<LHCb::MuonTileID> l0muonpads;
    L0MuonMonUtils::makePads(l0muontiles,l0muonpads);
    std::vector<LHCb::MuonTileID> muonpads;
    L0MuonMonUtils::makePads(muontiles,muonpads);

//     // Cosmics
//     std::vector<LHCb::MuonTileID> padsM4;    
//     L0MuonMonUtils::filterTilesWithStation(l0muonpads,padsM4,L0MuonMonUtils::M4);
//     std::vector<LHCb::MuonTileID> padsM5;    
//     L0MuonMonUtils::filterTilesWithStation(l0muonpads,padsM5,L0MuonMonUtils::M5);
//     name= "Cosmic_direction(dY(M5-M4))";
//     for (std::vector<LHCb::MuonTileID>::iterator itM4=padsM4.begin(); itM4<padsM4.end(); ++itM4){
//       for (std::vector<LHCb::MuonTileID>::iterator itM5=padsM5.begin(); itM5<padsM5.end(); ++itM5){
//         if ( ((*itM4).region()==(*itM5).region()) && ( ((*itM5).nX()-(*itM4).nX())<=7) ) 
//           fill(histo1D(name),((*itM5).nY()-(*itM4).nY()),1);
//       }
//     }
    

    // Multiplicity per bx : L0Muon
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

    // Multiplicity per bx : Muon
    int np_muon[L0MuonMonUtils::NStations]; for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) np_muon[sta]=0;
    for (std::vector<LHCb::MuonTileID>::iterator itpad=muonpads.begin(); itpad<muonpads.end();++itpad){
      ++np_muon[itpad->station()];
    }
    for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) {
      name="Muon_NPads_per_bx_"+L0MuonMonUtils::stationName(sta);
      AIDA::IHistogram2D *hcand2 = histo2D(name);
      if (hcand2!=NULL) fill(hcand2,(*it_ts),np_muon[sta],1.);
    }
    for (int sta=0; sta<L0MuonMonUtils::NStations; ++sta) npad_muon[sta]+=np_muon[sta];

  } // End of loop over time slots

  // Multiplicity per events
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string hname="NPads_per_event_"+L0MuonMonUtils::stationName(sta);
    AIDA::IHistogram1D *hmulti=histo1D(hname);
    if (hmulti!=NULL) fill(hmulti,npad[sta],1.);
    std::string hname_muon="Muon_NPads_per_event_"+L0MuonMonUtils::stationName(sta);
    AIDA::IHistogram1D *hmulti_muon=histo1D(hname_muon);
    if (hmulti!=NULL) fill(hmulti_muon,npad_muon[sta],1.);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
