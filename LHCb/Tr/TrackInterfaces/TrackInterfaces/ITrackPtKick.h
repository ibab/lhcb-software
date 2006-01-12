#ifndef TRACKINTERFACES_ITRACKPTKICK_H 
#define TRACKINTERFACES_ITRACKPTKICK_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
 class State;
};

static const InterfaceID IID_ITrackPtKick ( "ITrackPtKick", 1, 0 );

/** @class ITrackPtKick ITrackPtKick.h TrackInterfaces/ITrackPtKick.h
 *  
 *
 *  @author Eduardo Rodrigues, Jose Hernando
 *  @date   2005-05-20
 */
class ITrackPtKick : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackPtKick; }

  // Estimate the momentum P of a State
  virtual StatusCode calculate( LHCb::State* State ) const = 0;

protected:

private:

};
#endif // TRACKINTERFACES_ITRACKPTKICK_H
