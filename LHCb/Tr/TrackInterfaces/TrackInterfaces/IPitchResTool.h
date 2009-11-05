#ifndef TRACKINTERFACES_IPITCHRESTOOL_H
#define TRACKINTERFACES_IPITCHRESTOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
  class Track;
  class OTChannelID;
};

static const InterfaceID IID_IPitchResTool ( "IPitchResTool", 1, 0 );

/** @class IPitchResTool IPitchResTool.h
 *  
 *  Interface for the pitch residual  tool for the OT
 *
 *  @author Johan Blouw 
 *  @author Manuel Tobias Schiller
 *  @date   2009-09-04
 */
class IPitchResTool : virtual public IAlgTool {
public:
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPitchResTool; }

  // calculate the pitch residuals for each of the OT layers.
  // It is based on the fact that each OT layer actually contains
  // 2 monolayers at a known and well-defined position from each other.
  // Using the track slope, one can calculate, independent from e.g.
  // misalignments, a residual. The 2nd mono-layer is staggered with
  // 0.5 pitch wrt the first monolayer. That means that the sum of the
  // 2 residuals is bound by the distance between the two wires projected 
  // onto the track.
  virtual std::vector<std::pair<LHCb::OTChannelID, double> > 
    calcPitchResiduals(const LHCb::Track* track) const = 0;

};

//==============================================================================
//   end of class
//==============================================================================

#endif // TRACKINTERFACES_IPITCHRESTOOL_H
