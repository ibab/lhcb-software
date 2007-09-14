// $Id: TTHitExpectation.cpp,v 1.2 2007-09-14 12:04:18 mneedham Exp $

// from GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "TsaKernel/Line3D.h"
#include "LHCbMath/GeomFun.h"
#include "STDet/DeSTDetector.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "TTHitExpectation.h"

#include <algorithm>

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TTHitExpectation );
//=============================================================================
// 
//=============================================================================
TTHitExpectation::TTHitExpectation(const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent):
  GaudiTool(type, name, parent)
{ 
  // constructer
  declareInterface<IHitExpectation>(this);
};

//=============================================================================
// 
//=============================================================================
TTHitExpectation::~TTHitExpectation(){
  // destructer
}

//=============================================================================
// 
//=============================================================================
StatusCode TTHitExpectation::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

 
  m_extrapolator = tool<ITrackExtrapolator>("TrackFastParabolicExtrapolator");
  m_ttDet = getDet<DeSTDetector>(DeSTDetLocation::location("TT"));

  if (m_ttDet->nStation() != 2u){
    return Error("2 stations needed in TT",StatusCode::FAILURE);
  }
 
  const DeSTDetector::Stations& ttStations = m_ttDet->stations();
  m_zTTa = ttStations[0]->globalCentre().z();
  m_zTTb = ttStations[1]->globalCentre().z();
 
  return StatusCode::SUCCESS;
}


IHitExpectation::Info TTHitExpectation::expectation(const LHCb::Track& aTrack) const{

  IHitExpectation::Info info;
  info.likelihood = 0.0;
  info.nFound = 0;
  info.nExpected = nExpected(aTrack);
  return info;
}

unsigned int TTHitExpectation::nExpected(const LHCb::Track& aTrack) const{

  // make a line at TTa and TTb
  const State& TTaState = aTrack.closestState(m_zTTa);
  StateVector stateVectorTTa(TTaState.position(), TTaState.slopes());

  const State& TTbState = aTrack.closestState(m_zTTb);
  StateVector stateVectorTTb(TTbState.position(), TTbState.slopes());

  // determine which modules should be hit
  std::vector<STChannelID> expectedHitsA; expectedHitsA.reserve(4);
  std::vector<STChannelID> expectedHitsB; expectedHitsB.reserve(4);
  collectHits(expectedHitsA, stateVectorTTa, 1);
  collectHits(expectedHitsB, stateVectorTTb, 2);

  return expectedHitsA.size() + expectedHitsB.size();  
}

void TTHitExpectation::collectHits(std::vector<LHCb::STChannelID>& chan, 
                                  LHCb::StateVector stateVec, const unsigned int station ) const{


  // loop over the sectors
  const DeSTDetector::Sectors& sectorVector = m_ttDet->sectors();
  DeSTDetector::Sectors::const_iterator iterS = sectorVector.begin();
  for (; iterS != sectorVector.end(); ++iterS){
    // propagate to z of sector
    m_extrapolator->propagate(stateVec,(*iterS)->globalCentre().z());    
    STChannelID elemID = (*iterS)->elementID();
    if (elemID.station() == station){
      if (insideSector(*iterS,stateVec) == true){
        chan.push_back(elemID);
      }
    }  // correct station
  } // iterS

}



