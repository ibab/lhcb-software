// $Id: TrackContainerCopy.cpp,v 1.1 2006-06-13 12:24:14 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackContainerCopy.h"
using namespace LHCb;

// Needed for the creation of TrackContainerCopy objects.
static const AlgFactory<TrackContainerCopy> s_factory;
const IAlgFactory& TrackContainerCopyFactory = s_factory;

TrackContainerCopy::TrackContainerCopy(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
 this->declareProperty("inputLocation", m_inputLocation = TrackLocation::Velo);
 this->declareProperty("outputLocation", m_outputLocation = TrackLocation::Default);
}

TrackContainerCopy::~TrackContainerCopy()
{
  // destructor
}


StatusCode TrackContainerCopy::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);
  Tracks* outCont = getOrCreate<Tracks,Tracks>(m_outputLocation);

  // loop 
  for (Tracks::const_iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT){
    Track* aTrack = (*iterT)->clone();
    outCont->insert(aTrack); 
  } // iterT
   
  return StatusCode::SUCCESS;
};

