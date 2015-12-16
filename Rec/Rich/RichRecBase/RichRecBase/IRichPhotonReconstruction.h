
/** @file IRichPhotonReconstruction.h
 *
 *  Header file for tool interface : Rich::Rec::IPhotonReconstruction
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

#ifndef RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H
#define RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from MathCore
#include "GaudiKernel/Point3DTypes.h"

// RichKernel
#include "Kernel/RichSmartID.h"

// forward decs
namespace LHCb
{
  //class RichSmartID;
  class RichGeomPhoton;
  class RichTrackSegment;
  class RichRecSegment;
  class RichRecPixel;
}
namespace Rich
{
  class HPDPixelCluster;
}

/// Static Interface Identification
static const InterfaceID IID_IRichPhotonReconstruction ( "Rich::Rec::IPhotonReconstruction", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    /** @class IPhotonReconstruction IRichPhotonReconstruction.h RichKernel/IRichPhotonReconstruction.h
     *
     *  A tool to reconstruct photons given a RichTrackSegment and a detection
     *  point.  The result is returned as a RichGeomPhoton object.
     *
     *  @author Antonis Papanestis
     *  @date   2003-10-28
     */

    class IPhotonReconstruction : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichPhotonReconstruction; }

      /** Reconstructs the geometrical photon candidate for a given RichTrackSegment
       *  and RichSmartID channel identifier
       *
       *  @param[in]  segment   Pointer to the RichRecSegment to reconstruct the photon against
       *  @param[in]  pixel     The hit pixel
       *  @param[out] gPhoton   The reconstructed photon
       *
       *  @return Status of the reconstruction
       *  @retval StatusCode::SUCCESS Photon reconstruction was successful
       *  @retval StatusCode::FAILURE Photon reconstruction was no possible with given segment and hit position
       */
      virtual StatusCode
      reconstructPhoton ( const LHCb::RichRecSegment * segment,
                          const LHCb::RichRecPixel * pixel,
                          LHCb::RichGeomPhoton& gPhoton ) const = 0;

    };

  }
}

#endif // RICHKERNEL_IRICHPHOTONRECONSTRUCTION_H
