/** @file IRichPhotonReconstruction.h
 *
 *  Header file for tool interface : IRichPhotonReconstruction
 *
 *  $Id: IRichPhotonReconstruction.h,v 1.2 2006-05-05 10:39:07 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

#ifndef RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H
#define RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from MathCore
#include "Kernel/Point3DTypes.h"

// RichKernel
#include "Kernel/RichSmartID.h"

// forward decs
namespace LHCb
{
  //class RichSmartID;
  class RichGeomPhoton;
  class RichTrackSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichPhotonReconstruction ( "IRichPhotonReconstruction", 1, 0 );

/** @class IRichPhotonReconstruction IRichPhotonReconstruction.h RichKernel/IRichPhotonReconstruction.h
 *
 *  A tool to reconstruct photons given a RichTrackSegment and a detection
 *  point.  The result is returned as a RichGeomPhoton object.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

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
  reconstructPhoton ( const LHCb::RichTrackSegment& trSeg,
                      const LHCb::RichSmartID smartID,
                      LHCb::RichGeomPhoton& gPhoton ) const = 0;

  /** Reconstructs the geometrical photon candidate for a given RichTrackSegment
   *  and hit position in global LHCb coordinates
   *
   *  @param trSeg          The RichTrackSegment to reconstruct the photon against
   *  @param detectionPoint The hit position in global LHCb coordinates
   *  @param gPhoton        The reconstructed photon
   *  @param smartID        The RichSmartID associated to the hit position 
   *                        (optional, used to set the data member in reconstructed photon)
   *
   *  @return Status of the reconstruction
   *  @retval StatusCode::SUCCESS Photon reconstruction was successful
   *  @retval StatusCode::FAILURE Photon reconstruction was no possible with given segment and hit position
   */
  virtual StatusCode
  reconstructPhoton ( const LHCb::RichTrackSegment& trSeg,
                      const Gaudi::XYZPoint& detectionPoint,
                      LHCb::RichGeomPhoton& gPhoton,
                      const LHCb::RichSmartID smartID = LHCb::RichSmartID() ) const = 0;

};

#endif // RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H
