// Include files:
// STL
#include <string>
#include <vector>
// GSL                   
#include "gsl/gsl_math.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
//from Kernel
#include "Kernel/DVAlgorithm.h"
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

DECLARE_ALGORITHM_FACTORY(TrackClusterFinder);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackClusterFinder::TrackClusterFinder(const std::string& name,
                                       ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
{
  declareProperty("InputTracks",
                  m_inputTracks = LHCb::TrackLocation::Default);
  declareProperty("InputPVs",
                  m_inputPVs = LHCb::RecVertexLocation::Primary);
  declareProperty("MinWeightSeed", m_minWeightSeed = 6);
  declareProperty("ConeExtFactor", m_coneExtFactor = 4.0);
  declareProperty("MaxDeltaPhi",   m_maxDeltaPhi   = 8.5);  // [Degree]
  declareProperty("MaxDeltaEta",   m_maxDeltaEta   = 0.16);
  declareProperty("Min1stJetPt",   m_min1stJetPt   = 3.0);  // [GeV]
  declareProperty("Min2ndJetPt",   m_min2ndJetPt   = 0.0);  // [GeV]
  declareProperty("JetMult",       m_jetMult = 1);
}

//=============================================================================
// Destructor
//=============================================================================
TrackClusterFinder::~TrackClusterFinder() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode TrackClusterFinder::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  m_maxDeltaPhi = m_maxDeltaPhi * (M_PI / 180.0);
  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode TrackClusterFinder::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  if(exist<Tracks>(m_inputTracks) && exist<RecVertices>(m_inputPVs)) {
    Tracks* inputTracks = get<Tracks>(m_inputTracks);
    // Prepare temporary tracks
    std::vector<tmpTrack> tmpTracks;
    int trackIndex = -1;
    for(Track::Container::const_iterator itr = inputTracks->begin();
        inputTracks->end() != itr; itr++) {
      if ((*itr)->states().empty()) continue ;
      trackIndex++;
      tmpTrack track;
      track.index = trackIndex;
      track.weight = 0;
      track.phi = (*itr)->phi();
      track.eta = (*itr)->pseudoRapidity();
      track.pt = (*itr)->pt() / Gaudi::Units::GeV;
      tmpTracks.push_back(track);
    }
    // Allocate weights
    if(tmpTracks.size() > 0) {   
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
    }
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


//=============================================================================
// Finalization
//=============================================================================
StatusCode TrackClusterFinder::finalize() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize();
}
