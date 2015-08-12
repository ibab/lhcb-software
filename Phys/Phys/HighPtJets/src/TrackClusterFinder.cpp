// Include files:
// STL
#include <string>
#include <vector>
// GSL                   
#include "gsl/gsl_math.h"
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// from Event
#include "Event/Track.h"
#include "Event/RecVertex.h"
// GSL                   
#include "gsl/gsl_math.h"
// Local
#include "TrackClusterFinder.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackClusterFinder
//
// 2010-04-03 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(TrackClusterFinder)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackClusterFinder::TrackClusterFinder(const std::string& name,
                                       ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm(name,pSvcLocator)
{
  declareProperty("InputTracks",   m_inputTracks);
  declareProperty("InputPVs",
                  m_inputPVsName = LHCb::RecVertexLocation::Primary);
  declareProperty("MinWeightSeed", m_minWeightSeed = 6);
  declareProperty("ConeExtFactor", m_coneExtFactor = 4.0);
  declareProperty("MaxDeltaPhi",   m_maxDeltaPhi   = 4.0);  // [Degree]
  declareProperty("MaxDeltaEta",   m_maxDeltaEta   = 0.15);
  declareProperty("Min1stJetPt",   m_min1stJetPt   = 10.0); // [GeV]
  declareProperty("Min2ndJetPt",   m_min2ndJetPt   = 7.0);  // [GeV]
  declareProperty("JetMult",       m_jetMult       = 2);
  declareProperty("MaxRPV",        m_maxRPV        = 1.0);  // [mm]
  declareProperty("NrPVs",         m_nrPVs         = 1);
  declareProperty("MinSumEt",      m_minSumEt      = 0.0);  // [GeV]
}

//=============================================================================
// Destructor
//=============================================================================
TrackClusterFinder::~TrackClusterFinder() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode TrackClusterFinder::initialize() {
  StatusCode sc = DaVinciAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  m_maxDeltaPhi = m_maxDeltaPhi * (M_PI / 180.0);
  // Input tracks
  if(m_inputTracks.size() == 0) {
    m_inputTracks.push_back(LHCb::TrackLocation::Default);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode TrackClusterFinder::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  if(!exist<RecVertices>(m_inputPVsName)) return StatusCode::SUCCESS;
  m_inputPVs = get<LHCb::RecVertices>(m_inputPVsName);
  if(!(m_inputPVs->size())) return StatusCode::SUCCESS;
  if(m_inputPVs->size() > m_nrPVs) return StatusCode::SUCCESS; 
  // Retrieving forward tracks
  double sumEtSin = 0.0;
  double sumEtCos = 0.0;
  std::vector<const LHCb::Track*> forwardTracks;
  std::vector<std::string>::iterator itTrName;
  for(itTrName = m_inputTracks.begin(); itTrName != m_inputTracks.end();
      itTrName++) {
    LHCb::Tracks* inputTracks = get<LHCb::Tracks>(*itTrName);
    for(LHCb::Tracks::iterator itr = inputTracks->begin(); 
        inputTracks->end() != itr; itr++) {
      if((*itr)->p()) {
        if((*itr)->pt() / Gaudi::Units::GeV < 0.1) continue;
        forwardTracks.push_back(*itr);
        // Calculate refined sum Et
        double trkE   = (*itr)->p() / Gaudi::Units::GeV;
        double trkPhi = (*itr)->phi()* 57.29;
        double trkEta = (*itr)->pseudoRapidity();
        sumEtSin += (trkE * sin(trkPhi)) / cosh(trkEta);
        sumEtCos += (trkE * cos(trkPhi)) / cosh(trkEta);
      } 
    }
  }
  if(!forwardTracks.size()) return StatusCode::SUCCESS;
  // Refined sum Et
  double refinedSumEt = sqrt(sumEtSin * sumEtSin + sumEtCos * sumEtCos);
  if(refinedSumEt < m_minSumEt) return StatusCode::SUCCESS;
  // Prepare temporary tracks
  std::vector<tmpTrack> tmpTracks;
  int trackIndex = -1;
  for(std::vector<const LHCb::Track*>::iterator itr = forwardTracks.begin();
      forwardTracks.end() != itr; itr++) {
    if((*itr)->states().empty()) continue;
    if((*itr)->chi2PerDoF() > 5.0) continue;
    if((*itr)->pt() / Gaudi::Units::GeV < 0.1) continue;
    trackIndex++;
    tmpTrack track;
    track.index = trackIndex;
    track.weight = 0;
    track.phi = (*itr)->phi();
    track.eta = (*itr)->pseudoRapidity();
    track.pt  = (*itr)->pt() / Gaudi::Units::GeV;
    tmpTracks.push_back(track);
  }
  // Allocate weights
  if(!(tmpTracks.size())) return StatusCode::SUCCESS;   
  for(std::vector<tmpTrack>::iterator id1 = tmpTracks.begin(); 
      id1 != tmpTracks.end() - 1; id1++) {
    tmpTrack trk1 = *id1;
    for(std::vector<tmpTrack>::iterator id2 = id1 + 1;
        id2 != tmpTracks.end(); id2++) {
      tmpTrack trk2 = *id2;
      double diffEta = fabs(trk2.eta - trk1.eta);
      double diffPhi = fabs(trk2.phi - trk1.phi);
      if(diffPhi > M_PI) diffPhi = 2.0 * M_PI - diffPhi;
      if((diffEta < m_maxDeltaEta) && (diffPhi < m_maxDeltaPhi)) {
        tmpTracks[trk1.index].weight += 1;
        tmpTracks[trk2.index].weight += 1;
      }
    }
  }
  // Find highest multiplicity peak
  int cIndex, multPeak;
  getPeak(tmpTracks,cIndex,multPeak);
  // Assign tracks to jets
  std::vector<tmpJet> tmpJets;
  while(multPeak >= m_minWeightSeed) {
    tmpJet jet;
    jet.tracks.push_back(tmpTracks[cIndex]);
    tmpTracks[cIndex].weight = 0;
    for(std::vector<tmpTrack>::iterator id = tmpTracks.begin();
        id != tmpTracks.end(); id++) {
      tmpTrack trk = *id;
      double dEta = fabs(trk.eta - tmpTracks[cIndex].eta);
      double dPhi = fabs(trk.phi - tmpTracks[cIndex].phi);
      if(dPhi > M_PI) dPhi = 2.0 * M_PI - dPhi;
      if((dPhi < m_coneExtFactor * m_maxDeltaPhi) &&
         (dEta < m_coneExtFactor * m_maxDeltaEta)) {
        jet.tracks.push_back(trk);
        tmpTracks[trk.index].weight = 0;
      }
    }
    tmpJets.push_back(jet);
    getPeak(tmpTracks,cIndex,multPeak);
  }
  // Criteria to accept the event
  if(tmpJets.size() >= m_jetMult) {
    bool highPt = false;
    unsigned int twoHighPtJets = 0;
    for(std::vector<tmpJet>::iterator ij = tmpJets.begin(); 
        ij != tmpJets.end(); ij++) {
      double sumPt = 0.0;
      for(std::vector<tmpTrack>::iterator id = (*ij).tracks.begin();
          id != (*ij).tracks.end(); id++) {
        sumPt = sumPt + (*id).pt;
      }
      if(sumPt > m_min2ndJetPt) {
        twoHighPtJets++;
        if(sumPt > m_min1stJetPt) highPt = true;
      }
    }
    if(highPt && (twoHighPtJets >= m_jetMult)) setFilterPassed(true);
  }
  
  return StatusCode::SUCCESS;
}


//============================================================================
// Get peak
//============================================================================
void TrackClusterFinder::getPeak(std::vector<tmpTrack> tmpTracks, int& index, 
                                 int& multPeak)
{
  index = -1;
  multPeak = 0;
  for(std::vector<tmpTrack>::iterator id = tmpTracks.begin(); 
      id != tmpTracks.end(); id++) {
    tmpTrack trk = *id;
    if(trk.weight > multPeak) {
      multPeak = trk.weight;
      index = trk.index;
    }  
  }
}
