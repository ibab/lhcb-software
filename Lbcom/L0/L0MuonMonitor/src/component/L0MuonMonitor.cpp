// $Id: L0MuonMonitor.cpp,v 1.6 2008-06-18 12:23:13 jucogan Exp $
// Include files 

#include <math.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"
#include "Event/RecHeader.h"
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

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
  declareProperty( "MuonZS"  , m_muonZS = true);
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

//     info() <<"run= "<<run<<" event= "<<event<<" bunch= "<<bunch<<endmsg;

//     location = LHCb::L0MuonCtrlErrorLocation::Default ;
//     if (exist<LHCb::L0MuonCtrlErrors>(location) ){ //If container found in TES
//       LHCb::L0MuonCtrlErrors* perrors = get<LHCb::L0MuonCtrlErrors>(location);
//       LHCb::L0MuonCtrlErrors::iterator iterror;
//       for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over ctrl errors
//         int iq = (*iterror)->key();
//         int header=(*iterror)->header();
//         for (int i=1;i<3;++i){
//           if ( ((header>>i)&1) ) {
//             // error
//           }
//         }
//       }// End of loop over ctrl errors
//     } // End if container found in TES
    
//     location =  LHCb::L0MuonProcErrorLocation::Default ;
//     if (exist<LHCb::L0MuonProcErrors>(location) ){ // If container found in TES
//       LHCb::L0MuonProcErrors* perrors = get<LHCb::L0MuonProcErrors>(location);
//       LHCb::L0MuonProcErrors::iterator iterror;
//       for (iterror = perrors->begin() ; iterror!=perrors->end() ; ++iterror){ // Loop over proc errors
//         LHCb::MuonTileID mid = (*iterror)->key();
//         int iq = mid.quarter();
//         int ib = (*iterror)->board_index();
//         int header=(*iterror)->header();
//         for (int i=1;i<3;++i){
//           if ( ((header>>i)&1) ) {
//             // Error
//             return StatusCode::SUCCESS;
//           }
//         }
//       }// End of loop over proc errors
//     } // End if container found in TES

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
      if (m_muonZS) { // Look at ZS supressed bank
        m_muonBuffer->getTile(muontiles);
      } else { // Look at NonZS supressed bank
        std::vector<std::pair<LHCb::MuonTileID,unsigned int> >  tileAndTDC;
        m_muonBuffer->getNZSupp(tileAndTDC);
//         info()<<"Nb of muon hits : "<<tileAndTDC.size()<<endmsg;
        std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator itileAndTDC;
        for (itileAndTDC=tileAndTDC.begin(); itileAndTDC<tileAndTDC.end(); ++itileAndTDC) {
          muontiles.push_back(itileAndTDC->first);
        }
      } // End NonZS supressed bank
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

  std::vector<std::pair<LHCb::MuonTileID,double > >  tiles_and_deltaT;
  sc=compareTiles(tiles_and_deltaT);
  if( sc.isFailure() ) return Error( "Error comparing Muon and L0Muon hits ", sc );
  m_channelHist_l0muon->fillHistos(tiles_and_deltaT);
  
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

StatusCode L0MuonMonitor::compareTiles(std::vector<std::pair<LHCb::MuonTileID,double > > & tiles_and_deltaT) {

  static int ievt=0;
  if (exist<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default)) {
    LHCb::RecHeader* evt = get<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default);
    ievt = evt->evtNumber();
  } else {
    ++ievt;
  }
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
    
  StatusCode sc;
  
  std::vector<std::pair<LHCb::MuonTileID,double > > muontiles_and_time;
  std::vector<std::pair<LHCb::MuonTileID,double > > l0muontiles_and_time;

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    setProperty("RootInTes",L0MuonMonUtils::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    sc = getL0MuonTiles(l0muontiles);
    if (sc==StatusCode::FAILURE) {
      return StatusCode::SUCCESS;
    }
    std::vector<LHCb::MuonTileID >::iterator itl0muon;
    for(itl0muon=l0muontiles.begin();itl0muon<l0muontiles.end();++itl0muon){
      double time=(*it_ts);
      LHCb::MuonTileID mid=*itl0muon;
      std::pair<LHCb::MuonTileID,double> tdt = std::pair<LHCb::MuonTileID,double > (mid,time);
      l0muontiles_and_time.push_back(tdt);
    }
    
    // Get Muon Hits
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> > muontiles;
    if (m_muonBuffer) { // If muon raw buffer tool
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      } else return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
      if (m_muonZS) { // Look at ZS supressed bank
        m_muonBuffer->getTileAndTDC(muontiles);
      } else {// Look at NonZS supressed bank
        m_muonBuffer->getNZSupp(muontiles);
        //         info()<<"Nb of muon hits : "<<muontiles.size()<<endmsg;
      } // End NonZS supressed bank
      m_muonBuffer->forceReset();
    }// End if muon raw buffer tool
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator itmuon;
    //     info()<<"Nb of muon hits 2: "<<muontiles.size()<<endmsg;
    for(itmuon=muontiles.begin();itmuon<muontiles.end();++itmuon){
      double time=(*it_ts);
      LHCb::MuonTileID mid=itmuon->first;
      //       info()<<mid.toString()<<endmsg;
      if (!quarterInUse(mid.quarter())) continue;
      if (!stationInUse(mid.station())) continue;
      if (!regionInUse(mid.region())  ) continue;
      std::pair<LHCb::MuonTileID,double> tdt = std::pair<LHCb::MuonTileID,double >(mid,time);
      muontiles_and_time.push_back(tdt);
    }
  } // End of loop over time slots

  bool mismatch=false;

  std::vector<std::pair<LHCb::MuonTileID,double > >::iterator itmuon_and_time;
  std::vector<std::pair<LHCb::MuonTileID,double > >::iterator itl0muon_and_time;
  for (itl0muon_and_time=l0muontiles_and_time.begin();itl0muon_and_time<l0muontiles_and_time.end();++itl0muon_and_time){
    double deltaT=-15.;
    for (itmuon_and_time=muontiles_and_time.begin();itmuon_and_time<muontiles_and_time.end();++itmuon_and_time){
      if (itmuon_and_time->first==itl0muon_and_time->first) {
        double dT=itmuon_and_time->second-itl0muon_and_time->second;
        deltaT = fabs(deltaT)<fabs(dT) ? deltaT : dT;
      }
    }
    LHCb::MuonTileID mid=itl0muon_and_time->first;
    std::pair<LHCb::MuonTileID,double> tdt = std::pair<LHCb::MuonTileID,double> (mid,deltaT);
    tiles_and_deltaT.push_back(tdt);
    if (deltaT==-15) mismatch=true;
  }
  for (itmuon_and_time=muontiles_and_time.begin();itmuon_and_time<muontiles_and_time.end();++itmuon_and_time){
    bool found=false;
    for (itl0muon_and_time=l0muontiles_and_time.begin();itl0muon_and_time<l0muontiles_and_time.end();++itl0muon_and_time){
      if (itmuon_and_time->first==itl0muon_and_time->first) {
        found=true;
        break;
      }
    }
    if (!found) {
      double deltaT=+15.;
      LHCb::MuonTileID mid=itmuon_and_time->first;
      std::pair<LHCb::MuonTileID,double> tdt = std::pair<LHCb::MuonTileID,double> (mid,deltaT);
      tiles_and_deltaT.push_back(tdt);
      mismatch=true;
    }
  }


  if (msgLevel( MSG::INFO ) && mismatch) {
    // Candidates
    // Loop over time slots
    int ncand=0;
    for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
      setProperty("RootInTes",L0MuonMonUtils::timeSlot(*it_ts));
      if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
      std::string location = LHCb::L0MuonCandidateLocation::Default ;
      if (  exist<LHCb::L0MuonCandidates>(location ) ) {
        LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
        LHCb::L0MuonCandidates::const_iterator itcand;
        for ( itcand= cands->begin(); itcand<cands->end();++itcand) {
          LHCb::MuonTileID seed = (*itcand)->muonTileIDs(2)[0];
          if (!quarterInUse(seed.quarter())) continue;
          if (!regionInUse(seed.region())  ) continue;
          info()<<"Candidate found : seed= "<<seed.toString()<<" @ "<<(*it_ts)<<endmsg;
          ++ncand;
        }
      }
    } // End of loop over time slots
    
    
    info()<<endmsg;
    info()<<"MISMATCH !! evt# "<<ievt<<endmsg;
    info()<<endmsg;
    info()<<" Nb of candidates : "<<ncand<<endmsg;
    info()<<" Nb of hits : L0Muon= "<<l0muontiles_and_time.size()
          <<" Muon= "<<muontiles_and_time.size()
          <<"\n"<<endmsg;
    for (itl0muon_and_time=l0muontiles_and_time.begin();itl0muon_and_time<l0muontiles_and_time.end();++itl0muon_and_time){
      double deltaT=-15.;
      for (itmuon_and_time=muontiles_and_time.begin();itmuon_and_time<muontiles_and_time.end();++itmuon_and_time){
        if (itmuon_and_time->first==itl0muon_and_time->first) {
          double dT=itmuon_and_time->second-itl0muon_and_time->second;
          deltaT = fabs(deltaT)<fabs(dT) ? deltaT : dT;
        }
      }
      LHCb::MuonTileID mid=itl0muon_and_time->first;
      std::pair<LHCb::MuonTileID,double> tdt = std::pair<LHCb::MuonTileID,double> (mid,deltaT);
      info()<<"(mismatch) L0Muon : "<<(itl0muon_and_time->first).toString()<<" @ "<<itl0muon_and_time->second;
      if (deltaT==-15) {
        info()<<" NOT FOUND IN MUON";
      } else {
        info()<<" found in muon with dT= "<<deltaT;
      }
      info()<<" evt "<<ievt<<" bunchID"<<std::hex<<bunch<<std::dec<<endmsg;
    }
    for (itmuon_and_time=muontiles_and_time.begin();itmuon_and_time<muontiles_and_time.end();++itmuon_and_time){
    bool found=false;
    for (itl0muon_and_time=l0muontiles_and_time.begin();itl0muon_and_time<l0muontiles_and_time.end();++itl0muon_and_time){
      if (itmuon_and_time->first==itl0muon_and_time->first) {
        found=true;
        break;
      }
    }
    if (!found) {
      info()<<"mismatch) Muon   : "<<(itmuon_and_time->first).toString()<<" @ "<<itmuon_and_time->second;
      info()<<" NOT FOUND IN L0MUON";
      info()<<" evt "<<ievt<<" bunchID"<<std::hex<<bunch<<std::dec<<endmsg;
    }
    //info()<<"Muon   : "<<(itmuon_and_time->first).toString()<<" @ "<<itmuon_and_time->second<<endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
  
}
