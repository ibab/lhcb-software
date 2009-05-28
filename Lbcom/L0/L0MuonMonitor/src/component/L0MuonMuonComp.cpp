// $Id: L0MuonMuonComp.cpp,v 1.4 2009-05-28 13:52:36 jucogan Exp $
// Include files 

#include <math.h>
#include <fstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Kernel/MuonLayout.h"

#include "Event/RawEvent.h"
#include "Event/RecHeader.h"

#include "Event/L0MuonCandidate.h"

#include "L0MuonKernel/MonUtilities.h"

// local
#include "L0MuonMuonComp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMuonComp
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMuonComp );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMuonComp::L0MuonMuonComp( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
{
  declareProperty( "MuonZS"  , m_muonZS = true);
  declareProperty( "OutputFileName"  , m_outputFileName = "");
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMuonComp::~L0MuonMuonComp() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMuonComp::initialize() {
  StatusCode sc = L0MuonMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

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

  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRawTool", this);

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

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMuonComp::execute() {

  static int ievt=0;
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

  // Compare tiles from L0Muon and muon
  std::vector<std::pair<LHCb::MuonTileID,int > >  l0muontiles;
  sc=getL0MuonTilesTAE(l0muontiles);
  if (sc==StatusCode::FAILURE){
    return Error("can not get l0muon tiles",StatusCode::SUCCESS,100);
  }
  m_channelHist_l0muon->fillHistos(l0muontiles);
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
  
  std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >  muontiles;
  sc=getMuonTilesTAE(muontiles);
  if (sc==StatusCode::FAILURE){
    return Error("can not get muon tiles",StatusCode::SUCCESS,100);
  }
  m_channelHist_muon->fillHistos(muontiles);
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

  std::vector<std::pair<LHCb::MuonTileID,int > >  diff;
  diffL0MuonAndMuon(l0muontiles,muontiles,diff);
  m_channelHist_l0muon->fillHistosDT(diff);

  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMuonComp::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  olsErrorSummary(info());
  tilesMismatchSummary(info());
  if (m_outputFileName.size()>0) tilesMismatchSummary();
  
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

StatusCode L0MuonMuonComp::getMuonTilesTAE(std::vector<std::pair<LHCb::MuonTileID, std::pair<int,int> > > & muontiles)
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
      if (m_muonZS) { // Look at ZS supressed bank
        m_muonBuffer->getTileAndTDC(tiles);
      } else {// Look at NonZS supressed bank
        m_muonBuffer->getNZSupp(tiles);
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
      if (m_muonZS) { // Look at ZS supressed bank
        m_muonBuffer->getTileAndTDC(tiles_and_tdc);
      } else {// Look at NonZS supressed bank
        m_muonBuffer->getNZSupp(tiles_and_tdc);
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



void L0MuonMuonComp::diffCandAndData(std::vector<std::pair<LHCb::MuonTileID,int > > & candtiles,
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

  m_padHist_addCand->fillHistos(diff);
  m_optlinkHist_addCand->fillHistos(diff);
  
}

void L0MuonMuonComp::diffL0MuonAndMuon(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles,
                                       std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > > & muontiles_original,
                                       std::vector<std::pair<LHCb::MuonTileID,int > > & diff) {
  bool mismatch_found=false;

  std::vector<std::pair<LHCb::MuonTileID,int > >::iterator itl0muon;
  std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >::iterator itmuon;

  std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > > muontiles = muontiles_original;
  
  // Loop over the l0muon hits
  for (itl0muon=l0muontiles.begin();itl0muon<l0muontiles.end();++itl0muon){

    int deltaT=-15;
    std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > >::iterator muon_pos=muontiles.end();
    // Loop over the muon hits
    for (itmuon=muontiles.begin();itmuon<muontiles.end();++itmuon){

      if (itmuon->first==itl0muon->first) {
        std::pair<int,int> time=itmuon->second;
        int ts=time.first;
        int dT=ts-itl0muon->second;
        if (abs(deltaT)>abs(dT)){
          deltaT=dT;
          muon_pos=itmuon;
        }
      }

    } // End of loop over the muon hits
    if (muon_pos<muontiles.end()) muontiles.erase(muon_pos);

    LHCb::MuonTileID mid=itl0muon->first;
    std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int> (mid,deltaT);
    diff.push_back(tdt);
    if ( deltaT!=0 ){
      mismatch_found=true;
      std::map<LHCb::MuonTileID,int>::iterator itiles_mismatch =m_tiles_mismatch.find(mid);
      if (itiles_mismatch==m_tiles_mismatch.end()) m_tiles_mismatch[mid]=0;
      m_tiles_mismatch[mid]+=1;
    }
    
  } // End of loop over the l0muon hits

  // Loop over the muon hits
  for (itmuon=muontiles.begin();itmuon<muontiles.end();++itmuon){
    
    // Discard tile if it belongs to an OL in error
    LHCb::MuonTileID ol = m_opt_link_layout.contains((*itmuon).first);
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
    

    //     bool found=false;
    //     for (itl0muon=l0muontiles.begin();itl0muon<l0muontiles.end();++itl0muon){
    //       if (itmuon->first==itl0muon->first) {
    //         found=true;
    //         break;
    //       }
    //     }
    //     if (!found) {
    //       int deltaT=+15;
    //       LHCb::MuonTileID mid=itmuon->first;
    //       std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int> (mid,deltaT);
    //       diff.push_back(tdt);
    //       mismatch_found=true;
    //       std::map<LHCb::MuonTileID,int>::iterator itile_mismatch=m_tiles_mismatch.find(mid);
    //       if (itile_mismatch==m_tiles_mismatch.end()) m_tiles_mismatch[mid]=0;
    //       m_tiles_mismatch[mid]+=1;
    //     }
    int deltaT=+15;
    LHCb::MuonTileID mid=itmuon->first;
    std::pair<LHCb::MuonTileID,int> tdt = std::pair<LHCb::MuonTileID,int> (mid,deltaT);
    diff.push_back(tdt);
    mismatch_found=true;
    std::map<LHCb::MuonTileID,int>::iterator itile_mismatch=m_tiles_mismatch.find(mid);
    if (itile_mismatch==m_tiles_mismatch.end()) m_tiles_mismatch[mid]=0;
    m_tiles_mismatch[mid]+=1;

  }
  
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
  }
  
}



//=============================================================================

void L0MuonMuonComp::tilesMismatchSummary(MsgStream & msg) const{
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
void L0MuonMuonComp::tilesMismatchSummary() const{

  std::ofstream fout;
  fout.open(m_outputFileName.c_str());

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

  fout<<"--- L0Muon/Muon comparison error summary : "<<m_tiles_mismatch.size()<<" errors detected "
     <<" on "<<olsmismatch.size()<<" links"<<"\n";;

  for (iolsmismatch=olsmismatch.begin(); iolsmismatch!=olsmismatch.end(); ++iolsmismatch){
    LHCb::MuonTileID ol=iolsmismatch->first;
    LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(ol);
    int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
    LHCb::MuonTileID mid_pu=ol.containerID(MuonLayout(2,2));
    int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);

    std::vector<LHCb::MuonTileID> ltiles=iolsmismatch->second;

    fout<<"-  "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
       <<" PB"<<ipb<<" PU"<<ipu
       <<"\t ol= "<<ol.toString()
       <<" : "
       <<"\t nb of tiles in error= "<<ltiles.size()<<"\n";;

    std::vector<LHCb::MuonTileID>::iterator itiles;
    for (itiles=ltiles.begin(); itiles<ltiles.end(); ++itiles){
      LHCb::MuonTileID tile=*itiles;
      const int nerrors=m_tiles_mismatch.find(tile)->second;
      fout<<"-- "<<"Q"<<(ol.quarter()+1)<<" M"<<(ol.station()+1)<<" R"<<(ol.region()+1)
         <<" PB"<<ipb<<" PU"<<ipu
         <<"\t ol= "<<ol.toString()
         <<" :"
         <<"\t tile= "<<tile.toString()
         <<"\t : "<<nerrors<<" errors"<<"\n";;
    }
  }

//   for (ittile=m_tiles_mismatch.begin(); ittile!=m_tiles_mismatch.end(); ++ittile) {
//     LHCb::MuonTileID tile=ittile->first;
//     int nerrors=ittile->second;
//     LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(tile);
//     int ipb=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
//     LHCb::MuonTileID mid_pu=tile.containerID(MuonLayout(2,2));
//     int ipu=(mid_pu.nY()%2)*2+(mid_pu.nX()%2);
    
//     fout<<"-- "<<"Q"<<(tile.quarter()+1)<<" M"<<(tile.station()+1)<<" R"<<(tile.region()+1)
//        <<" PB"<<ipb<<" PU"<<ipu
//        <<"\t ol= "<<m_opt_link_layout.contains(tile).toString()
//        <<"\t tile= "<<tile.toString()
//        <<"\t : "<<nerrors<<" errors"<<"\n";
//   }
//   fout<<"---"<<"\n";
  fout.close();
  
}
