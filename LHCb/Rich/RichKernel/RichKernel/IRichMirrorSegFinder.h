// $Id: IRichMirrorSegFinder.h,v 1.2 2004-06-18 09:39:02 jonrob Exp $
#ifndef RICHKERNEL_IRICHMIRRORSEGFINDER_H
#define RICHKERNEL_IRICHMIRRORSEGFINDER_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// RichDet
class DeRichSphMirror;
class DeRichFlatMirror;

// CLHEP
class HepPoint3D;
class HepVector3D;

/** @class IRichMirrorSegFinder IRichMirrorSegFinder.h RichDetTools/IRichMirrorSegFinder.h
 *
 *  Interface to a tool to trace photons (or similar) from a point all the way to
 *  the photo detectors.
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */

static const InterfaceID IID_IRichMirrorSegFinder( "IRichMirrorSegFinder", 1, 0 );

class IRichMirrorSegFinder : public virtual IAlgTool {

public:

  static const InterfaceID& interfaceID() { return IID_IRichMirrorSegFinder; }

  /// Finds the spherical mirror
  virtual DeRichSphMirror* findSphMirror( const Rich::DetectorType rich,
                                          const Rich::Side side,
                                          const HepPoint3D & reflPoint ) const = 0;
  /// Finds the flat mirror
  virtual DeRichFlatMirror* findFlatMirror( const Rich::DetectorType rich,
                                            const Rich::Side side,
                                            const HepPoint3D & reflPoint ) const = 0;
};
#endif // RICHKERNEL_IRICHMIRRORSEGFINDER_H
