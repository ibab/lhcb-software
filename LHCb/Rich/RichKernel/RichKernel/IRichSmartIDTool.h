// $Id: IRichSmartIDTool.h,v 1.1 2004-06-17 12:00:46 cattanem Exp $
#ifndef RICHDETTOOLS_IRICHSMARTIDTOOL_H
#define RICHDETTOOLS_IRICHSMARTIDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// CLHEP
class HepPoint3D;
class HepVector3D;

// Rich Kernel
class RichSmartID;


/** @class IRichSmartIDTool IRichSmartIDTool.h RichDetTools/IRichSmartIDTool.h
 *
 *  A tool to convert between smartID and detection points.  Also provides
 *  a list with all valid smartIDs.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

static const InterfaceID IID_IRichSmartIDTool( "IRichSmartIDTool", 1, 0 );

class IRichSmartIDTool : virtual public IAlgTool {

public:

  static const InterfaceID& interfaceID() { return IID_IRichSmartIDTool; }

  /// Returns the position of a SmartID in global coordinates
  virtual StatusCode globalPosition ( const RichSmartID&,
                                      HepPoint3D& ) const = 0;

  /// Returns the global position given a local position, rich and
  /// panel number
  virtual HepPoint3D globalPosition ( const HepPoint3D& localPoint,
                                      const Rich::DetectorType rich,
                                      const Rich::Side side ) const = 0;

  /// Returns the SmartID for a given global position
  virtual StatusCode smartID ( const HepPoint3D&,
                               RichSmartID& ) const = 0;

  /// Returns a list with all valid smartIDs
  virtual StatusCode readoutChannelList(std::vector<RichSmartID>&
                                        readoutChannels ) const = 0;

  /// Converts a global position to the coordinate system of the
  /// photodetector panel
  virtual HepPoint3D globalToPDPanel(const HepPoint3D& globalPoint) const = 0;
};
#endif // RICHDETTOOLS_IRICHSMARTIDTOOL_H
