// $Id: ITrajectoryProvider.h,v 1.3 2006-03-02 10:49:47 erodrigu Exp $
#ifndef TRACKINTERFACES_ITRAJECTORYPROVIDER_H 
#define TRACKINTERFACES_ITRAJECTORYPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbDefinitions
#include "Kernel/TrackTypes.h"

// Forward declarations
namespace LHCb {
  class LHCbID;
  class Trajectory;
  class Measurement;
  class State;
};

static const InterfaceID IID_ITrajectoryProvider ( "ITrajectoryProvider", 1, 0 );

/** @class ITrajectoryProvider ITrajectoryProvider.h TrackInterfaces/ITrajectoryProvider.h
 *  
 *  Interface for the trajectory provider tool
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-02-17
 */
class ITrajectoryProvider : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrajectoryProvider; }

  virtual StatusCode initialize() = 0;

  /// Return a "Measurement Trajectory" from a Measurement
  virtual LHCb::Trajectory* trajectory( LHCb::Measurement& meas ) = 0;

  /** Return a "Measurement Trajectory" from an LHCbID
   *  Note: the meaning of the offset input depends on the sub-detector type
   *  @return Pointer to the Trajectory created
   *  @param  id:     input LHCbID
   *  @param  offset: input offset
   */
  virtual LHCb::Trajectory* trajectory( const LHCb::LHCbID& id,
                                        double offset = 0 ) = 0;
  
  /// Return a "State Trajectory" from a State
  virtual LHCb::Trajectory* trajectory( const LHCb::State& state ) = 0;

  /// Return a "State Trajectory" from a State vector and a z-position
  virtual LHCb::Trajectory* trajectory( const Gaudi::TrackVector& stateVector,
                                        double z ) = 0;
  
protected:
  
private:

};
#endif // TRACKINTERFACES_ITRAJECTORYPROVIDER_H
