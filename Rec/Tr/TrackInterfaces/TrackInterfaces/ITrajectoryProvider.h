#ifndef TRACKINTERFACES_ITRAJECTORYPROVIDER_H 
#define TRACKINTERFACES_ITRAJECTORYPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from TrackEvent
#include "Event/TrackTypes.h"

#include <memory>

// Forward declarations
namespace LHCb {
  class LHCbID;
  class Trajectory;
  class Measurement;
  class State;
}


/** @class ITrajectoryProvider ITrajectoryProvider.h TrackInterfaces/ITrajectoryProvider.h
 *  
 *  Interface for the trajectory provider tool
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-02-17
 */
class ITrajectoryProvider : public extend_interfaces<IAlgTool> {
public: 

  DeclareInterfaceID( ITrajectoryProvider, 2, 0 );

  /// Return a "Measurement Trajectory" from a Measurement
  virtual const LHCb::Trajectory* trajectory( const LHCb::Measurement& meas ) const = 0;

  /** Return a "Measurement Trajectory" from an LHCbID
   *  Note: the meaning of the offset input depends on the sub-detector type
   *  @return Pointer to the Trajectory created
   *  @param  id:     input LHCbID
   *  @param  offset: input offset
   */
  virtual std::unique_ptr<LHCb::Trajectory> trajectory( const LHCb::LHCbID& id,
                                                        double offset = 0 ) const = 0;
  
  /// Return a "State Trajectory" from a State
  virtual std::unique_ptr<LHCb::Trajectory> trajectory( const LHCb::State& state ) const = 0;
  
  /// Return a "State Trajectory" from a State vector and a z-position
  virtual std::unique_ptr<LHCb::Trajectory> trajectory( const Gaudi::TrackVector& stateVector,
                                                        double z ) const = 0;
  
};
#endif // TRACKINTERFACES_ITRAJECTORYPROVIDER_H
