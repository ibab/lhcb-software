#ifndef TRACKINTERFACES_ITRACKMOMENTUMESTIMATE_H 
#define TRACKINTERFACES_ITRACKMOMENTUMESTIMATE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
 class State;
};

static const InterfaceID IID_ITrackMomentumEstimate ( "ITrackMomentumEstimate", 1, 0 );

/** @class ITrackMomentumEstimate ITrackMomentumEstimate.h TrackInterfaces/ITrackMomentumEstimate.h
 *  
 *
 *  @author Stephanie Hansmann-Menzemer
 *  @date   2007-10-30
 */
class ITrackMomentumEstimate : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackMomentumEstimate; }

  // Estimate the momentum P of a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* TState, double& qOverP, 
				double& sigmaQOverP, bool cubical = 0) const = 0; 

  // Estimate the momentum P of a velo State and a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* veloState, const LHCb::State* tState,
				double& qOverP, double& sigmaQOverP,
				bool cubical = 0) const = 0;
  

};
#endif // TRACKINTERFACES_ITRACKMOMENTUMESTIMATE_H
