// $Id: IRichPhotonReconstruction.h,v 1.3 2004-07-15 15:36:53 jonrob Exp $
#ifndef RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H
#define RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H 1

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

/** @class IRichPhotonReconstruction IRichPhotonReconstruction.h RichKernel/IRichPhotonReconstruction.h
 *
 *  A tool to reconstruct photons given a RichTrackSegment and a detection
 *  point.  The result is returned as a RichGeomPhoton object.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

static const InterfaceID IID_IRichPhotonReconstruction ( "IRichPhotonReconstruction", 1, 0 );

class IRichPhotonReconstruction : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichPhotonReconstruction; }

  /** Reconstructs the geometrical photon candidate for a given RichTrackSegment
   *  and RichSmartID channel identifier
   *
   *  @param trSeg    The RichTrackSegment to reconstruct the photon against
   *  @param smartID  The hit position to use, encoded as a RichSmartID
   *  @param gPhoton  The reconstructed photon
   *
   *  @return Status of the reconstruction
   *  @retval StatusCode::SUCCESS Photon reconstruction was successful
   *  @retval StatusCode::FAILURE Photon reconstruction was no possible with given segment and hit position
   */
  virtual StatusCode 
  reconstructPhoton ( const RichTrackSegment& trSeg, 
                      const RichSmartID& smartID, 
                      RichGeomPhoton& gPhoton ) const = 0;

  /** Reconstructs the geometrical photon candidate for a given RichTrackSegment
   *  and hit position in global LHCb coordinates
   *
   *  @param trSeg          The RichTrackSegment to reconstruct the photon against
   *  @param detectionPoint The hit position in global LHCb coordinates
   *  @param gPhoton  The reconstructed photon
   *
   *  @return Status of the reconstruction
   *  @retval StatusCode::SUCCESS Photon reconstruction was successful
   *  @retval StatusCode::FAILURE Photon reconstruction was no possible with given segment and hit position
   */
  virtual StatusCode 
  reconstructPhoton ( const RichTrackSegment& trSeg,   
                      const HepPoint3D& detectionPoint, 
                      RichGeomPhoton& gPhoton ) const = 0;

};

#endif // RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H
