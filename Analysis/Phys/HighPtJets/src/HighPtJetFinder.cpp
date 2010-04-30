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
#include "HighPtJetFinder.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HighPtJetFinder
//
// 2010-04-03 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(HighPtJetFinder);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HighPtJetFinder::HighPtJetFinder(const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : DVAlgorithm(name,pSvcLocator)
{
  declareProperty("InputTracks",
                  m_inputTracks = LHCb::TrackLocation::Default);
  declareProperty("InputPVs",
                  m_inputPVs = LHCb::RecVertexLocation::Primary);
  declareProperty("MinWeight", m_minWeight = 6);
  declareProperty("MaxDeltaPhi", m_maxDeltaPhi = 7.0);  // [Degree]
  declareProperty("MaxDeltaEta", m_maxDeltaEta = 0.12);
  declareProperty("Min1stJetPt", m_min1stJetPt = 0.0);  // [GeV]
  declareProperty("Min2ndJetPt", m_min2ndJetPt = 0.0);  // [GeV]
  declareProperty("JetMult", m_jetMult = 2);
}

//=============================================================================
// Destructor
//=============================================================================
HighPtJetFinder::~HighPtJetFinder() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HighPtJetFinder::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Execution
//=============================================================================
StatusCode HighPtJetFinder::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  if(exist<Tracks>(m_inputTracks) && exist<RecVertices>(m_inputPVs)) {
    Tracks* inputTracks = get<Tracks>(m_inputTracks);
    // Prepare temporary tracks
    std::vector<tmpTrack> tmpTracks;
    int trackIndex = -1;
    for(Track::Container::const_iterator itr = inputTracks->begin();
        inputTracks->end() != itr; itr++) {
      trackIndex++;
      double phiDeg = (*itr)->phi() * (180 / M_PI);
      if(phiDeg > 180.0) phiDeg = phiDeg - 360.0;
      tmpTrack track;
      track.index = trackIndex;
      track.weight = 0;
      track.phi = phiDeg;
      track.eta = (*itr)->pseudoRapidity();
      track.pt = (*itr)->pt() / Gaudi::Units::GeV;
      tmpTracks.push_back(track);
    }
    // Allocate weights
    for(std::vector<tmpTrack>::iterator id1 = tmpTracks.begin(); 
        id1 != tmpTracks.end() - 1; id1++) {
      tmpTrack trk1 = *id1;
      for(std::vector<tmpTrack>::iterator id2 = id1 + 1;
          id2 != tmpTracks.end(); id2++) {
        tmpTrack trk2 = *id2;
        double diffEta = fabs(trk2.eta - trk1.eta);
        double diffPhi = deltaPhi(trk2.phi,trk1.phi);
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
    while(multPeak >= m_minWeight) {
      tmpJet jet;
      jet.tracks.push_back(tmpTracks[cIndex]);
      tmpTracks[cIndex].weight = 0;
      for(std::vector<tmpTrack>::iterator id = tmpTracks.begin();
          id != tmpTracks.end(); id++) {
        tmpTrack trk = *id;
        if((deltaPhi(trk.phi,tmpTracks[cIndex].phi) < 2.0 * m_maxDeltaPhi) &&
           (fabs(trk.eta - tmpTracks[cIndex].eta) < 2.0 * m_maxDeltaEta)) {
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

  return StatusCode::SUCCESS;
}


//============================================================================
// Calculate delta phi
//============================================================================
double HighPtJetFinder::deltaPhi(double phi1, double phi2)
{
  double diff1 = fabs(phi1 - phi2);
  double diff2 = 99999.;
  if(phi1 > phi2) {
    diff2 = fabs(phi1 - (phi2 + 360.0));
  } else {
    diff2 = fabs(phi2 - (phi1 + 360.0));      
  }
  double diffPhi = GSL_MIN_DBL(diff1,diff2);
  return diffPhi;
}


//============================================================================
// Get peak
//============================================================================
void HighPtJetFinder::getPeak(std::vector<tmpTrack> tmpTracks, int& index, 
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
StatusCode HighPtJetFinder::finalize() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize();
}
