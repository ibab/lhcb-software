
// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackEraseExtraInfo.h"
#include "GaudiKernel/ToStream.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"



using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackEraseExtraInfo )

TrackEraseExtraInfo::TrackEraseExtraInfo(const std::string& name,
					 ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  
  declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
    
}

TrackEraseExtraInfo::~TrackEraseExtraInfo()
{
  // destructor
}


StatusCode TrackEraseExtraInfo::initialize() {
  
  return StatusCode::SUCCESS;
}

StatusCode TrackEraseExtraInfo::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);

  // loop 
  for (Tracks::iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {

    (*iterT)->eraseInfo(LHCb::Track::PatQuality);
    (*iterT)->eraseInfo(LHCb::Track::Cand1stQPat);
    (*iterT)->eraseInfo(LHCb::Track::Cand2ndQPat);
    (*iterT)->eraseInfo(LHCb::Track::Cand1stChi2Mat);
    (*iterT)->eraseInfo(LHCb::Track::Cand2ndChi2Mat); 
    (*iterT)->eraseInfo(LHCb::Track::MatchChi2);
    (*iterT)->eraseInfo(LHCb::Track::TsaLikelihood); 
    (*iterT)->eraseInfo(LHCb::Track::nPRVeloRZExpect);
    (*iterT)->eraseInfo(LHCb::Track::nPRVelo3DExpect);
   
  }
   
  return StatusCode::SUCCESS;
}

