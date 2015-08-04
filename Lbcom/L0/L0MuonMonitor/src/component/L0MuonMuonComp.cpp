// Include files 

#include <math.h>

#include "Kernel/MuonLayout.h"

#include "Event/RawEvent.h"
#include "Event/RecHeader.h"
#include "Event/ODIN.h"
#include "Event/L0MuonCandidate.h"
#include "MuonDet/MuonDAQHelper.h"

#include "L0MuonKernel/MonUtilities.h"

// local
#include "L0MuonMuonComp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMuonComp
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMuonComp )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMuonComp::L0MuonMuonComp( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
  , m_h_mismatch(0)
  , m_h_not_aligned(0)
  , m_h_not_centered(0)
  , m_h_missing_in_muon(0)
  , m_h_missing_in_l0muon(0)
  , m_h_present(0)
  , m_channelHist_l0muon(0)
  , m_channelHist_muon(0)
  , m_channelHist_l0muononly(0)
  , m_channelHist_muononly(0)
  , m_channelHist_mismatch(0)
  , m_optlinkHist_mismatch(0)
  , m_optlinkHist_error(0)
  , m_padHist_addCand(0)
  , m_optlinkHist_addCand(0)
  , m_muonBuffer(0)
{
  declareProperty( "MuonZS"  , m_muonZS = true);
  declareProperty( "OutputFileName"  , m_outputFileName = "");
  declareProperty( "PDM",m_pdm=false);
  declareProperty( "PrintOutSplashes",m_splash=false);
  declareProperty( "TAE_ignore_border",m_tae_ignore_border=false);
  declareProperty( "Use_central_finetime",m_use_central_finetime=false);
  m_event_counter=0;

}

//=============================================================================
// Destructor
//=============================================================================
L0MuonMuonComp::~L0MuonMuonComp() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMuonComp::initialize() {
  debug() << "++> Initialize" << endmsg;

  StatusCode sc = L0MuonMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  debug() << "  --> MuonRawTool " << endmsg;
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRawTool", this);
  debug() << "  --> MuonRawTool done" << endmsg;

  m_h_online = book1D("Muon_Vs_L0Muon",-0.5,2.5,3);
  
  if (!m_online) {
    
    // Counters
    initFullTileList();
    m_event_counter=0;
    
    // Tools
    m_channelHist_l0muon     = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "l0muon", this);
    m_channelHist_muon       = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "muon", this);
    m_channelHist_l0muononly = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "l0muonOnly", this);
    m_channelHist_muononly   = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "muonOnly", this);
    m_channelHist_mismatch   = tool<L0MuonChannelsHistos>( "L0MuonChannelsHistos", "mismatch", this);
    m_optlinkHist_mismatch   = tool<L0MuonOptLinksHistos>( "L0MuonOptLinksHistos", "ol_mismatch", this);
    m_optlinkHist_error      = tool<L0MuonOptLinksHistos>( "L0MuonOptLinksHistos", "ol_error", this);
  
    m_padHist_addCand   = tool<L0MuonPadsHistos>( "L0MuonPadsHistos", "addCand", this);
    m_optlinkHist_addCand   = tool<L0MuonOptLinksHistos>( "L0MuonOptLinksHistos", "ol_addCand", this);
  
    // Physical channels & Opt. links
    m_channelHist_l0muon->setHistoDir("L0Muon/MuonComp");
    m_channelHist_muon->setHistoDir("L0Muon/MuonComp");
    m_channelHist_l0muononly->setHistoDir("L0Muon/MuonComp");
    m_channelHist_muononly->setHistoDir("L0Muon/MuonComp");
    m_channelHist_mismatch->setHistoDir("L0Muon/MuonComp");
    m_optlinkHist_mismatch->setHistoDir("L0Muon/MuonComp");
    m_optlinkHist_error->setHistoDir("L0Muon/MuonComp");
    m_padHist_addCand->setHistoDir("L0Muon/MuonComp");
    m_optlinkHist_addCand->setHistoDir("L0Muon/MuonComp");
  
    m_optlinkHist_mismatch->bookMultiHistos(m_shortnames);
    m_optlinkHist_error->bookMultiHistos(m_shortnames);
    for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
      int sta = (*its);
      if (sta>2) continue;
      m_padHist_addCand->bookHistos(sta,m_shortnames);
    }
    
    for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
      int iq = (*itq);
      for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
        int reg = (*itr);
        for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
          int sta = (*its);
          m_channelHist_l0muononly->bookHistos(iq,reg,sta,m_shortnames);
          m_channelHist_muononly->bookHistos(iq,reg,sta,m_shortnames);
          m_channelHist_l0muon->bookHistos(iq,reg,sta,m_shortnames);
          m_channelHist_muon->bookHistos(iq,reg,sta,m_shortnames);
          m_channelHist_mismatch->bookHistos(iq,reg,sta,m_shortnames);
          m_optlinkHist_mismatch->bookHistos(iq,reg,sta,m_shortnames);
          m_optlinkHist_error->bookHistos(iq,reg,sta,m_shortnames);
          m_optlinkHist_addCand->bookHistos(iq,reg,sta,m_shortnames);
        }
      }
    }
  
    // Book additional histos
    m_h_mismatch          = book1D("mismatch",0.5,500.5,500);
    m_h_not_aligned       = book1D("not_aligned",0.5,500.5,500);
    m_h_not_centered      = book1D("not_centered",0.5,500.5,500);
    m_h_missing_in_muon   = book1D("missing_in_muon",0.5,500.5,500);
    m_h_missing_in_l0muon = book1D("missing_in_l0muon",0.5,500.5,500);
    m_h_present           = book1D("present",0.5,500.5,500);
  
  
    // Open output file 
    if (m_outputFileName.size()>0) {
      m_fout.open(m_outputFileName.c_str());
      if (m_splash) {
        m_fout<<"### splash evt number,";
        m_fout<<",mismatch";
        m_fout<<",not_aligned";
        m_fout<<",not_centered";
        m_fout<<",missing_in_muon";
        m_fout<<",missing_in_l0muon";
        m_fout<<",present";
        m_fout<<"\n";
      }
    }
  } // if not online
  

  debug() << "==> Initialize done" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMuonComp::execute() {

  if (m_rnd()>m_prescale) return StatusCode::SUCCESS;

  static ulonglong ievt=0;
  if (exist<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default)) {
    LHCb::RecHeader* evt = get<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default);
    ievt = evt->evtNumber();
  } else {
    ++ievt;
  }

  StatusCode sc;
  
  std::string location;
  std::string name;
  
  debug() << "==> Execute" << endmsg;

  setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(0));
  if (excludedBx()) return StatusCode::SUCCESS;
  if (!exclusiveBx()) return StatusCode::SUCCESS;
  if (!selectedTrigger()) return StatusCode::SUCCESS;

  // Get optical links in error
  sc=storeOlsInError();
  if (sc==StatusCode::FAILURE){
    return Error("can not get olsin error",StatusCode::SUCCESS,100);
  }

  if (!m_online){
    // Compare pads in data and pads from candidates
    std::vector<std::pair<LHCb::MuonTileID,int > >  candpads;
    sc=getCandPadsTAE(candpads);
    if (sc==StatusCode::FAILURE){
      return Error("can not get cand pads",StatusCode::SUCCESS,100);
    }

    std::vector<std::pair<LHCb::MuonTileID,int > >  datapads;
    sc=getL0MuonPadsTAE(datapads);
    //sc=getMuonPadsTAE(datapads);
    if (sc==StatusCode::FAILURE){
      return Error("can not get l0muon pads",StatusCode::SUCCESS,100);
    }

    diffCandAndData(candpads,datapads);
  }

  // Compare tiles from L0Muon and muon

  // Decode muon data to get muon tiles
  std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >  muontiles;
  bool truncated = false;
  sc=getMuonTilesTAE(muontiles,truncated);
  if (sc==StatusCode::FAILURE){
    return Error("can not get muon tiles",StatusCode::SUCCESS,100);
  }
  if (truncated) {
    return Warning("Truncation of Muon data in event, comparison skipped", StatusCode::SUCCESS,20);
  }
  if (!m_online) m_channelHist_muon->fillHistos(muontiles);
  if (msgLevel( MSG::VERBOSE ) ){
    std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >::iterator it;
    for (it=muontiles.begin(); it<muontiles.end(); ++it){
      LHCb::MuonTileID tile=it->first;
      int dt=(it->second).first;
      LHCb::MuonTileID ol=m_opt_link_layout.contains(tile);
      LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
      int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
      LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
      int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
      verbose()<<"--muon--- "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
               <<" PB"<<ipb<<" PU"<<ipu
               <<"\t ol= "<<ol.toString()
               <<"\t tile= "<<tile.toString()<<" in "<<dt<<endmsg;
    }
  }



  std::vector<std::pair<LHCb::MuonTileID,int > >  l0muontiles;
  sc=getL0MuonTilesTAE(l0muontiles);
  if (sc==StatusCode::FAILURE){
    return Error("can not get l0muon tiles",StatusCode::SUCCESS,100);
  }
  if (!m_online) m_channelHist_l0muon->fillHistos(l0muontiles);
  if (msgLevel( MSG::VERBOSE ) ){
    std::vector<std::pair<LHCb::MuonTileID,int > >::iterator it;
    for (it=l0muontiles.begin(); it<l0muontiles.end(); ++it){
      LHCb::MuonTileID tile=it->first;
      int dt=it->second;
      LHCb::MuonTileID ol=m_opt_link_layout.contains(tile);
      LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
      int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
      LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
      int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
      verbose()<<"--l0muon- "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
               <<" PB"<<ipb<<" PU"<<ipu
               <<"\t ol= "<<ol.toString()
               <<"\t tile= "<<tile.toString()<<" in "<<dt<<endmsg;
    }
  }
  
  std::vector<std::pair<LHCb::MuonTileID,int > >  diff;
  bool diff_l0muon_vs_muon=diffL0MuonAndMuon(l0muontiles,muontiles,diff);
  if (!m_online) m_channelHist_l0muon->fillHistosDT(diff);

  // ----------------------------------------------------------------------------
  if ( msgLevel(MSG::DEBUG) ) {
    if (diff_l0muon_vs_muon) {
      setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(0));
      LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
      debug()<<"l0muon_vs_muon : difference found"
             <<", run: "<<odin->runNumber()
             <<", event: "<<odin->eventNumber()
             <<", bid: "<<odin->bunchId()
             << endmsg;
    }
  }
  // ----------------------------------------------------------------------------
  

  fill(m_h_online,0.,1.);
  if (diff_l0muon_vs_muon) fill(m_h_online,2.,1.);
  else fill(m_h_online,1.,1.);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMuonComp::finalize() {

  debug() << "==> Finalize" << endmsg;

  if (!m_online) {
    errorSummary();
    // Close output file
    if (m_fout.is_open()) m_fout.close();
  }
  
  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

StatusCode L0MuonMuonComp::getCandPadsTAE(std::vector<std::pair<LHCb::MuonTileID, int > > & candtiles)
{

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    
    std::string cand_location = LHCb::L0MuonCandidateLocation::PU;
    if (!exist<LHCb::L0MuonCandidates>( cand_location )) 
      return Error("L0MuonCandidates not found at "+cand_location,StatusCode::FAILURE,100);  
    LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( cand_location );

    if (msgLevel( MSG::VERBOSE ) ) {
      verbose()<<"Nb of candidates= "<<cands->size()<<" @"<<(*it_ts)<<endmsg;
    }
    
    LHCb::L0MuonCandidates::const_iterator itcands;
    for ( itcands= cands->begin(); itcands<cands->end();++itcands) {
      for (int sta=0; sta<3; ++sta) {
        if (!stationInUse(sta)) continue;
        std::vector<LHCb::MuonTileID> mids = (*itcands)->muonTileIDs(sta);
        if (mids.size()>1) continue;
        LHCb::MuonTileID mid=mids[0];
        int qua=mid.quarter();
        if (!quarterInUse(qua)) continue;
        int reg=mid.region();
        if (!regionInUse(reg)) continue;
        std::pair<LHCb::MuonTileID, int > tile_and_time= std::pair<LHCb::MuonTileID, int >(mid,*it_ts);
        candtiles.push_back(tile_and_time);
      }
    }
  } // End of loop over time slots
  
  return StatusCode::SUCCESS;
  
}

StatusCode L0MuonMuonComp::getMuonTilesTAE(std::vector<std::pair<LHCb::MuonTileID, std::pair<int,int> > > & muontiles, bool & truncated)
{

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    
    setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    // Get Muon Hits
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> > tiles;
    if (m_muonBuffer) { // If muon raw buffer tool
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      } else return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
      LHCb::RawEvent* rawEvt = getIfExists<LHCb::RawEvent>( LHCb::RawEventLocation::Default , true);
      if (m_muonZS) { // Look at ZS supressed bank
        m_muonBuffer->getTileAndTDC(rawEvt, tiles, rootInTES());

	// Check muon ro saturation
	for (unsigned int TellNum=0;TellNum<MuonDAQHelper_maxTell1Number;TellNum++){
	  for (unsigned int linkNum=0;linkNum<MuonDAQHelper_linkNumber;linkNum++){
	    truncated |= m_muonBuffer->LinkReachedHitLimit(TellNum,linkNum);
	  }
	}
	
      } else {// Look at NonZS supressed bank
        m_muonBuffer->getNZSupp(rawEvt, tiles, rootInTES());
        //         debug()<<"Nb of muon hits : "<<muontiles.size()<<endmsg;
      } // End NonZS supressed bank

      m_muonBuffer->forceReset();
    }// End if muon raw buffer tool

    std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator it;
    //     debug()<<"Nb of muon hits 2: "<<muontiles.size()<<endmsg;
    for(it=tiles.begin();it<tiles.end();++it){
      LHCb::MuonTileID mid=it->first;
      if (!quarterInUse(mid.quarter())) continue;
      if (!stationInUse(mid.station())) continue;
      if (!regionInUse(mid.region())  ) continue;

      std::pair<int,int> time = std::pair<int,int>(*it_ts,it->second);
      std::pair<LHCb::MuonTileID,std::pair<int,int> > tdt = std::pair<LHCb::MuonTileID,std::pair<int,int> >(mid,time);
      muontiles.push_back(tdt);
    }
  } // End of loop over time slots

  return StatusCode::SUCCESS;
}


StatusCode L0MuonMuonComp::getMuonPadsTAE(std::vector<std::pair<LHCb::MuonTileID, int > > & muonpads)
{

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    // Get Muon Hits
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> > tiles_and_tdc;
    if (m_muonBuffer) { // If muon raw buffer tool
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      } else return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
      LHCb::RawEvent* rawEvt = getIfExists<LHCb::RawEvent>( LHCb::RawEventLocation::Default , true);
      if (m_muonZS) { // Look at ZS supressed bank
        m_muonBuffer->getTileAndTDC(rawEvt,tiles_and_tdc, rootInTES());
      } else {// Look at NonZS supressed bank
        m_muonBuffer->getNZSupp(rawEvt,tiles_and_tdc, rootInTES());
        //         debug()<<"Nb of muon hits : "<<muonpads.size()<<endmsg;
      } // End NonZS supressed bank
      m_muonBuffer->forceReset();
    }// End if muon raw buffer tool

    std::vector<LHCb::MuonTileID>  tiles;
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator it;
    for(it=tiles_and_tdc.begin();it<tiles_and_tdc.end();++it){
      LHCb::MuonTileID mid=it->first;
      if (!quarterInUse(mid.quarter())) continue;
      if (!stationInUse(mid.station())) continue;
      if (!regionInUse(mid.region())  ) continue;
      tiles.push_back(mid);
    }
    
    // Build logical channels
    std::vector<LHCb::MuonTileID> pads;
    L0Muon::MonUtilities::makePads(tiles,pads);
    
    std::vector<LHCb::MuonTileID >::iterator itpad;
    for(itpad=pads.begin();itpad<pads.end();++itpad){
      LHCb::MuonTileID mid=*itpad;
      std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int > (mid,*it_ts);
      muonpads.push_back(tdt);
    }
    
  } // End of loop over time slots

  return StatusCode::SUCCESS;
}



bool L0MuonMuonComp::diffCandAndData(std::vector<std::pair<LHCb::MuonTileID,int > > & candtiles,
                                     std::vector<std::pair<LHCb::MuonTileID,int > > & datatiles) {
  
  std::vector<LHCb::MuonTileID > diff;

  std::vector<std::pair<LHCb::MuonTileID,int > >::iterator itcand;
  for (itcand=candtiles.begin(); itcand<candtiles.end(); ++itcand){
    bool found=false;
    int match_time=15;
    std::vector<std::pair<LHCb::MuonTileID,int > >::iterator itdata;
    for (itdata=datatiles.begin(); itdata<datatiles.end(); ++itdata){
      if (*itdata==*itcand) {
        found=true;
        match_time=itdata->second;
        break;
      }
    }
    if (!found){
      diff.push_back(itcand->first);
    }
    if (msgLevel( MSG::VERBOSE ) || msgLevel( MSG::DEBUG )) {
      LHCb::MuonTileID tile=itcand->first;
      LHCb::MuonTileID ol=m_opt_link_layout.contains(tile);
      LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
      int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
      LHCb::MuonTileID mid_pu=tile.containerID(MuonLayout(2,2));
      int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
      if (!found){
        debug()<<"!  "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
               <<" PB"<<ipb<<" PU"<<ipu
               <<"\t ol= "<<ol.toString()
               <<"\t pad= "<<tile.toString()
               <<" use to make a candidate @ "<<itcand->second<<" but not in data"<<endmsg;
      } else {
        verbose()<<"   "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
                 <<" PB"<<ipb<<" PU"<<ipu
                 <<"\t ol= "<<ol.toString()
                 <<"\t pad= "<<tile.toString()
                 <<" use to make a candidate @ "<<itcand->second<<" was found in data @ "<<match_time<<endmsg;
      }
    }
  }

  if (!m_online) {
    m_padHist_addCand->fillHistos(diff);
    m_optlinkHist_addCand->fillHistos(diff);
  }
  
  return (diff.size()>0);
  
}


bool L0MuonMuonComp::diffL0MuonAndMuon(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles,
                                       std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > > & muontiles_original,
                                       std::vector<std::pair<LHCb::MuonTileID,int > > & diff) {
  // Fill a list (diff) with the hits seen by the muon system (detector or trigger) and their difference in time
  // -15 : hit was not seen by the detector (muon)
  // +15 : hit was not seen by the trigger (l0muon)

  // Get TAE window
  std::vector<int> active_slots;
  activeTS(active_slots);
  int ts_min=+10;
  int ts_max=-10;
  for (std::vector<int>::iterator it=active_slots.begin(); it<active_slots.end();++it){
    if (*it<ts_min) ts_min=*it;
    if (*it>ts_max) ts_max=*it;
  }
  
  int i_mismatch=0;
  int i_not_aligned=0;
  int i_not_centered=0;
  int i_missing_in_muon=0;
  int i_missing_in_l0muon=0;
  int i_present=0;

  bool mismatch_found=false;

  std::vector<std::pair<LHCb::MuonTileID,int > >::iterator itl0muon;
  std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >::iterator itmuon;

  std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > > muontiles = muontiles_original;
  
  bool centered=false;

  // Loop over the l0muon hits
  for (itl0muon=l0muontiles.begin();itl0muon<l0muontiles.end();++itl0muon){

    // Ignore hit at the border of the TAE
    //info() << "itl0muon->second "<<itl0muon->second<<" tmin,max"<<ts_min<<" , "<<ts_max<<endmsg;
    if ( (m_tae_ignore_border) && ( (itl0muon->second>=ts_max) || (itl0muon->second<=ts_min)) ) continue ;
    //info() << "itl0muon->second OK"<<endmsg;
    
    LHCb::MuonTileID l0mid=itl0muon->first;

    int deltaT=-15;
    int finetime=8;
    std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >::iterator muon_pos=muontiles.end();
    // Loop over the muon hits
    for (itmuon=muontiles.begin();itmuon<muontiles.end();++itmuon){

      if (itmuon->first==l0mid) {
        std::pair<int,int> time=itmuon->second;
        int ts=time.first;
        int dT=ts-itl0muon->second;
        if (abs(deltaT)>abs(dT)){
          deltaT=dT;
          finetime= (muon_pos->second).second;
          muon_pos=itmuon;
          if (itl0muon->second==0) centered=true;
          else centered=false;
        }
      }

    } // End of loop over the muon hits

    // Remove the muon hit if matched to a l0muon hit 
    if (muon_pos<muontiles.end()) {
      muontiles.erase(muon_pos);
      if ( (m_use_central_finetime) && ( (finetime<3) || (finetime>12)) ) continue ;
    }

    std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int> (l0mid,deltaT);

    // Update the difference list
    diff.push_back(tdt);
    if ( deltaT!=0 ) mismatch_found=true;

    if (!m_online) { // if online
      
      // Update counters
      std::map<LHCb::MuonTileID,int>::iterator itiles_present =m_tiles_present.find(l0mid);
      if (itiles_present==m_tiles_present.end()) m_tiles_present[l0mid]=0;
      m_tiles_present[l0mid]+=1;
      i_present+=1;
      
      if ( deltaT!=0 ){
        // muon tile is missing (deltaT=-15) or is not on time with the l0muon tile
        std::map<LHCb::MuonTileID,int>::iterator itiles_mismatch =m_tiles_mismatch.find(l0mid);
        if (itiles_mismatch==m_tiles_mismatch.end()) m_tiles_mismatch[l0mid]=0;
        m_tiles_mismatch[l0mid]+=1;
        i_mismatch+=1;
      } 
      if ( (deltaT!=0) && (deltaT!=-15) ){
        // muon tile is not on time with the l0muon tile
        std::map<LHCb::MuonTileID,int>::iterator itiles_not_aligned =m_tiles_not_aligned.find(l0mid);
        if (itiles_not_aligned==m_tiles_not_aligned.end()) m_tiles_not_aligned[l0mid]=0;
        m_tiles_not_aligned[l0mid]+=1;
        i_not_aligned+=1;
      } 
      if ( deltaT==-15 ){
        // muon tile is missing 
        std::map<LHCb::MuonTileID,int>::iterator itiles_missing_in_muon =m_tiles_missing_in_muon.find(l0mid);
        if (itiles_missing_in_muon==m_tiles_missing_in_muon.end()) m_tiles_missing_in_muon[l0mid]=0;
        m_tiles_missing_in_muon[l0mid]+=1;
        i_missing_in_muon+=1;
      } 
      if ( (!mismatch_found) && (!centered) ) {
        // muon tile and l0muon tile are there and on time but not on the central bunch crossing
        std::map<LHCb::MuonTileID,int>::iterator itiles_not_centered=m_tiles_not_centered.find(l0mid);
        if (itiles_not_centered==m_tiles_not_centered.end()) m_tiles_not_centered[l0mid]=0;
        m_tiles_not_centered[l0mid]+=1;
        i_not_centered+=1;
      }
    
      std::map<LHCb::MuonTileID,int>::iterator itiles_present_bis =m_tiles_present_bis.find(l0mid);
      if (itiles_present_bis==m_tiles_present_bis.end()) m_tiles_present_bis[l0mid]=0;
      m_tiles_present_bis[l0mid]+=1;

    } // if not online
    
  } // End of loop over the l0muon hits

  // Loop over the remaining muon hits (those which were not match to a l0muon hit)
  for (itmuon=muontiles.begin();itmuon<muontiles.end();++itmuon){

    // Ignore hit at the border of the TAE
    if ((m_tae_ignore_border) && ( ((itmuon->second).first>=ts_max) || ((itmuon->second).first<=ts_min)) ) continue ;

    // Ignore non-central hit in the TS
    if ( (m_use_central_finetime) && ( ((itmuon->second).second<3) || ((itmuon->second).second>12)) ) continue ;
    
    LHCb::MuonTileID mid=itmuon->first;
    
    // Discard tile if it belongs to an OL in error
    LHCb::MuonTileID ol = m_opt_link_layout.contains(mid);
    std::pair<int,int> time=itmuon->second;
    std::pair<LHCb::MuonTileID,int> ol_ts=std::pair<LHCb::MuonTileID,int> (ol,time.first);
    if (m_err_ols.find(ol_ts)!=m_err_ols.end()) {
      if (msgLevel( MSG::VERBOSE )) {
        LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
        int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
        LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
        int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
        verbose()<<"+ "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
                 <<" PB"<<ipb<<" PU"<<ipu
                 <<"\t ol= "<<ol.toString()
                 <<"\t tile= "<<((*itmuon).first).toString()
                 <<" => muon tile discarded from comparison"<<endmsg;
      }
      continue;
    }
    
    // Update the difference list
    int deltaT=+15;
    std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int> (mid,deltaT);
    diff.push_back(tdt);
    mismatch_found=true;

    if (!m_online) { // if online
      // Update counter
      std::map<LHCb::MuonTileID,int>::iterator itiles_present =m_tiles_present.find(mid);
      if (itiles_present==m_tiles_present.end()) m_tiles_present[mid]=0;
      m_tiles_present[mid]+=1;
      i_present+=1;

      std::map<LHCb::MuonTileID,int>::iterator itile_mismatch=m_tiles_mismatch.find(mid);
      if (itile_mismatch==m_tiles_mismatch.end()) m_tiles_mismatch[mid]=0;
      m_tiles_mismatch[mid]+=1;
      i_mismatch+=1;

      std::map<LHCb::MuonTileID,int>::iterator itile_missing_in_l0muon=m_tiles_missing_in_l0muon.find(mid);
      if (itile_missing_in_l0muon==m_tiles_missing_in_l0muon.end()) m_tiles_missing_in_l0muon[mid]=0;
      m_tiles_missing_in_l0muon[mid]+=1;
      i_missing_in_l0muon+=1;

      std::map<LHCb::MuonTileID,int>::iterator itiles_present_bis =m_tiles_present_bis.find(mid);
      if (itiles_present_bis==m_tiles_present_bis.end()) m_tiles_present_bis[mid]=0;
      m_tiles_present_bis[mid]+=1;

    } // if not online

  } // End of loop over the remaining muon hits

  if (!m_online) { // if online
  
    if (m_splash && m_fout.is_open() ) {
      bool error_splash=false;
      error_splash|=((i_mismatch           >500) );
      error_splash|=((i_not_aligned        >500) );
      error_splash|=((i_not_centered       >500) );
      error_splash|=((i_missing_in_muon    >500) );
      error_splash|=((i_missing_in_l0muon  >500) );
      if (!m_pdm) {
        error_splash|=(i_present            >500);
      }
      
      if (error_splash) {
        m_fout<<"### splash evt "<<m_event_counter;
        m_fout<<", "<<i_mismatch          ;
        m_fout<<", "<<i_not_aligned       ;
        m_fout<<", "<<i_not_centered      ;
        m_fout<<", "<<i_missing_in_muon   ;
        m_fout<<", "<<i_missing_in_l0muon ;
        m_fout<<", "<<i_present;
        m_fout<<"\n";
      }
    }
  
    // Fill histos
    fill(m_h_mismatch          , double( i_mismatch          ),1. );
    fill(m_h_not_aligned       , double( i_not_aligned       ),1. );
    fill(m_h_not_centered      , double( i_not_centered      ),1. );
    fill(m_h_missing_in_muon   , double( i_missing_in_muon   ),1. );
    fill(m_h_missing_in_l0muon , double( i_missing_in_l0muon ),1. );
    fill(m_h_present           , double( i_present           ),1. );
    
    if (mismatch_found) {
      std::vector<LHCb::MuonTileID> l0muon_only;
      std::vector<LHCb::MuonTileID> muon_only;
      std::vector<LHCb::MuonTileID> mismatch;
      std::vector<std::pair<LHCb::MuonTileID,int > >::iterator it_diff;
      for (it_diff=diff.begin(); it_diff<diff.end(); ++it_diff){
        int dt=it_diff->second;
        if (dt==0)  continue;
        mismatch.push_back(it_diff->first);
        if (dt==-15) {
          l0muon_only.push_back(it_diff->first);
        } else if (dt== 15) {
          muon_only.push_back(it_diff->first);
        }
      }
      
      m_channelHist_l0muononly->fillHistos(l0muon_only);
      m_channelHist_muononly->fillHistos(muon_only);
      m_channelHist_mismatch->fillHistos(mismatch);
      m_channelHist_mismatch->fillHistosDT(diff);
      m_optlinkHist_mismatch->fillHistos(mismatch);
      m_optlinkHist_error->fillHistos(m_err_ols);
    }
  
    // VERBOSE or DEBUG print out
    if (msgLevel( MSG::VERBOSE ) || msgLevel( MSG::DEBUG )){
  
      std::vector<std::pair<LHCb::MuonTileID,int > >::iterator it_diff;
      for (it_diff=diff.begin(); it_diff<diff.end(); ++it_diff){
        LHCb::MuonTileID tile=it_diff->first;
        int dt=it_diff->second;
        LHCb::MuonTileID ol=m_opt_link_layout.contains(tile);
        LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
        int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
        LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
        int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
        if (dt==0) {
          verbose()<<"-  "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
                    <<" PB"<<ipb<<" PU"<<ipu
                    <<"\t ol= "<<ol.toString()
                    <<"\t tile= "<<tile.toString()<<endmsg;
        } else if (dt==-15) {
          debug()<<"-  "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
                 <<" PB"<<ipb<<" PU"<<ipu
                 <<"\t ol= "<<ol.toString()
                 <<"\t tile= "<<tile.toString()<<" missing in MUON"<<endmsg;
        } else if (dt== 15) {
          debug()<<"-  "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
                 <<" PB"<<ipb<<" PU"<<ipu
                 <<"\t ol= "<<ol.toString()
                 <<"\t tile= "<<tile.toString()<<" missing in L0MUON"<<endmsg;
        } else {
          debug()<<"-  "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
                 <<" PB"<<ipb<<" PU"<<ipu
                 <<"\t ol= "<<ol.toString()
                 <<"\t tile= "<<tile.toString()<<" not in time: "<<dt<<endmsg;
        }
      } 
  
    } // End of VERBOSE or DEBUG print out

  } // if not online

  m_event_counter+=1;

  return mismatch_found;

}

//=============================================================================

void L0MuonMuonComp::errorSummary() {

  if (!m_fout.is_open()) return;

  
  m_fout<<"\n% "<<m_event_counter<<" events analysed\n";
  m_fout<<"\n* OL_input\n"; 
  olsCounterSummary();
  m_fout<<"\n* Mismatch\n"; 
  tilesCounterSummary(m_tiles_mismatch);
  m_fout<<"\n* Missing_in_muon \n"; 
  tilesCounterSummary(m_tiles_missing_in_muon);
  m_fout<<"\n* Missing_in_l0muon \n"; 
  tilesCounterSummary(m_tiles_missing_in_l0muon);
  m_fout<<"\n* Not_aligned \n"; 
  tilesCounterSummary(m_tiles_not_aligned);
  m_fout<<"\n* Not_centered \n"; 
  tilesCounterSummary(m_tiles_not_centered);

  if (m_pdm){
    for (std::vector<LHCb::MuonTileID>::iterator it=m_full_tile_list.begin();it!=m_full_tile_list.end();++it){
      if (!quarterInUse(it->quarter())) continue;
      if (!stationInUse(it->station())) continue;
      if (!regionInUse(it->region())  ) continue;
      std::map<LHCb::MuonTileID,int>::iterator itiles_present =m_tiles_present.find(*it);
      if (itiles_present==m_tiles_present.end()) m_tiles_absent[*it]=m_event_counter;
      else if (m_tiles_present[*it]<m_event_counter) m_tiles_absent[*it]=m_event_counter-m_tiles_present[*it];
    }
    m_fout<<"\n* Absent \n"; 
    tilesCounterSummary(m_tiles_absent);
  }

  if (fullDetail()){
    m_fout<<"\n* Present\n"; 
    tilesCounterSummary(m_tiles_present);
    m_fout<<"\n* PresentBis # check\n"; 
    tilesCounterSummary(m_tiles_present_bis);
  }
  
}

void L0MuonMuonComp::olsCounterSummary() {

  if (!m_fout.is_open()) return;

  m_fout<<"# L0Muon input optical links error summary : "<<m_err_ols_stored.size()<<" errors detected"<<"\n";
  std::map<LHCb::MuonTileID,int>::const_iterator itol;
  for (itol=m_err_ols_stored.begin(); itol!=m_err_ols_stored.end(); ++itol) {
    LHCb::MuonTileID ol=itol->first;
    int nerrors=itol->second;
    LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
    int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
    LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
    int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
    
    m_fout<<"+ "
        <<" Q"<<(ol.quarter()+1)<<"M"<<(ol.station()+1)<<"R"<<(ol.region()+1)<<"PB"<<ipb<<"PU"<<ipu
        <<" "<<ol.toString()
        <<" "<<"???"
        <<" "<<nerrors<<" errors"<<"\n";
  }
  
}


void L0MuonMuonComp::tilesCounterSummary(const std::map<LHCb::MuonTileID,int> & tiles_counter) {

  if (!m_fout.is_open()) return;

  std::map<LHCb::MuonTileID,int>::const_iterator ittile;

  std::map<LHCb::MuonTileID,std::vector<LHCb::MuonTileID> > ol_tiles_map;
  std::map<LHCb::MuonTileID,std::vector<LHCb::MuonTileID> >::iterator iol_tiles_map;
  for (ittile=tiles_counter.begin(); ittile!=tiles_counter.end(); ++ittile) {
    LHCb::MuonTileID tile=ittile->first;
    LHCb::MuonTileID ol=m_opt_link_layout.contains(tile);
    iol_tiles_map=ol_tiles_map.find(ol);
    if (iol_tiles_map==ol_tiles_map.end()) {
      std::vector<LHCb::MuonTileID> vide;
      ol_tiles_map[ol]=vide;
    }
    
    ol_tiles_map[ol].push_back(tile);
  }

  m_fout<<"# L0Muon/Muon comparison error summary : "<<tiles_counter.size()<<" errors detected "
      <<" on "<<ol_tiles_map.size()<<" links"<<"\n";

  for (iol_tiles_map=ol_tiles_map.begin(); iol_tiles_map!=ol_tiles_map.end(); ++iol_tiles_map){
    LHCb::MuonTileID ol=iol_tiles_map->first;
    LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
    int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
    LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
    int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);

    std::vector<LHCb::MuonTileID> ltiles=iol_tiles_map->second;

    m_fout<<"+ "
        <<" Q"<<(ol.quarter()+1)<<"M"<<(ol.station()+1)<<"R"<<(ol.region()+1)<<"PB"<<ipb<<"PU"<<ipu
        <<" "<<ol.toString()
        <<" "<<"???"
        <<" "<<ltiles.size()<<" tiles in error \n";;

    std::vector<LHCb::MuonTileID>::iterator itiles;
    for (itiles=ltiles.begin(); itiles<ltiles.end(); ++itiles){
      LHCb::MuonTileID tile=*itiles;
      const int nerrors=tiles_counter.find(tile)->second;
      std::map<LHCb::MuonTileID,int>::const_iterator ittile_present=m_tiles_present.find(tile);
      if (!m_pdm) {
        if (ittile_present==m_tiles_present.end()){
          m_fout<<"ERROR : Tile " <<tile.toString()<<" in error but not in list of present tiles\n";
          error()<<" : tile "<<tile.toString()<<" in error but not in list of present tiles"<<endmsg;
          continue;
        }
      }
      int nhits=ittile_present->second;
      double err_rate=double(nerrors)/double(nhits);
      m_fout<<"-     "
            <<" Q"<<(ol.quarter()+1)<<"M"<<(ol.station()+1)<<"R"<<(ol.region()+1)<<"PB"<<ipb<<"PU"<<ipu
            <<" "<<tile.toString()
            <<" "<<"???"
            <<" "<<err_rate<<" = "<<nerrors<<" / "<< nhits<<"\n";
    }
  }
  
}

void L0MuonMuonComp::initFullTileList()
{

  MuonSystemLayout pad_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                                                MuonStationLayout(MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(12,8),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0)),
                                                MuonStationLayout(MuonLayout(12,8),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0),
                                                                  MuonLayout(0,0)));
  MuonSystemLayout stripH_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                                                   MuonStationLayout(MuonLayout(8,8),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(8,8),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(4,8),
                                                                     MuonLayout(2,8),
                                                                     MuonLayout(2,8)));
  
  MuonSystemLayout stripV_layout =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                                                   MuonStationLayout(MuonLayout(48,1),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2)),
                                                   MuonStationLayout(MuonLayout(48,1),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2),
                                                                     MuonLayout(48,2)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2)),
                                                   MuonStationLayout(MuonLayout(0,0),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2),
                                                                     MuonLayout(12,2)));

  std::vector<LHCb::MuonTileID> tiles;

  tiles=pad_layout.tiles();
  m_full_tile_list.insert(m_full_tile_list.end(),tiles.begin(),tiles.end());
  
  tiles=stripH_layout.tiles();
  m_full_tile_list.insert(m_full_tile_list.end(),tiles.begin(),tiles.end());
  
  tiles=stripV_layout.tiles();
  m_full_tile_list.insert(m_full_tile_list.end(),tiles.begin(),tiles.end());

}

StatusCode L0MuonMuonComp::isMuonTruncatedTAE( bool & truncated)
{
  //
  //  m_muonBuffer->LinkReachedHitLimit force the deconding of tiles if not done yet
  // The decoding will look for the rawevent in the location given by the decoder helper 
  //

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    
    setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    if (m_muonBuffer) { // If muon raw buffer tool
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      } else return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
      for (unsigned int TellNum=0;TellNum<MuonDAQHelper_maxTell1Number;TellNum++){
        for (unsigned int linkNum=0;linkNum<MuonDAQHelper_linkNumber;linkNum++){
          truncated |= m_muonBuffer->LinkReachedHitLimit(TellNum,linkNum);
        }
      }
    } else  return Error( "Unable to find the MuonRawBuffer Tool",  StatusCode::FAILURE);
  }
  
  return StatusCode::SUCCESS;
  
}
