#ifndef TRACKINTERFACES_IPATSEEDINGTOOL_H 
#define TRACKINTERFACES_IPATSEEDINGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Track.h"


// forward declaration
namespace LHCb {
  class State;
}

/** @class IPatSeedingTool IPatSeedingTool.h TrackInterfaces/IPatSeedingTool.h
 *  Interface to the forward pattern tool
 *
 *  @author Olivier Callot
 *  @date   2005-10-04
 */
class IPatSeedingTool : public extend_interfaces<IAlgTool> {
public: 

  // Return the interface ID
  DeclareInterfaceID( IPatSeedingTool, 1, 0 );

  virtual StatusCode performTracking(
		 LHCb::Tracks& output,
		 const LHCb::State* state = nullptr ) const = 0; 

  virtual StatusCode performTracking(
		 std::vector<LHCb::Track*>& outputTracks,
		 const LHCb::State* state = nullptr )  const= 0;

  virtual unsigned prepareHits () = 0; 
 
};
#endif // TRACKINTERFACES_IPATSEEDINGTOOL_H
