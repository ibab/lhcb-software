// $Id: TrackContainerCopy.cpp,v 1.2 2006-07-27 12:01:54 cattanem Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackContainerCopy.h"
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackContainerCopy );

TrackContainerCopy::TrackContainerCopy(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation  = TrackLocation::Velo );
  declareProperty( "outputLocation", m_outputLocation = TrackLocation::Default );
  declareProperty( "copyFailures",   m_copyFailures   = false );
}

TrackContainerCopy::~TrackContainerCopy()
{
  // destructor
}


StatusCode TrackContainerCopy::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);
  Tracks* outCont = getOrCreate<Tracks,Tracks>(m_outputLocation);

  // loop 
  for (Tracks::const_iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {
    if ( !(*iterT)->checkFlag(Track::Invalid) || m_copyFailures ) 
    {
      Track* aTrack = (*iterT)->clone();
      outCont->insert(aTrack); 
    }
  } // iterT
   
  return StatusCode::SUCCESS;
};

