// $Id: IRichPhotonReconstruction.h,v 1.1 2004-06-17 12:00:46 cattanem Exp $
#ifndef RICHDETTOOLS_IRICHPHOTONRECONSTRUCTION_H 
#define RICHDETTOOLS_IRICHPHOTONRECONSTRUCTION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// CLHEP
class HepPoint3D;
class HepVector3D;

// Rich Kernel
class RichSmartID;

// Rich Utils
class RichGeomPhoton;
class RichTrackSegment;

/** @class IRichPhotonReconstruction IRichPhotonReconstruction.h RichDetTools/IRichPhotonReconstruction.h
 *  A tool to reconstruct photons given a RichTrackSegment and a detection 
 *  point.  The result is returned as a GeomPhoton.  
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

static const InterfaceID IID_IRichPhotonReconstruction ( "IRichPhotonReconstruction", 1, 0 );

class IRichPhotonReconstruction : public virtual IAlgTool {

public:

  static const InterfaceID& interfaceID() { return IID_IRichPhotonReconstruction; }

  /// Reconstructs the geometrical photon candidate for a given track segment
  /// and pixel smart ID
  virtual StatusCode reconstructPhoton( const RichTrackSegment&,
                                        const RichSmartID&,
                                        RichGeomPhoton& ) const = 0;

  /// Reconstructs the geometrical photon candidate for a given track segment
  /// and pixel global position
  virtual StatusCode reconstructPhoton( const RichTrackSegment&,
                                        const HepPoint3D&,
                                        RichGeomPhoton& ) const = 0;

};
#endif // RICHDETTOOLS_IRICHPHOTONRECONSTRUCTION_H
