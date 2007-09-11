// $Id: TrackAddLikelihood.cpp,v 1.1 2007-09-11 14:49:45 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackAddLikelihood.h"
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackAddLikelihood );

TrackAddLikelihood::TrackAddLikelihood(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
 
 declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
 declareProperty("LikelihoodTool", m_likelihoodToolName = "TrackLikelihood");
}

TrackAddLikelihood::~TrackAddLikelihood()
{
  // destructor
}


StatusCode TrackAddLikelihood::initialize() {
  m_likelihood = tool<ITrackManipulator>(m_likelihoodToolName,"likelihood", this );
  return StatusCode::SUCCESS;
}

StatusCode TrackAddLikelihood::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);

  // loop 
  for (Tracks::iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {
    m_likelihood->execute(**iterT).ignore();
  } // iterT
   
  return StatusCode::SUCCESS;
};

