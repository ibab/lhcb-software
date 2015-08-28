#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackStateProvider.h"
#include <string>

// track interfaces
#include "Event/Track.h"
#include "Event/State.h"
#include "TrackKernel/TrackTraj.h"
#include <boost/foreach.hpp>

/** @class TrackStateAdder TrackStateAdder.h
 *
 * Adds missing states to tracks on DSTs.
 *
 *  @author W. Hulsbergen
 */

class TrackStateAdder: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackStateAdder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackStateAdder();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

private:
  std::string m_location;
  ToolHandle<ITrackStateProvider> m_stateprovider;
};

DECLARE_ALGORITHM_FACTORY( TrackStateAdder )

TrackStateAdder::TrackStateAdder(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator),
  m_stateprovider("TrackStateProvider")
{
  // constructor
  declareProperty( "TrackLocation" ,  m_location  = LHCb::TrackLocation::Default );
  declareProperty( "StateProvider", m_stateprovider );
}

TrackStateAdder::~TrackStateAdder()
{
  // destructor
}

StatusCode TrackStateAdder::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  // retrieve the StateProvider if it is set
  sc = m_stateprovider.retrieve() ;
  if(sc.isFailure())
    error() << "Failed to retrieve StateProvider." << endmsg ;
  return sc ;
}

StatusCode TrackStateAdder::finalize()
{
  m_stateprovider.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}

StatusCode TrackStateAdder::execute()
{
  //
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_location);
  BOOST_FOREACH( LHCb::Track* track, *tracks ) {
    // OK, we use a hack: We actually let TrackStateProvider determine
    // which states. To access those states, we take its
    // tracktrajectory. 
    const LHCb::TrackTraj* tracktraj = m_stateprovider->trajectory(*track) ;
    if( tracktraj ) {
      //size_t numadded(0) ;
      // to make this reasonably fast, first make a list of states that we already have
      std::set< LHCb::State::Location > locations ;
      BOOST_FOREACH( const LHCb::State* state, track->states() ) {
	if( state->location() != LHCb::State::LocationUnknown)
	  locations.insert( state->location() ) ;
      }
      BOOST_FOREACH( const LHCb::State* state, tracktraj->refStates() ) {
	if( state->location() != LHCb::State::LocationUnknown &&
	    locations.find( state->location() ) == locations.end() ) {
	  track->addToStates( *state ) ;
	  //info() << "Added state of type " << state->location() << " to track of type "
	  //<< track->type() << endmsg ;
	  locations.insert( state->location() ) ;
	  //++numadded ;
	}
      }
      //if(numadded>0)
      //info() << "Added " << numadded << " states to track of type " << track->type() << endmsg ;
    }
  }
  return StatusCode::SUCCESS;
}

