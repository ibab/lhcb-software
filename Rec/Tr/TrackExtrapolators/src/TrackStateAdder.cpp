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

  StatusCode initialize() override;
  StatusCode finalize() override;
  StatusCode execute() override;

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

StatusCode TrackStateAdder::initialize()
{
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
  for( LHCb::Track* track: *get<LHCb::Tracks>(m_location) ) {
    // OK, we use a hack: We actually let TrackStateProvider determine
    // which states. To access those states, we take its
    // tracktrajectory. 
    const auto* tracktraj = m_stateprovider->trajectory(*track) ;
    if( tracktraj ) {
      // to make this reasonably fast, first make a list of states that we already have
      // insert 'unknown' explicitly a-priori, so that we don't have to explicitly skip it..
      std::set< LHCb::State::Location > locations{ LHCb::State::LocationUnknown };
      std::transform( track->states().begin(), track->states().end(),
                      std::inserter( locations, locations.end() ),
                      [](const LHCb::State* s) { return s->location(); } );
      for( const LHCb::State* state: tracktraj->refStates() ) {
        if( locations.find( state->location() ) == locations.end() ) {
          track->addToStates( *state ) ;
          //info() << "Added state of type " << state->location() << " to track of type "
          //<< track->type() << endmsg ;
          locations.insert( state->location() ) ;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
