#ifndef TRACKINTERFACES_IFASTMOMENTUMESTIMATE_H 
#define TRACKINTERFACES_IFASTMOMENTUMESTIMATE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
 class State;
};

static const InterfaceID IID_IFastMomentumEstimate ( "IFastMomentumEstimate", 1, 0 );

/** @class IFastMomentumEstimate IFastMomentumEstimate.h TrackInterfaces/IFastMomentumEstimate.h
 *  
 *
 *  @author Stephanie Hansmann-Menzemer
 *  @date   2007-10-30
 */
class IFastMomentumEstimate : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IFastMomentumEstimate; }

  // Estimate the momentum P of a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* TState, double& qOverP, 
				double& sigmaQOverP, bool cubical ) const = 0; 

  // Estimate the momentum P of a velo State and a State in T at ZAtMidT
  virtual StatusCode calculate( const LHCb::State* veloState, const LHCb::State* tState,
				double& qOverP, double& sigmaQOverP,
				bool cubical) const = 0;
  

};
#endif // TRACKINTERFACES_IFASTMOMENTUMESTIMATE_H
