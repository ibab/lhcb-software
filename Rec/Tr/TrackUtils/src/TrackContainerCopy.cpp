#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackContainerCopy.h"
#include "TrackInterfaces/ITrackSelector.h"

#include <boost/foreach.hpp>

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackContainerCopy )

TrackContainerCopy::TrackContainerCopy(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_selector("",this)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation  = TrackLocation::Velo );
  declareProperty( "outputLocation", m_outputLocation = TrackLocation::Default );
  declareProperty( "copyFailures",   m_copyFailures   = false );
  declareProperty( "Selector", m_selector );
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

  // retrieve the selector if it is set
  if ( !m_selector.empty() ) {
    sc = m_selector.retrieve() ;
    if(sc.isFailure())
      error() << "Failed to retrieve selector." << endmsg ;
  }
  
  return sc ;
}

StatusCode TrackContainerCopy::finalize()
{
  if( ! m_selector.empty() ) m_selector.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}

StatusCode TrackContainerCopy::execute(){

  LHCb::Track::Range inCont = get<LHCb::Track::Range>(m_inputLocation);
  Tracks* outCont = getOrCreate<Tracks,Tracks>(m_outputLocation);
  // loop 
  size_t naccepted(0) ;
  BOOST_FOREACH( const LHCb::Track* track, inCont ) {
    if ( ( !track->checkFlag(Track::Invalid) || m_copyFailures ) &&
	 (m_selector.empty() || m_selector->accept(*track)) ) {
      Track* aTrack = track->clone();
      outCont->insert(aTrack); 
      ++naccepted ;
    } // selected
  }
  //   counter("# tracks in")    += inCont->size() ;
  //   counter("# tracks out")   += outCont->size() ;
  //   counter("# tracks added") += naccepted ;
  
  return StatusCode::SUCCESS;
}

