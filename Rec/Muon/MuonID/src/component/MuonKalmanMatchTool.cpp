#include "MuonKalmanMatchTool.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonID/CommonMuonHit.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Node.h"
#include "Event/FitNode.h"
#include "Event/TrackFitResult.h"
#include "Event/ChiSquare.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "GaudiKernel/SystemOfUnits.h"
#include <TMatrixF.h>
#include <vector>
#include <algorithm>


/** @class MuonKalmanMatchTool MuonKalmanMatchTool.h component/MuonKalmanMatchTool.h
 *
 *  muon match tool based on Kalman filter through Muon stations
 *
 *  @author Giacomo Graziani
 *  @date   2015-11-11
 */


MuonKalmanMatchTool::MuonKalmanMatchTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),  theMuon(13), m_lasttrack(NULL)
{
  declareInterface<IMuonMatchTool>(this);
  declareProperty("OnlyCrossedHits",m_OnlyCrossedHits=false,
                  "use only crossed hits");
  declareProperty( "MaxmatchDisplacement", m_maxDeltaCut=5,
                   "keep all muon matches below these number of delta-chi");
  declareProperty( "MaxmatchBestDisplacement", m_maxDeltaCutStation=8,
                   "keep best muon match per station below these number of delta-chi");
  declareProperty( "OutlierCut", m_outlierCut=5,
                   "minimal number of sigma (x or y) for outlier removal after track fit");
  declareProperty("MaxMatches", m_maxMatches=100,
                  "do not consider tracks with more than these matches in M2-M5");
}

StatusCode MuonKalmanMatchTool::initialize() {
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;


  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator",this );
  m_trackFitter = tool<ITrackFitter>( "TrackMasterFitter", this );

  // initialize geometry
  m_mudet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  nStations = m_mudet->stations();
  nRegions = m_mudet->regions();

  m_bestMatchedTile.resize(nStations);
  m_bestPM.resize(nStations);

  // vectors to store the output
  m_hitOnStation.resize(nStations);
  m_bestmatchedHits.resize(nStations);


  return StatusCode::SUCCESS;
}

void MuonKalmanMatchTool::muOutlierRemoval(LHCb::Track * mutrack, LHCb::LHCbID id) {
  if (! (mutrack->fitResult()) ) return;
  for (std::vector<LHCb::Node*>::const_iterator iter = mutrack->fitResult()->nodes().begin();
       iter<mutrack->fitResult()->nodes().end(); iter++) {
    LHCb::Node* node=*iter;
    if (node) {
      if(node->measurement().lhcbID() == id)
        static_cast<LHCb::FitNode*>(node)->deactivateMeasurement();
    }
  }
  mutrack->removeFromLhcbIDs(id);
}


double  MuonKalmanMatchTool::matchStateToMuonHit(const CommonMuonHit* hit, LHCb::State &StState, double& sigmax, double& sigmay ) {
  double xmatch = fabs(hit->x()- (StState.x()+StState.tx()*(hit->z()-StState.z())));
  double ymatch = fabs(hit->y()- (StState.y()+StState.ty()*(hit->z()-StState.z())));
  double Err2x = (hit->dx()*hit->dx()*4)/12. +  StState.errX2();
  double Err2y = (hit->dy()*hit->dy()*4)/12. +  StState.errY2();
  sigmax =  xmatch/sqrt(Err2x);
  sigmay =  ymatch/sqrt(Err2y);
  return sqrt(sigmax*sigmax+sigmay*sigmay);
}




// =============== MAIN FUNCTION ===============
StatusCode MuonKalmanMatchTool::run(const LHCb::Track* pTrack,
                                    std::vector<TrackMuMatch>* bestMatches,
                                    std::vector<TrackMuMatch>* spareMatches)
{
  m_lasttrack = pTrack;
  m_lastMatchesBest = bestMatches;
  m_lastMatchesSpare = spareMatches;

  m_matchedHits.clear();
  for (unsigned int S=0; S< nStations ; S++) {
    m_hitOnStation[S]=false;
    m_bestmatchedHits[S]=NULL;
  }

  m_muchi2 = m_mymuchi2  =-1.;
  m_mudof = m_mymudof =1;

  if (!pTrack) return StatusCode::SUCCESS;
  if(!bestMatches)  return  StatusCode::SUCCESS;
	if(bestMatches->size() <2) return  StatusCode::SUCCESS; // require at least 2 reasonable matches


  const LHCb::State muState = pTrack->closestState(m_mudet->getStationZ(0));
  std::vector<LHCb::LHCbID> muGoodHits;
  std::vector< CommonConstMuonHits > matchedCoords(nStations);



  // look to possible matches (keep everything below m_maxDeltaCut Delta-chi, and the best per station below m_maxDeltaCutStation
  int nSmatched=0;
  for(unsigned int station = 0; station < nStations ; station++) {
    m_bestPM[station]=99;
  }

  int ok25=0;
  std::vector<TrackMuMatch>::iterator ih;
  for (ih=bestMatches->begin(); ih != bestMatches->end(); ih++) {
    float nSigmaMatch= std::get<1>(*ih);
    if (nSigmaMatch<m_maxDeltaCutStation) {
      const CommonMuonHit* link=std::get<0>(*ih);
      LHCb::LHCbID thisTile = (LHCb::LHCbID) link->tile();
      int station = link->station();
      if (nSigmaMatch<m_bestPM[station]) {
        m_bestPM[station]=nSigmaMatch;
        m_bestMatchedTile[station]=thisTile;
        m_bestmatchedHits[station]=link;
      }
      if (nSigmaMatch<m_maxDeltaCut) {
        if (!m_hitOnStation[station]) {
          m_hitOnStation[station]=true;
          nSmatched++;
        }
        if(nStations==5 && station>0) ok25++;
        muGoodHits.push_back(thisTile);
        matchedCoords[station].push_back(link);
      }
    }
  }


  // attach best hit if nothing found within  m_maxDeltaCut sigma
  for(unsigned int station = 0; station < nStations ; station++) {
    if (!m_hitOnStation[station] && m_bestPM[station]<m_maxDeltaCutStation) {
      m_hitOnStation[station]=true;
      nSmatched++;
      if(nStations==5 && station>0) ok25++;
      muGoodHits.push_back(m_bestMatchedTile[station]);
      matchedCoords[station].push_back(m_bestmatchedHits[station]);
    }
  }

  if(ok25> m_maxMatches) {
    warning()<<" Too many muon matches: track is bad or too crowded event, ending now"<<endmsg;
    return StatusCode::SUCCESS;
  }

  //repeat fast check
  if (nSmatched < 2 || (pTrack->p() > 10*Gaudi::Units::GeV && nSmatched<3)) {
    debug()<<" Too few muon stations matched with this track, ending now"<<endmsg;
    return StatusCode::SUCCESS;
  }


  LHCb::Track mutrack;
  mutrack.addToStates(muState);
  // attach muon hits to track
  for ( std::vector<LHCb::LHCbID>::reverse_iterator rit=muGoodHits.rbegin() ;
        rit < muGoodHits.rend(); ++rit ) {
    mutrack.addToLhcbIDs( *rit );
    debug() << "added muon logical pad " <<  *rit<< endmsg;
  }


  // refit  track (Kalman filter) with muon hits
  debug() << "starting mutrack with muon hits before fit" << endmsg;
  debug() << mutrack  << endmsg;
  m_trackFitter -> fit(mutrack, theMuon);
  debug() << "track with muon hits" << endmsg;
  debug() <<mutrack  << endmsg;




  bool refit=true;
  // choose best hits and remove outliers iteratively
  LHCb::State StState;
  double sigmax=999,sigmay=999;
  while (refit) {
    refit=false;
    m_muchi2=0;
    m_mudof=2; // first point of fit is the starting state (track extrapolation)
    nSmatched=0;
    //    StState = mutrack.closestState(m_mudet->getStationZ(0));
    for(unsigned int station = 0; station<nStations ; station++) {
      m_bestmatchedHits[station]=NULL;
      m_bestPM[station]=9.e9;
      StState = mutrack.closestState(m_mudet->getStationZ(station));
      if (fabs(m_mudet->getStationZ(station)-StState.z())>1*Gaudi::Units::cm)
        m_extrapolator->propagate(StState, m_mudet->getStationZ(station), theMuon);
      CommonConstMuonHits::iterator hit=(matchedCoords[station]).begin();
      while( hit< (matchedCoords[station]).end()) {
        double mchi = matchStateToMuonHit(*hit, StState, sigmax, sigmay);
        if (sigmax > m_outlierCut || sigmay > m_outlierCut) {
          refit=true;
          muOutlierRemoval(&mutrack, (LHCb::LHCbID) ((*hit)->tile()) );
          hit=matchedCoords[station].erase(hit);
          debug() << "outlier removed from station "<< station <<" dx/dy ="<<sigmax<<"/"<<sigmay<<endmsg;
        }
        else {
          m_muchi2 += mchi*mchi;
          m_mudof += 2;
          if (mchi<m_bestPM[station]) {
            if(!m_bestmatchedHits[station]) nSmatched++;
            m_bestPM[station]=mchi;
            m_bestmatchedHits[station]=*hit;
          }
          hit++;
        }
      }
    }
    //repeat fast check again
    if (nSmatched < 2 || (pTrack->p() > 10*Gaudi::Units::GeV && nSmatched<3)) {
      debug()<<" Too few muon stations matched with this track after outlier removal, ending now"<<endmsg;
      return StatusCode::SUCCESS;
    }
    if (refit) {
      debug() << "refitting track after outlier removal chi2prob was " << mutrack.probChi2() << endmsg;
      m_trackFitter -> fit(mutrack, theMuon);
      debug() << "refitted track after outlier removal chi2prob is now "<<mutrack.probChi2()<<endmsg;
      debug() <<mutrack  << endmsg;
    }
  } // end outlier check

  m_mudof -= 4; // 4 fitted parameters (x,y,tx,ty)


  // check also low quality muon hits for stations with no good match
  if(spareMatches) {
    if(spareMatches->size() > 0) {
      for(unsigned int station = 0; station<nStations ; station++) {
        if (m_bestmatchedHits[station]) continue;
        m_bestPM[station]=9.e9;
        StState = mutrack.closestState(m_mudet->getStationZ(station));
        if (fabs(m_mudet->getStationZ(station)-StState.z())>1*Gaudi::Units::cm)
          m_extrapolator->propagate(StState, m_mudet->getStationZ(station), theMuon);
        for (ih=spareMatches->begin(); ih != spareMatches->end(); ih++) {
          const CommonMuonHit* hit=std::get<0>(*ih);
          unsigned int s = hit->station();
          if (s != station) continue;
          if (m_OnlyCrossedHits &&  hit->uncrossed()) continue;
          double mchi = matchStateToMuonHit(hit, StState, sigmax, sigmay);
          if(mchi > m_maxDeltaCutStation)  continue;
          debug() << "    poor quality match: station "<< station <<" dx/dy ="<<sigmax<<" / "<<sigmay<<endmsg;
          matchedCoords[station].push_back(hit);
          if (mchi<m_bestPM[station]) {
            m_bestPM[station]=mchi;
            m_bestmatchedHits[station]=hit;
          }
        }
      }
    }
  }

  // save output hits
  for(unsigned int station = 0; station<nStations ; station++)
    m_matchedHits.insert(m_matchedHits.end(),matchedCoords[station].begin(),matchedCoords[station].end());

  // evaluate chi2 from best hits of mu stations only
  m_mymudof=2; // first point of fit is the starting state (track extrapolation)
  m_mymuchi2=0.;
  m_missedStations=0;

  for(unsigned int station = 0; station<nStations ; station++) {
    if (!m_bestmatchedHits[station]) {
      m_missedStations++;
      m_hitOnStation[station]=false;
      continue;
    }
    m_hitOnStation[station]=true;
    m_mymudof += 2;
    m_mymuchi2 += m_bestPM[station];
  }
  m_mymudof -= 4; // 4 fitted parameters (x,y,tx,ty)

  debug() << "MuonKalmanMatchTool output: chi2="<<m_muchi2<<" with ndof="<<m_mudof<<endmsg;
  debug() << "using best hits only: chi2="<<m_mymuchi2<<" with ndof="<<m_mymudof<<endmsg;

  return StatusCode::SUCCESS;
}

void MuonKalmanMatchTool::getListofCommonMuonHits(CommonConstMuonHits& matchedMuonHits, int station, bool onlybest) {
  matchedMuonHits.clear();
  if(onlybest) {
    if(station>-1 && station<(int)nStations && m_bestmatchedHits[station]) {
      matchedMuonHits.push_back(m_bestmatchedHits[station]);
    } else {
      std::copy_if( m_bestmatchedHits.begin(), m_bestmatchedHits.end(),
                    std::back_inserter(matchedMuonHits),
                    [](const CommonMuonHit* hit) -> bool { return hit; } );
    }
  } else {
    std::copy_if( m_matchedHits.begin(), m_matchedHits.end(),
                  std::back_inserter(matchedMuonHits),
                  [&](const CommonMuonHit* hit) {
      return station<0 || station == (int) hit->station();
                  } );
  }
}

void MuonKalmanMatchTool::getListofMuonTiles(std::vector<LHCb::MuonTileID>& matchedTiles, int station, bool onlybest) {
  matchedTiles.clear();
  if(onlybest) {
    if(station>-1 && station<(int)nStations && m_bestmatchedHits[station]) {
      matchedTiles.push_back(m_bestmatchedHits[station]->tile());
    } else {
      std::transform( m_bestmatchedHits.begin(), m_bestmatchedHits.end(),
                     std::back_inserter(matchedTiles),
                     [](const CommonMuonHit* hit) { return hit->tile(); } );
    }
  } else {
    for (auto ih=m_matchedHits.begin(); ih!=m_matchedHits.end(); ih++)
      if(station<0 || station == (int) ((*ih)->station()) ) matchedTiles.push_back((*ih)->tile());
  }
}

double MuonKalmanMatchTool::getChisquare(int& ndof) {
  ndof= m_mymudof;
  return m_mymuchi2;
}

IMuonMatchTool::MuonMatchType MuonKalmanMatchTool::getMatch(int station) {
  if(!m_hitOnStation[station]) return IMuonMatchTool::NoMatch;
  if(m_bestmatchedHits[station]->uncrossed()) return IMuonMatchTool::Uncrossed;
  return IMuonMatchTool::Good;
}


double MuonKalmanMatchTool::getMatchSigma(int station) {
  // retrieve the original match distance in sigma for this station
  verbose() << "requested MatchSigma for M"<<station+1<<" lastTrack is "<<m_lasttrack<<" match is "<<m_hitOnStation[station]<<endmsg;
  double out=999.;
  if(!m_hitOnStation[station]) return out;
  if(m_lasttrack) {
    auto mvector = ( m_lastMatchesBest ?  m_lastMatchesBest : nullptr );
    if(!mvector && m_lastMatchesSpare) mvector = m_lastMatchesSpare;
    if(mvector) {
      auto im = std::find_if( mvector->begin(), mvector->end(),
                              [&](const TrackMuMatch& tmm) {
                                return std::get<0>(tmm) == m_bestmatchedHits[station];
                              } );
      if (im!=mvector->end()) out = std::get<1>(*im);
    }
  }
  return out;
}

double MuonKalmanMatchTool::muonMatchPropertyD(const char* propertyName, int ) {
  return -9999.;
}

int MuonKalmanMatchTool::muonMatchPropertyI(const char* propertyName, int) {
  return std::string{propertyName} == "matchedStations" ? nStations- m_missedStations : -9999;
}

DECLARE_TOOL_FACTORY( MuonKalmanMatchTool )
