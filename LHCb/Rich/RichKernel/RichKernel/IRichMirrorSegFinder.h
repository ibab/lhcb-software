// $Id: IRichMirrorSegFinder.h,v 1.1 2004-06-17 12:00:46 cattanem Exp $
#ifndef RICHDETTOOLS_IRICHMIRRORSEGFINDER_H 
#define RICHDETTOOLS_IRICHMIRRORSEGFINDER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// from RichDet
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichFlatMirror.h"

// CLHEP
class HepPoint3D;
class HepVector3D;


/** @class IRichMirrorSegFinder IRichMirrorSegFinder.h RichDetTools/IRichMirrorSegFinder.h
 *  A tool to trace photons (or similar) from a point all the way to
 *  the photo detectors.
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */

static const InterfaceID IID_IRichMirrorSegFinder( "IRichMirrorSegFinder", 
                                                   1, 0 );

class IRichMirrorSegFinder : public virtual IAlgTool {

public:

  static const InterfaceID& interfaceID() { return IID_IRichMirrorSegFinder; }

  /// Finds the spherical mirror
  virtual DeRichSphMirror* findSphMirror(const Rich::DetectorType rich,
                                         const Rich::Side side,
                                         const HepPoint3D& reflPoint) const=0;
  /// Finds the flat mirror
  virtual DeRichFlatMirror* findFlatMirror(const Rich::DetectorType rich,
                                           const Rich::Side side,
                                           const HepPoint3D& reflPoint)const=0;
};
#endif // RICHDETTOOLS_IRICHMIRRORSEGFINDER_H
