// $Id: IRichMirrorSegFinder.h,v 1.3 2004-06-29 19:27:30 jonrob Exp $
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

/** @class IRichMirrorSegFinder IRichMirrorSegFinder.h RichKernel/IRichMirrorSegFinder.h
 *
 *  Interface to a tool to find the appropriate mirror segment for a given reflection point
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
