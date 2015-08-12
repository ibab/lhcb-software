#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif

// track interfaces
#include "Event/Track.h"

#include "TrackContainerCopy.h"
#include "TrackInterfaces/ITrackSelector.h"

#include <boost/foreach.hpp>

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackContainerCopy )

//=============================================================================
// Constructor
//=============================================================================
TrackContainerCopy::TrackContainerCopy(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_selector("",this)
{
  // constructor
  declareProperty( "inputLocation",  m_inputLocation  = "" ); // obsolete. Use inputLocations
  declareProperty( "inputLocations",  m_inputLocations  = { TrackLocation::Velo } );
  declareProperty( "outputLocation", m_outputLocation = TrackLocation::Default );
  declareProperty( "copyFailures",   m_copyFailures   = false );
  declareProperty( "Selector", m_selector );
}

//=============================================================================
// Destructor
//=============================================================================
TrackContainerCopy::~TrackContainerCopy()
{
  // destructor
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode TrackContainerCopy::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  // backwards compatibile. Retain for a while to allow for transistion...
  if ( !m_inputLocation.empty() )
  {
    Warning( "'inputLocation' Property is obsolete. Please change to use 'inputLocations' instead").ignore();
    m_inputLocations = { m_inputLocation };
  }

  // retrieve the selector if it is set
  if ( !m_selector.empty() ) {
    sc = m_selector.retrieve() ;
    if(sc.isFailure())
      error() << "Failed to retrieve selector." << endmsg ;
  }
  
  return sc ;
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackContainerCopy::finalize()
{
  if( ! m_selector.empty() ) m_selector.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}
//=============================================================================
// Execute
//=============================================================================
StatusCode TrackContainerCopy::execute(){

  
  Tracks* outCont = getOrCreate<Tracks,Tracks>(m_outputLocation);
  size_t naccepted(0) ;

  // loop 
  for( const std::string inputLoc : m_inputLocations){
    
    LHCb::Track::Range inCont = getIfExists<LHCb::Track::Range>(inputLoc);
    if( inCont.empty() ) continue;
    
    for( const LHCb::Track* track : inCont){
      
      if ( ( !track->checkFlag(Track::Invalid) || m_copyFailures ) &&
           (m_selector.empty() || m_selector->accept(*track)) ) {
        
        Track* aTrack = track->clone();
        outCont->insert(aTrack); 
        ++naccepted ;
      }
    }
  }
  
  /*
  BOOST_FOREACH( const LHCb::Track* track, inCont ) {
    if ( ( !track->checkFlag(Track::Invalid) || m_copyFailures ) &&
	 (m_selector.empty() || m_selector->accept(*track)) ) {
      Track* aTrack = track->clone();
      outCont->insert(aTrack); 
      ++naccepted ;
    } // selected
  }
  */
  //   counter("# tracks in")    += inCont->size() ;
  //   counter("# tracks out")   += outCont->size() ;
  //   counter("# tracks added") += naccepted ;
  
  return StatusCode::SUCCESS;
}

