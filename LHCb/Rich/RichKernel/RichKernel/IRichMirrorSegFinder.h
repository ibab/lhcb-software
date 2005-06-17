
//-----------------------------------------------------------------------------
/** @file IRichMirrorSegFinder.h
 *
 *  Header file for tool interface : IRichMirrorSegFinder
 *
 *  $Id: IRichMirrorSegFinder.h,v 1.8 2005-06-17 14:26:46 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHMIRRORSEGFINDER_H
#define RICHKERNEL_IRICHMIRRORSEGFINDER_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"

// RichDet
class DeRichSphMirror;
class DeRichFlatMirror;


/// Static Interface Identification
static const InterfaceID IID_IRichMirrorSegFinder( "IRichMirrorSegFinder", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichMirrorSegFinder IRichMirrorSegFinder.h RichKernel/IRichMirrorSegFinder.h
 *
 *  Interface to a tool to find the appropriate mirror segment for a given reflection point
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

class IRichMirrorSegFinder : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichMirrorSegFinder; }

  /** Locates the spherical mirror Segment given a reflection point,
   *  RICH identifier and panel
   *
   *  @param rich       The RICH detector
   *  @param side       The RICH HPD panel side
   *  @param reflPoint  The reflection point on the spherical mirror
   *
   *  @return Const pointer to DeRichSphMirror object for the associated mirror segment
   */
  virtual const DeRichSphMirror*
  findSphMirror( const Rich::DetectorType rich,
                 const Rich::Side side,
                 const HepPoint3D & reflPoint ) const = 0;

  /** Locates the flat mirror Segment given a reflection point,
   *  RICH identifier and panel
   *
   *  @param rich       The RICH detector
   *  @param side       The RICH HPD panel side
   *  @param reflPoint  The reflection point on the flat mirror
   *
   *  @return Const pointer to DeRichSphMirror object for the associated mirror segment
   */
  virtual const DeRichFlatMirror*
  findFlatMirror( const Rich::DetectorType rich,
                  const Rich::Side side,
                  const HepPoint3D & reflPoint ) const = 0;

};

#endif // RICHKERNEL_IRICHMIRRORSEGFINDER_H
