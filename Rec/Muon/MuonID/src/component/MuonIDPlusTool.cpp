#include <stdlib.h>

// local
#include "MuonIDPlusTool.h"
#include "MuonID/CommonMuonHit.h"
#include "MuonID/IMuonMatchTool.h"

#include "Event/ODIN.h"
#include "Event/MuonPID.h"

#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/IMuonFastPosTool.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonInterfaces/IMuonPadRec.h"
#include "MuonInterfaces/IMuonClusterRec.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonInterfaces/MuonHit.h"
#include "Event/MuonCoord.h"
#include "Event/Track.h"
#include "Event/MuonPID.h"

#include "Event/State.h"
#include "TrackInterfaces/ITrackExtrapolator.h"


using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonIDPlusTool::MuonIDPlusTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),  m_lastRun(0), m_lastEvent(0), theMuon(13)
{
  declareInterface<IMuonIDTool>(this);

  declareProperty( "UseFirstStation", m_useM1 = false,
                   "also use M1 hits for muonID");
  declareProperty( "SearchSigmaCut" , m_searchSigmaCut = 9.,
                   "number of sigma of extrap. error defining initial search window for muon hits");
  declareProperty( "MaxTrackChi2" , m_maxTrackChi2 = 10.,
                   "max chi2/dof for considered tracks");
  declareProperty( "MaxGhostProb" , m_maxGhostProb = .8,
                   "max ghost prob. for considered tracks");
  declareProperty( "MinTrackMomentum" , m_minTrackMomentum = 3.*Gaudi::Units::GeV,
                   "minimum momentum  for considered tracks");

  declareProperty("MaxCluSize", m_maxCluSize=8,
                  "do not consider cluster above this size for track fitting (just attach after fit)");

  declareProperty("InputTracksLocation", m_BestTrackLocation =LHCb::TrackLocation::Default,
                  "address of best tracks container");
  declareProperty("MatchToolName", m_matchToolName = "MuonChi2MatchTool",
                  "Tool for track extrapolation and match to muon stations");
  declareProperty("ReleaseObjectOwnership", m_ReleaseObjectOwnership = true,
                  "Assume all created MuonTrack and MuonPID objects will be stored to TES");
}

StatusCode MuonIDPlusTool::initialize() {
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "initializing MuonIDPlusTool" <<endmsg;

  m_mudet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  nStations = m_mudet->stations();
  nRegions = m_mudet->regions();


  m_muhits.resize(nStations);
  for (std::vector<CommonMuonHits>::iterator is=m_muhits.begin(); is!=m_muhits.end(); is++)
    is->reserve(MAXHITS); // ensure that pointers to stored hits won't change
  m_hitCounter.resize(nStations);
  m_stationHasLargeCluster.resize(nStations);
  m_matchM.resize(nStations);
  m_matchSigma.resize(nStations);
  m_matchX.resize(nStations);
  m_matchY.resize(nStations);
  m_matchT.resize(nStations);
  m_matchdT.resize(nStations);
  m_clusize.resize(nStations);
  m_isoM.resize(nStations);

  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator", this );
  m_posTool = tool<IMuonFastPosTool>("MuonFastPosTool");
  m_padrectool = tool<IMuonPadRec>("MuonPadFromCoord");
  m_clustertool=tool<IMuonClusterRec>("MuonClusterRec");
  m_matchTool = tool<IMuonMatchTool>(m_matchToolName, this);

  m_lastRun=m_lastEvent=0;

  return sc;
}


void MuonIDPlusTool::clearPIDs() {
  // need to delete created objects if they are not handled by TES
  if(m_ReleaseObjectOwnership == false) {
    for (std::vector<LHCb::MuonPID*>::iterator ipid=m_muonPIDs.begin() ; ipid != m_muonPIDs.end() ; ipid++) {
      if(*ipid) {
        if((*ipid)->muonTrack()) delete ((*ipid)->muonTrack());
        delete *ipid;
      }
    }
  }
  m_muonPIDs.clear();
}

StatusCode MuonIDPlusTool::eventInitialize()
{
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default);
  if ( !odin ) { odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default,false); }
  if ( !odin )
  {
    // should always be available ...
    return Error( "Cannot load the ODIN data object", StatusCode::SUCCESS );
  }

  if (odin->runNumber()   != m_lastRun ||
      odin->eventNumber() != m_lastEvent) {
    // initialize the event doing preliminary matching between all tracks and all muon hits
    debug() << "Begin Event: load muon hits"<<endmsg;
    clearPIDs();
    getMuonHits();
    matchHitsToTracks();

    m_lastRun = odin->runNumber();
    m_lastEvent = odin->eventNumber();
  }
  return StatusCode::SUCCESS;

}

void MuonIDPlusTool::initVariables() {
  m_nSmatched=m_friendShares=0;
  m_medianClusize=0.;
  m_maxisoM=-1.;
  for (unsigned int s=0; s< nStations ; s++) {
    m_matchM[s]=m_clusize[s]=0; m_isoM[s]=m_matchSigma[s]=999.;m_matchX[s]=m_matchY[s]=m_matchT[s]=m_matchdT[s]=0.;
  }

  m_isMuonLoose = m_isMuon = m_isMuonTight = false;

}



void MuonIDPlusTool::checkMuIsolation(const LHCb::Track *pTrack, std::vector<CommonConstMuonHits>* mucoord) {
  std::map<const LHCb::Track*, int> nStcloseTr;
  std::map<const LHCb::Track*, bool> closeTr;
  debug() << "--- starting mu isolation check---"<<endmsg;

  for (unsigned int s=0; s<nStations; s++) {
    closeTr.clear();
    m_isoM[s]=999.;
    CommonConstMuonHits* sthits = &((*mucoord)[s]);
    debug() << "size hits in M"<<s+1<<"  ="<<sthits->size()<<endmsg;
    for (auto hit = sthits->begin(); hit != sthits->end(); hit++) {
      const CommonMuonHit* link=*hit;
      if(link->clusterSize() > m_maxCluSize) {
        m_isoM[s] = 0.; // large clusters are not isolated by definition
        continue;
      }
      debug() << "ckMuIso M"<<s+1<<" There are "<<m_mutrkmatchTable[link].size()<<" matches for this hit"<<endmsg;
      if(m_mutrkmatchTable[link].size() > 1) {
        for (auto imatch=m_mutrkmatchTable[link].begin() ; imatch < m_mutrkmatchTable[link].end(); imatch++) {
          if (imatch->first == pTrack) continue;
          // require that competing track has enough momentum for this station
          debug() << "concurrent track with momentum "<<imatch->first->p()/Gaudi::Units::GeV<<" GeV"<<endmsg;
          if (imatch->first->p() < ( m_mudet->getStationZ(s) > 18*Gaudi::Units::m ? 6*Gaudi::Units::GeV :
                                     (  m_mudet->getStationZ(s) > 17*Gaudi::Units::m ?  5*Gaudi::Units::GeV : m_minTrackMomentum)) )  continue;
          if (imatch->second < m_isoM[s]) m_isoM[s] = imatch->second;
          closeTr[imatch->first]=true;
        }
      }
    }
    for (auto ict=closeTr.begin() ; ict != closeTr.end(); ict++) {
      nStcloseTr[ict->first]++;
    }
    debug() << "isoM station "<<s+1<<" = "<<m_isoM[s]<<endmsg;
  }
  int maxctr=-1;
  const LHCb::Track* tfriend=nullptr;
  for (auto isct=nStcloseTr.begin() ; isct != nStcloseTr.end(); isct++) {
    if (isct->second > maxctr) {
      maxctr=isct->second;
      tfriend=isct->first;
    }
  }
  if(tfriend) {
    m_friendShares=maxctr;
    debug() << "closest sharing track is "<<*tfriend<<" with "<<m_friendShares <<" common hits"<<endmsg;
  }
  else {
    m_friendShares=0;
  }

  m_maxisoM=0.;
  for (unsigned int s=1; s<nStations ; s++) {
    if(m_matchM[s] == 2 && m_isoM[s]>m_maxisoM)
      m_maxisoM =  m_isoM[s];
  }

}

double MuonIDPlusTool::medianClusize() {
  double mcl=0.;
  std::vector<double> thiscls;
  for (unsigned int s=0; s<nStations ; s++)
    if(m_matchM[s] == 2) thiscls.push_back(m_clusize[s]);
  if(thiscls.size()>1) {
    std::sort (thiscls.begin(), thiscls.end());
    if (thiscls.size()%2 == 1)
      mcl=thiscls[thiscls.size()/2];
    else
      mcl=(thiscls[thiscls.size()/2-1]+thiscls[thiscls.size()/2])/2.;
  }
  return mcl;
}





LHCb::MuonPID* MuonIDPlusTool::getMuonID(const LHCb::Track* track) {
  std::unique_ptr<MuonPID> muPid;
  initVariables();
  if (!track) return muPid.release();


  // check that the current event has been intialized
  eventInitialize();

  muPid.reset(new MuonPID());

  m_muonPIDs.push_back(muPid.get());
  muPid->setIDTrack(track);
  muPid->setPreSelMomentum(track->p() > m_minTrackMomentum);
  bool inAcc = m_trkInAcceptance.count(track) ? (m_trkInAcceptance[track])[nStations] : false;
  muPid->setInAcceptance(inAcc);
  m_lasttrack = track; m_lastPID = muPid.get();

  // run the matching tool for this track
  StatusCode matchStatus = m_matchTool->run(track, &(m_trkmumatchTable[track]), &(m_trkmumatchTableSpares[track]));
  if(matchStatus.isFailure()) {
    warning() << " Failed to match track to Muon Detector! " << endreq;
    return muPid.release();
  }

  // now make the muonTrack
  std::unique_ptr<LHCb::Track> mtrack{ new LHCb::Track(muPid->key()) };
  mtrack->addToAncestors(*track);
  mtrack->addToStates(track->closestState(m_mudet->getStationZ(0)));


  // get informations from the best matched hit in each station
  CommonConstMuonHits matchedMuonHits;
  m_matchTool->getListofCommonMuonHits(matchedMuonHits, -1, true);
  m_nSmatched =0;
  m_nVmatched =0;


  for (const auto& hit : matchedMuonHits ) {
    ++m_nSmatched;
    mtrack->addToLhcbIDs(hit->tile());

    int station = hit->station();
    m_matchM[station] = hit->uncrossed() ? 1 : 2;
    m_nVmatched += m_matchM[station];
    m_matchX[station] = hit->x();
    m_matchY[station] = hit->y();
    m_matchT[station] = hit->time();
    m_matchdT[station] = hit->deltaTime();
    m_clusize[station] = hit->clusterSize();
    m_matchSigma[station] = m_matchTool->getMatchSigma(station);
    debug()<<"station M"<<station+1<<" matched with type "<<m_matchM[station]<<" sigma="<<m_matchSigma[station]<<endmsg;
  }

  debug() <<"There are "<<m_nSmatched<<" matched stations for this track" << endmsg;


  if(m_nSmatched>0) {
    int ndof;
    double chi2= m_matchTool->getChisquare(ndof);
    mtrack->setChi2AndDoF (chi2, ndof);
    mtrack->setType(LHCb::Track::Muon);
    mtrack->setHistory(LHCb::Track::MuonID);
    mtrack->addInfo(LHCb::Track::MuonMomentumPreSel, m_minTrackMomentum);
    mtrack->addInfo(LHCb::Track::MuonInAcceptance, muPid->InAcceptance());
    mtrack->addInfo(LHCb::Track::IsMuonLoose, muPid->IsMuonLoose());
    mtrack->addInfo(LHCb::Track::IsMuon, muPid->IsMuon());
    mtrack->addInfo(LHCb::Track::IsMuonTight, muPid->IsMuonTight());
    mtrack->addInfo(LHCb::Track::MuonChi2perDoF, mtrack->chi2PerDoF());
    muPid->setMuonTrack(mtrack.release());

    m_medianClusize = medianClusize();

    // check that muon hits are not compatible with other tracks
    std::vector<CommonConstMuonHits> matchedHitsByStation;
    for (unsigned int s=0; s<nStations; s++) {
      CommonConstMuonHits matchInstation;
      m_matchTool->getListofCommonMuonHits(matchInstation, s, true);
      matchedHitsByStation.push_back(matchInstation);
    }
    checkMuIsolation(track, &matchedHitsByStation);

    // set binary flags
    setIsMuon(track->p(), muPid->muonTrack()->chi2PerDoF());
  }

  muPid->setIsMuon(m_isMuon);
  muPid->setIsMuonLoose(m_isMuonLoose);
  muPid->setIsMuonTight(m_isMuonTight);


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // note that LLmuon, LLbg and Nshared are not (yet) defined by this tool
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  return muPid.release();
}


void MuonIDPlusTool::setIsMuon(double momentum, double chi2perdof ) {

  // this is very preliminary and untuned and not (yet) really used

  int requiredViews =  std::min( 7,  std::max ( 3, (int) (3 + 4*( momentum - 3*Gaudi::Units::GeV )/6*Gaudi::Units::GeV  )) );

  m_isMuonLoose = m_nVmatched > requiredViews;

  m_isMuon = m_isMuonLoose && chi2perdof<20;

  m_isMuonTight = m_isMuon && chi2perdof<5;

}


StatusCode MuonIDPlusTool::getMuonHits(){
//==========================================================================
  // get the Muon hits, compute the spatial coordinates and store them by station

  for (auto& i : m_muhits) i.clear();

  m_largeClusters=false;
  for(unsigned int s = 0; s<nStations ; s++) {
      m_stationHasLargeCluster[s]=false;
      m_hitCounter[s]=0;
  }

  double x,dx,y,dy,z,dz;

  const std::vector<MuonLogPad*> *muRawHits=m_padrectool->pads();
  // clusterize muon signals to assign cluster size to each hit
  // loop over the clusters and store hits by station
  for (const auto& clu : *m_clustertool->clusters(muRawHits)) {
    int cls=  clu->npads();
    int st= clu->station();
    if (st == 0 &&  nStations == 5 && m_useM1 == false) continue;
    if (cls>m_maxCluSize) {
      m_largeClusters=true; // remember that this event have hits that won't be used for track fitting but could be attached to tracks
      m_stationHasLargeCluster[st]=true;
    }
    for(const auto& pad : *clu->logPads() ) {
      LHCb::MuonTileID tile= pad->tile();
      auto sc = m_posTool->calcTilePos(tile,x,dx,y,dy,z,dz);
      if (sc.isFailure()){
        warning() << " Failed to get x,y,z of tile " << tile << endreq;
        continue;
      }
      if(++m_hitCounter[st] < MAXHITS) {
        m_muhits[st].emplace_back(tile,x,dx,y,dy,z,dz,false,pad->time(),pad->dtime(),cls);
      } else
        warning() << "maximum number of stored hits reached for station M"<<st+1<<endmsg;
    }
  }

  // also store uncrossed log. hits
  int cl1v=1;
  for (const auto& onev : *muRawHits ) {
    MuonLogPad* pad = const_cast<MuonLogPad*>(onev);
    if (pad->truepad()) continue;
    LHCb::MuonTileID tile= pad->tile();
    if(++(m_hitCounter[tile.station()]) < MAXHITS) {
      m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
      m_muhits[tile.station()].emplace_back(tile,x,dx,y,dy,z,dz,true,pad->time(),pad->dtime(),cl1v);
    } else
      warning() << "maximum number of stored hits reached for station M"<<tile.station()+1<<endmsg;
  }
  debug() <<"Muon hits per station:";
  for(unsigned int s = 0; s<nStations ; s++)
    debug() <<" M"<<s+1<<" "<<m_hitCounter[s];
  debug() <<endmsg;

  return StatusCode::SUCCESS;
}


bool MuonIDPlusTool::isTrackInsideStation(const LHCb::State& state, unsigned int istation) const {
  auto xcond=((fabs(state.x())+sqrt(state.errX2()) > m_mudet->getInnerX(istation))
           && (fabs(state.x())-sqrt(state.errX2()) < m_mudet->getOuterX(istation)));
  auto ycond=((fabs(state.y())+sqrt(state.errY2()) > m_mudet->getInnerY(istation))
           && (fabs(state.y())-sqrt(state.errY2()) < m_mudet->getOuterY(istation)));
  return (xcond && ycond);
}

StatusCode MuonIDPlusTool::matchHitsToTracks()
// check compatibility of each muon hit with all good long tracks
{
  m_mutrkmatchTable.clear();
  m_trkmumatchTable.clear();
  m_trkmumatchTableSpares.clear();
  m_nStationsWithMatch=0;

  std::vector<bool> matchInStation(nStations);
  for(unsigned int s = 0; s<nStations ; s++)
    matchInStation[s]=false;

  const LHCb::Tracks* bestTracks = get<LHCb::Tracks>( m_BestTrackLocation );
  LHCb::Tracks::const_iterator t;
  std::pair<const LHCb::Track*, float> trackMatch;
  TrackMuMatch muhitMatch;
  double xdist,ydist,Err2x,Err2y,mtcSigmax,mtcSigmay;
  int nm=0;
  debug() << "Start matching tracks with muon hits"<<endmsg;
  for (const auto& trk : *bestTracks) {
    if( trk->type() != LHCb::Track::Long &&  trk->type() != LHCb::Track::Downstream)  continue;
    if( trk->chi2PerDoF() > m_maxTrackChi2)  continue;
    if( trk->ghostProbability() > m_maxGhostProb)  continue;
    if( trk->p() < m_minTrackMomentum)  continue;
    LHCb::State ExtraState = trk->closestState(m_mudet->getStationZ(0));
    std::vector<bool> inAcc(nStations+1,true); // inacceptance bit for each station + global

    for(unsigned int station = (m_useM1 ? 0 : 1); station<nStations ; station++) {
      if (fabs(m_mudet->getStationZ(station)-ExtraState.z())>1*Gaudi::Units::cm) {
        m_extrapolator->propagate(ExtraState, m_mudet->getStationZ(station), theMuon);
        verbose()<< "Track extrapolation to station M"<<station+1<<" has errors (in cm) "<<sqrt(ExtraState.errX2())/Gaudi::Units::cm<<"/"<<sqrt(ExtraState.errY2())/Gaudi::Units::cm<<endmsg;
      }
      inAcc[station]=isTrackInsideStation(ExtraState, station);  // is in acceptance of this station
      inAcc[nStations+1] = (inAcc[station] && inAcc[nStations+1]); // is in acceptance of all stations

      for ( auto ihit=m_muhits[station].begin(); ihit<m_muhits[station].end(); ihit++) {
        CommonMuonHit* hit= &(*ihit);
        bool spare =  (hit->uncrossed()) ||  // hit in single view, treat as spare
          (hit->clusterSize() > m_maxCluSize);     // treat also large clusters as spare hits
        xdist = hit->x()- (ExtraState.x()+ ExtraState.tx()*(hit->z()-ExtraState.z()));
        ydist = hit->y()- (ExtraState.y()+ ExtraState.ty()*(hit->z()-ExtraState.z()));
        Err2x = (hit->dx()*hit->dx()*4)/12. +  ExtraState.errX2();
        Err2y = (hit->dy()*hit->dy()*4)/12. +  ExtraState.errY2();
        mtcSigmax = fabs(xdist)/sqrt(Err2x);
        mtcSigmay = fabs(ydist)/sqrt(Err2y);
        if (mtcSigmax < m_searchSigmaCut &&
            mtcSigmay < m_searchSigmaCut) { // found compatible muon hit with this track
          verbose() << "    found match in station "<<station+1 <<" sigma= "<<sqrt(mtcSigmax*mtcSigmax + mtcSigmay*mtcSigmay)<<" clusize="<<hit->clusterSize()<<endmsg;
          trackMatch.first = trk;
          std::get<0>(muhitMatch) =hit;
          std::get<1>(muhitMatch)= trackMatch.second= sqrt(mtcSigmax*mtcSigmax + mtcSigmay*mtcSigmay);
          std::get<2>(muhitMatch)= ExtraState.x() + ExtraState.tx()*(hit->z()-ExtraState.z());
          std::get<3>(muhitMatch)= ExtraState.y() + ExtraState.ty()*(hit->z()-ExtraState.z());
          m_mutrkmatchTable[hit].push_back(trackMatch);
          if(spare)
            m_trkmumatchTableSpares[trk].push_back(muhitMatch);
          else
            m_trkmumatchTable[trk].push_back(muhitMatch);
          if (!matchInStation[station]) {
            matchInStation[station]=true;
            m_nStationsWithMatch++;
          }
          nm++;
        }
      }
      m_trkInAcceptance[trk] = inAcc;
    }
  }
  debug() << "end of matchHitsToTracks found nmatch="<<nm<<endmsg;
  return StatusCode::SUCCESS;
}


double MuonIDPlusTool::muonIDPropertyD(const LHCb::Track* track, const char* propertyName, int station) {
  if (m_lasttrack != track) getMuonID(track);
  std::string prop(propertyName);
  double out=-9999.;
  bool reqStation = (station>-1 && station < (int)nStations);
  if(prop == "matchSigma") {
    if(reqStation) out=m_matchSigma[station];
  }
  else if(prop == "mediancs") {
    out=m_medianClusize;
  }
  else if(prop == "iso") {
    if(reqStation) out=m_isoM[station];
  }
   else if(prop == "maxIso") {
     out=m_maxisoM;
  }
  else if(prop == "time") {
    if(reqStation) out=m_matchT[station];
  }
  else if(prop == "dtime") {
    if(reqStation) out=m_matchdT[station];
  }
  return out;
}

int MuonIDPlusTool::muonIDPropertyI(const LHCb::Track* track, const char* propertyName, int station) {
  if (m_lasttrack != track) getMuonID(track);
  std::string prop(propertyName);
  int out=-9999;
  bool reqStation = (station>-1 && station < (int)nStations);
  if (prop == "match") {
    if(reqStation)   out=m_matchM[station];
  }
  else if(prop == "clusize") {
    if(reqStation)   out=m_clusize[station];
  }
  else if(prop == "matchedStations") {
    out=m_nSmatched;
  }
  else if(prop == "matchedViews") {
    out=m_nVmatched;
  }
  else if(prop == "MaxSharedHits") {
    out=m_friendShares;
  }
  return out;
}

DECLARE_TOOL_FACTORY( MuonIDPlusTool )
