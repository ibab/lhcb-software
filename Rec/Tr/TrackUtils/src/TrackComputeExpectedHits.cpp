// track interfaces
#include "Event/Track.h"

#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackComputeExpectedHits.h"
#include "GaudiKernel/ToStream.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"
#include "Kernel/HitPattern.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackComputeExpectedHits )

TrackComputeExpectedHits::TrackComputeExpectedHits(const std::string& name,
                                                   ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_veloExpectation(0)
  , m_ttExpectation(0)
  , m_itExpectation(0)
  , m_otExpectation(0)
{
    
    declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
    
}

TrackComputeExpectedHits::~TrackComputeExpectedHits() {
  // destructor
}


StatusCode TrackComputeExpectedHits::initialize() {
  
  m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
  m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
  m_otExpectation = tool<IHitExpectation>("OTHitExpectation");
  
  return StatusCode::SUCCESS;
}

StatusCode TrackComputeExpectedHits::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);

  // loop 
  for (Tracks::iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {

    std::vector<LHCb::LHCbID > ids;
    LHCb::Track* aTrack = *(iterT);

    IVeloExpectation::VeloPattern velo[4];
    m_veloExpectation->expectedInfo(*aTrack,velo);
    m_ttExpectation->collect(*aTrack, ids);
    m_itExpectation->collect(*aTrack, ids);
    m_otExpectation->collect(*aTrack, ids);
    
    LHCb::HitPattern hitPattern = LHCb::HitPattern(ids);
    hitPattern.setVeloRA(velo[0]);
    hitPattern.setVeloRC(velo[1]); 
    hitPattern.setVeloPhiA(velo[2]);
    hitPattern.setVeloPhiC(velo[3]);
    

    //    (*iterT)->setExpectedHitPattern(hitPattern);

  } // iterT
   
  return StatusCode::SUCCESS;
}
