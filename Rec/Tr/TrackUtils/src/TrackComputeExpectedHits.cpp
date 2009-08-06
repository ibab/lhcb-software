
// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackComputeExpectedHits.h"
#include "GaudiKernel/ToStream.h"

#include "OTDet/DeOTDetector.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackComputeExpectedHits );

TrackComputeExpectedHits::TrackComputeExpectedHits(const std::string& name,
						   ISvcLocator* pSvcLocator):
    GaudiAlgorithm(name, pSvcLocator)
{
    
    declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
    
}

TrackComputeExpectedHits::~TrackComputeExpectedHits()
{
  // destructor
}


StatusCode TrackComputeExpectedHits::initialize() {
  
  m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
  m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
  m_otExpectation = tool<IHitExpectation>("OTHitExpectation");
  
  m_otDet = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  
  return StatusCode::SUCCESS;
}

StatusCode TrackComputeExpectedHits::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);

  // loop 
  for (Tracks::iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {

    std::vector<LHCb::LHCbID > ids;
    LHCb::Track* aTrack = *(iterT);

    std::bitset<23> velo[4];
    m_veloExpectation->expectedInfo(*aTrack, velo);
    m_ttExpectation->collect(*aTrack, ids);
    m_itExpectation->collect(*aTrack, ids);
    m_otExpectation->collect(*aTrack, ids);
    
    LHCb::HitPattern hitPattern = LHCb::HitPattern(ids, m_otDet);
    hitPattern.setVelo(velo);

    (*iterT)->setExpectedHitPattern(hitPattern);

  } // iterT
   
  return StatusCode::SUCCESS;
};

