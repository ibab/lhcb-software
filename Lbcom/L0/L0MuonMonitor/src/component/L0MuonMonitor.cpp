// Include files 

#include <math.h>

#if _USE_MUONMONREC_TOOL_
#include "MuonMonKernel/IMuonMonRec.h"
#include "MuonMonKernel/MuonMonHit.h"
#include "MuonMonKernel/MuonMonPad.h"
#endif

#include "Kernel/MuonLayout.h"

#include "Event/RawEvent.h"
#include "Event/RecHeader.h"
#include "Event/ODIN.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

#include "L0MuonKernel/MonUtilities.h"

// local
#include "L0MuonMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitor
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitor::L0MuonMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
  , m_channelHist_l0muon(0)
  , m_channelHist_muon(0)
#if _USE_MUONMONREC_TOOL_
  , m_recTool(0)
#endif
  , m_muonBuffer(0)
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
  m_channelHist_l0muon = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "l0muon", this);
  m_channelHist_muon = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "muon", this);
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRawTool", this);
#if _USE_MUONMONREC_TOOL_
  m_recTool = tool<IMuonMonRec>("MuonMonRec");
  if(!m_recTool){
    error()<<"error retrieving the muon mon. rec. tool "<<endmsg;
    return StatusCode::FAILURE;
  }
#endif

  // Run info
  setHistoDir("L0Muon/Analysis");
  std::string name;
  name = "L0EventNumber";
  book1D(name,name,-0.5,-0.5+4096,4096);
  name = "L0_B_Id";
  book1D(name,name,-0.5,-0.5+3564,3564);

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
        m_channelHist_l0muon->bookHistos(iq,reg,sta,m_shortnames);
        m_channelHist_muon->bookHistos(iq,reg,sta,m_shortnames);
      }
    }
  }

  setHistoDir("L0Muon/Analysis");
  // Multiplicity  
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string name;
    name="NPads_per_event_"+L0Muon::MonUtilities::stationName(sta);
    book1D(name,name,-0.5,10.5,11);
    name="NPads_per_bx_"+L0Muon::MonUtilities::stationName(sta);
    book2D(name,name,
           (-1*int(m_time_slots.size()/2))-0.5,(int(m_time_slots.size()/2))+0.5,m_time_slots.size(),
           +0.5,10.5,10);
  }
 
  // Multiplicity  
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string name;
    name="Muon_NPads_per_event_"+L0Muon::MonUtilities::stationName(sta);
    book1D(name,name,-0.5,10.5,11);
    name="Muon_NPads_per_bx_"+L0Muon::MonUtilities::stationName(sta);
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

  sc=setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(0));
  if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
  
  if (excludedBx()) return StatusCode::SUCCESS;
  if (!exclusiveBx()) return StatusCode::SUCCESS;
  if (!selectedTrigger()) return StatusCode::SUCCESS;

  int npad[L0Muon::MonUtilities::NStations]; for (int i=0; i<L0Muon::MonUtilities::NStations; ++i) npad[i]=0;
  int npad_muon[L0Muon::MonUtilities::NStations]; for (int i=0; i<L0Muon::MonUtilities::NStations; ++i) npad_muon[i]=0;

#if _USE_MUONMONREC_TOOL_
  const std::vector<MuonMonHit*> * muonmonhits = m_recTool->hits();
  debug()<<"# of muon hits in event  "<<hits->size()<<endmsg;
#endif

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    sc=setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
    

    if (!exist<LHCb::ODIN> (LHCb::ODINLocation::Default)) continue;

    LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

    ulonglong event=odin->eventNumber();
    //    unsigned int run=odin->runNumber();
    unsigned int bunch=odin->bunchId();

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
    fill(histo1D(name),int(event&0xFFF),1,name);
    name = "L0_B_Id";
    fill(histo1D(name),bunch,1,name);

    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    sc = getL0MuonTiles(l0muontiles);
    if (sc==StatusCode::FAILURE) {
      return StatusCode::SUCCESS;
    }
    
    // Get Muon Hits

#if _USE_MUONMONREC_TOOL_
    std::vector<LHCb::MuonTileID> muonmontiles;
    std::vector<MuonMonHit*>::const_iterator ithits;
    for (ithits=muonmonhits->begin(); ithits<muonmonhits->end(); ++ithits){
      int bx = (int)( ((*ithits)->time()+160)/16 ) -10 ; 
      if ( (*it_ts)==bx ) {
        LHCb::MuonTileID* itile = (*ithits)->tile();
        muonmontiles.push_back(*itile);
      }
    }
#endif

    std::vector<LHCb::MuonTileID> muontiles;
    if (m_muonBuffer) { // If muon raw buffer tool
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        sc = prop->setProperty( "RootInTES", rootInTES() );
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

#if _USE_MUONMONREC_TOOL_
    debug()<<"# of muon hits in "<<rootInTES()<<": muonBuffer = "<<muontiles.size()<<" muonRec= "<<muonmontiles.size()<<endmsg;
#endif    
    
    // Physical channels
    m_channelHist_l0muon->fillHistos(l0muontiles,*it_ts);
    m_channelHist_muon->fillHistos(muontiles,*it_ts);

    // Logical channels
    std::vector<LHCb::MuonTileID> l0muonpads;
    L0Muon::MonUtilities::makePads(l0muontiles,l0muonpads);
    std::vector<LHCb::MuonTileID> muonpads;
    L0Muon::MonUtilities::makePads(muontiles,muonpads);

    // Multiplicity per bx : L0Muon
    int np[L0Muon::MonUtilities::NStations]; for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) np[sta]=0;
    for (std::vector<LHCb::MuonTileID>::iterator itpad=l0muonpads.begin(); itpad<l0muonpads.end();++itpad){
      ++np[itpad->station()];
    }
    for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) {
      name="NPads_per_bx_"+L0Muon::MonUtilities::stationName(sta);
      AIDA::IHistogram2D *hcand2 = histo2D(name);
      if (hcand2!=NULL) fill(hcand2,(*it_ts),np[sta],1.);
    }
    for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) npad[sta]+=np[sta];

    // Multiplicity per bx : Muon
    int np_muon[L0Muon::MonUtilities::NStations]; for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) np_muon[sta]=0;
    for (std::vector<LHCb::MuonTileID>::iterator itpad=muonpads.begin(); itpad<muonpads.end();++itpad){
      ++np_muon[itpad->station()];
    }
    for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) {
      name="Muon_NPads_per_bx_"+L0Muon::MonUtilities::stationName(sta);
      AIDA::IHistogram2D *hcand2 = histo2D(name);
      if (hcand2!=NULL) fill(hcand2,(*it_ts),np_muon[sta],1.);
    }
    for (int sta=0; sta<L0Muon::MonUtilities::NStations; ++sta) npad_muon[sta]+=np_muon[sta];

  } // End of loop over time slots

  // Multiplicity per events
  for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
    int sta = (*its);
    std::string hname="NPads_per_event_"+L0Muon::MonUtilities::stationName(sta);
    AIDA::IHistogram1D *hmulti=histo1D(hname);
    if (hmulti!=NULL) fill(hmulti,npad[sta],1.);
    std::string hname_muon="Muon_NPads_per_event_"+L0Muon::MonUtilities::stationName(sta);
    AIDA::IHistogram1D *hmulti_muon=histo1D(hname_muon);
    if (hmulti!=NULL) fill(hmulti_muon,npad_muon[sta],1.);
  }

//   std::vector<std::pair<LHCb::MuonTileID,int > >  tiles_and_deltaT;
//   sc=compareTiles(tiles_and_deltaT);
//   if( sc.isFailure() ) return Error( "Error comparing Muon and L0Muon hits ", sc );
//   m_channelHist_l0muon->fillHistosDT(tiles_and_deltaT);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  olsErrorSummary(info());
  tilesMismatchSummary(info());
  
  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================

StatusCode L0MuonMonitor::compareTiles(std::vector<std::pair<LHCb::MuonTileID,double > > & tiles_and_deltaT) {

  static long ievt=0;
  if (exist<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default)) {
    LHCb::RecHeader* evt = get<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default);
    ievt = long(evt->evtNumber());
  } else {
    ++ievt;
  }

  StatusCode sc;
  
  std::vector<std::pair<LHCb::MuonTileID,double > > muontiles_and_time;
  std::vector<std::pair<LHCb::MuonTileID,double > > l0muontiles_and_time;

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

    verbose()<<" Nb of OL in error "<<ol_in_error.size()<<endmsg;
    if (msgLevel( MSG::VERBOSE ) ){
      for (std::vector<LHCb::MuonTileID>::iterator itol=ol_in_error.begin(); itol<ol_in_error.end(); ++itol){
        verbose()<<"mismatch - OL : "<<itol->toString()<<" @ "<<(*it_ts)<<" evt "<<ievt<<endmsg;
      }
    }
  
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
      for (std::vector<LHCb::MuonTileID>::iterator itol=ol_in_error.begin(); itol<ol_in_error.end(); ++itol){
        if (itol->intercept(mid)) {
          error()<<"hit "<<mid.toString()<<" @ "<<(*it_ts)<<" should have been masked "
                 <<" - error on OL "<<itol->toString()
                 <<" in PU "<<itol->containerID(MuonLayout(2,2)).toString()<<endmsg;
        }
      }
      std::pair<LHCb::MuonTileID,double> tdt = std::pair<LHCb::MuonTileID,double > (mid,time);
      l0muontiles_and_time.push_back(tdt);
    }
    
    // Get Muon Hits
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> > muontiles;
    if (m_muonBuffer) { // If muon raw buffer tool
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      } else return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
      if (m_muonZS) { // Look at ZS supressed bank
        m_muonBuffer->getTileAndTDC(muontiles);
      } else {// Look at NonZS supressed bank
        m_muonBuffer->getNZSupp(muontiles);
        //         debug()<<"Nb of muon hits : "<<muontiles.size()<<endmsg;
      } // End NonZS supressed bank
      m_muonBuffer->forceReset();
    }// End if muon raw buffer tool
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator itmuon;
    //     debug()<<"Nb of muon hits 2: "<<muontiles.size()<<endmsg;
    for(itmuon=muontiles.begin();itmuon<muontiles.end();++itmuon){
      double time=(*it_ts);

      LHCb::MuonTileID mid=itmuon->first;
      if ( (time<0) && (itmuon->second!=0) ) {
        --time;
      }
      time+=itmuon->second*1./16.;

      bool mask_in_l0muon=false;
      for (std::vector<LHCb::MuonTileID>::iterator itol=ol_in_error.begin(); itol<ol_in_error.end(); ++itol){
        if (itol->intercept(mid)) {
          mask_in_l0muon=true;
          break;
        }
      }
      if (mask_in_l0muon) continue;
      //       debug()<<mid.toString()<<endmsg;
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
  
  // Loop over the l0muon hits
  for (itl0muon_and_time=l0muontiles_and_time.begin();itl0muon_and_time<l0muontiles_and_time.end();++itl0muon_and_time){

    double deltaT=-15.;
    // Loop over the muon hits
    for (itmuon_and_time=muontiles_and_time.begin();itmuon_and_time<muontiles_and_time.end();++itmuon_and_time){

      if (itmuon_and_time->first==itl0muon_and_time->first) {
        double dT=itmuon_and_time->second-itl0muon_and_time->second;
        deltaT = fabs(deltaT)<fabs(dT) ? deltaT : dT;
      }

    } // End of loop over the muon hits
    LHCb::MuonTileID mid=itl0muon_and_time->first;
    std::pair<LHCb::MuonTileID,double> tdt = std::pair<LHCb::MuonTileID,double> (mid,deltaT);
    tiles_and_deltaT.push_back(tdt);
    // if deltaT is still equal to -15, it means the l0muon hit has not been found in the muon
    if (deltaT==-15) mismatch=true;
    if ( (deltaT>=1) || (deltaT<=-1) ){
      std::map<LHCb::MuonTileID,int>::iterator itiles_mismatch =m_tiles_mismatch.find(mid);
      if (itiles_mismatch==m_tiles_mismatch.end()) m_tiles_mismatch[mid]=0;
      m_tiles_mismatch[mid]+=1;
    }
    
  } // End of loop over the l0muon hits

  // Loop over the muon hits
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
      std::map<LHCb::MuonTileID,int>::iterator itile_mismatch=m_tiles_mismatch.find(mid);
      if (itile_mismatch==m_tiles_mismatch.end()) m_tiles_mismatch[mid]=0;
      m_tiles_mismatch[mid]+=1;
    }
  }


  if (msgLevel( MSG::DEBUG ) ){

    if( !mismatch) {
      verbose()<<" Nb of hits : L0Muon= "<<l0muontiles_and_time.size()
            <<" Muon= "<<muontiles_and_time.size()
            <<" ==> OK \n"<<endmsg;
    } else if( mismatch) {

      if (msgLevel( MSG::VERBOSE ) ){
        // Candidates
        // Loop over time slots
        int ncand=0;
        for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
          sc=setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
          if (sc.isFailure()) return Error("Can not set RootInTes ",StatusCode::SUCCESS,10);
          
          if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
          std::string location = LHCb::L0MuonCandidateLocation::Default ;
          if (  exist<LHCb::L0MuonCandidates>(location ) ) {
            LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
            LHCb::L0MuonCandidates::const_iterator itcand;
            for ( itcand= cands->begin(); itcand<cands->end();++itcand) {
              LHCb::MuonTileID seed = (*itcand)->muonTileIDs(2)[0];
              if (!quarterInUse(seed.quarter())) continue;
              if (!regionInUse(seed.region())  ) continue;
              verbose()<<"Candidate found : seed= "<<seed.toString()<<" @ "<<(*it_ts)<<endmsg;
              ++ncand;
            }
          }
        } // End of loop over time slots
        verbose()<<" Nb of candidates : "<<ncand<<endmsg;
      }
      
      debug()<<endmsg;
      debug()<<"MISMATCH !! evt# "<<ievt<<endmsg;
      debug()<<endmsg;
      debug()<<" Nb of hits : L0Muon= "<<l0muontiles_and_time.size()
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
        if (deltaT==-15) {
          debug()<<"mismatch - L0Muon : "<<mid.toString()<<" @ "<<itl0muon_and_time->second;
          debug()<<" NOT FOUND IN MUON"
                <<" should have been in OL "<<m_opt_link_layout.contains(mid).toString()<<endmsg;
        } else {
          verbose()<<"mismatch - L0Muon : "<<mid.toString()<<" @ "<<itl0muon_and_time->second;
          verbose()<<" found in muon with dT= "<<deltaT
                <<" was in OL "<<m_opt_link_layout.contains(mid).toString()<<endmsg;
        }
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
          debug()<<"mismatch - Muon   : "<<(itmuon_and_time->first).toString()<<" @ "<<itmuon_and_time->second
                <<" NOT FOUND IN L0MUON"
                <<" should have been in OL "<<m_opt_link_layout.contains(itmuon_and_time->first).toString()<<endmsg;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
  
}

void L0MuonMonitor::tilesMismatchSummary(MsgStream & msg) const{
  std::map<LHCb::MuonTileID,int>::const_iterator ittile;

  std::map<LHCb::MuonTileID,std::vector<LHCb::MuonTileID> > olsmismatch;
  std::map<LHCb::MuonTileID,std::vector<LHCb::MuonTileID> >::iterator iolsmismatch;
  for (ittile=m_tiles_mismatch.begin(); ittile!=m_tiles_mismatch.end(); ++ittile) {
    LHCb::MuonTileID tile=ittile->first;
    LHCb::MuonTileID ol=m_opt_link_layout.contains(tile);
    iolsmismatch=olsmismatch.find(ol);
    if (iolsmismatch==olsmismatch.end()) {
      std::vector<LHCb::MuonTileID> vide;
      olsmismatch[ol]=vide;
    }
    
    olsmismatch[ol].push_back(tile);
  }

  msg<<"--- L0Muon/Muon comparison error summary : "<<m_tiles_mismatch.size()<<" errors detected "
     <<" on "<<olsmismatch.size()<<" links"<<endmsg;

  for (iolsmismatch=olsmismatch.begin(); iolsmismatch!=olsmismatch.end(); ++iolsmismatch){
    LHCb::MuonTileID ol=iolsmismatch->first;
    LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
    int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
    LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
    int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);

    std::vector<LHCb::MuonTileID> ltiles=iolsmismatch->second;

    msg<<"-  "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
       <<" PB"<<ipb<<" PU"<<ipu
       <<"\t ol= "<<ol.toString()
       <<" : "
       <<"\t nb of tiles in error= "<<ltiles.size()<<endmsg;

    std::vector<LHCb::MuonTileID>::iterator itiles;
    for (itiles=ltiles.begin(); itiles<ltiles.end(); ++itiles){
      LHCb::MuonTileID tile=*itiles;
      const int nerrors=m_tiles_mismatch.find(tile)->second;
      msg<<"-- "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
         <<" PB"<<ipb<<" PU"<<ipu
         <<"\t ol= "<<ol.toString()
         <<" :"
         <<"\t tile= "<<tile.toString()
         <<"\t : "<<nerrors<<" errors"<<endmsg;
    }
  }

//   for (ittile=m_tiles_mismatch.begin(); ittile!=m_tiles_mismatch.end(); ++ittile) {
//     LHCb::MuonTileID tile=ittile->first;
//     int nerrors=ittile->second;
//     LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(tile);
//     int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
//     LHCb::MuonTileID mid_pu=tile.containerID(MuonLayout(2,2));
//     int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
    
//     msg<<"-- "<<"Q"<<(tile.quarter()+1)<<" M"<<(tile.station()+1)<<" R"<<(tile.region()+1)
//        <<" PB"<<ipb<<" PU"<<ipu
//        <<"\t ol= "<<m_opt_link_layout.contains(tile).toString()
//        <<"\t tile= "<<tile.toString()
//        <<"\t : "<<nerrors<<" errors"<<endmsg;
//   }
//   msg<<"---";

}
