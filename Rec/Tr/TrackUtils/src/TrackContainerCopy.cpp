// $Id: TrackContainerCopy.cpp,v 1.4 2008-12-02 14:46:38 wouter Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackContainerCopy.h"
#include "TrackInterfaces/ITrackSelector.h"
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackContainerCopy );

TrackContainerCopy::TrackContainerCopy(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_selector(0)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation  = TrackLocation::Velo );
  declareProperty( "outputLocation", m_outputLocation = TrackLocation::Default );
  declareProperty( "copyFailures",   m_copyFailures   = false );
  declareProperty( "selectorName", m_selectorName = "TrackSelector");
}

TrackContainerCopy::~TrackContainerCopy()
{
  // destructor
}

StatusCode TrackContainerCopy::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  // da selector --- by default takes all tracks
  if(  m_selectorName != "" && m_selectorName != "None" ) 
    m_selector = tool<ITrackSelector>(m_selectorName, "Selector", this);

  return StatusCode::SUCCESS;
}

StatusCode TrackContainerCopy::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);
  Tracks* outCont = getOrCreate<Tracks,Tracks>(m_outputLocation);

  // loop 
  size_t naccepted(0) ;
  for (Tracks::const_iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {
    if ( ( !(*iterT)->checkFlag(Track::Invalid) || m_copyFailures ) &&
	 ( !m_selector || m_selector->accept(**iterT) ) ) {
      Track* aTrack = (*iterT)->clone();
      outCont->insert(aTrack); 
      ++naccepted ;
    } // selected
  }
  //   counter("# tracks in")    += inCont->size() ;
  //   counter("# tracks out")   += outCont->size() ;
  //   counter("# tracks added") += naccepted ;
  
  return StatusCode::SUCCESS;
};

