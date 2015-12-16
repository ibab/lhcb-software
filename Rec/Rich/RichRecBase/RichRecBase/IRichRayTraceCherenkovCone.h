
//---------------------------------------------------------------------------------
/** @file IRichRayTraceCherenkovCone.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IRayTraceCherenkovCone
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H
#define RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H 1

// from std
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// Event model
namespace LHCb
{
  class RichRecRing;
  class RichRecSegment;
}

// Kernel
#include "Kernel/RichTraceMode.h"
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichDetectorType.h"

/// Static Interface Identification
static const InterfaceID IID_IRichRayTraceCherenkovCone( "Rich::Rec::IRayTraceCherenkovCone", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------
    /** @class IRayTraceCherenkovCone IRichRayTraceCherenkovCone.h
     *
     *  Interface for tool to ray trace cherenkov photons in a cone around a given
     *  RichRecSegment direction, at the given angle or the angle correct for a given mass
     *  hypothesis.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //---------------------------------------------------------------------------------

    class IRayTraceCherenkovCone : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichRayTraceCherenkovCone; }

      /** Ray trace the Cherenkov cone for the given segment and mass hypothesis
       *  to the detector plane. Assummed emission point is the best average for the
       *  given RichRecSegment and the direction around which the cone is projected
       *  is the best average direction for the segment.
       *
       *  @param segment Pointer to the RichRecSegment to raytrace from
       *  @param id      The mass hypothesis to use to calculate the Cherenkov cone angle
       *  @param points  On return the vector of points (in global coordinates)
       *                 on the HPD detector plane that lie on the Cherenkov ring.
       *  @param nPoints Number of points to ray trace around the ring
       *  @param mode    The ray-tracing mode configuration object
       *
       *  @return StatusCode for the ray tracing
       *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
       *  @retval StatusCode::FAILURE Ray tracing failed
       */
      virtual StatusCode rayTrace ( LHCb::RichRecSegment * segment,
                                    const Rich::ParticleIDType id,
                                    std::vector<Gaudi::XYZPoint> & points,
                                    const unsigned int nPoints = 100,
                                    const LHCb::RichTraceMode mode = LHCb::RichTraceMode() ) const = 0;

      /** Ray trace the Cherenkov cone for the given segment and Cherenkoc angle
       *  to the detector plane. Assummed emission point is the best average for the
       *  given RichRecSegment and the direction around which the cone is projected
       *  is the best average direction for the segment.
       *
       *  @param segment Pointer to the RichRecSegment to raytrace from
       *  @param ckTheta The Cherenkov angle to use
       *  @param points  On return the vector of points (in global coordinates)
       *                 on the HPD detector plane that lie on the Cherenkov ring.
       *  @param nPoints Number of points to ray trace around the ring
       *  @param mode    The ray-tracing mode configuration object
       *
       *  @return StatusCode for the ray tracing
       *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
       *  @retval StatusCode::FAILURE Ray tracing failed
       */
      virtual StatusCode rayTrace ( LHCb::RichRecSegment * segment,
                                    const double ckTheta,
                                    std::vector<Gaudi::XYZPoint> & points,
                                    const unsigned int nPoints = 100,
                                    const LHCb::RichTraceMode mode = LHCb::RichTraceMode() ) const = 0;

      /** Ray trace the Cherenkov cone using the given emission point, direction
       *  and Cherenkov angle.
       *
       *  @param rich          The RICH detector in which to ray trace
       *  @param emissionPoint The emission point for the ray tracing
       *  @param direction     The direction around which to project the Cherenkov cone
       *  @param ckTheta       The Cherenkov angle to use
       *  @param points        On return the vector of points (in global coordinates)
       *                       on the HPD detector plane that lie on the Cherenkov ring.
       *  @param nPoints       Number of points to ray trace around the ring
       *  @param mode          The ray-tracing mode configuration object
       *
       *  @return StatusCode for the ray tracing
       *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
       *  @retval StatusCode::FAILURE Ray tracing failed
       */
      virtual StatusCode rayTrace ( const Rich::DetectorType rich,
                                    const Gaudi::XYZPoint & emissionPoint,
                                    const Gaudi::XYZVector & direction,
                                    const double ckTheta,
                                    std::vector<Gaudi::XYZPoint> & points,
                                    const unsigned int nPoints = 100,
                                    const LHCb::RichTraceMode mode = LHCb::RichTraceMode() ) const = 0;

      /** Ray trace the Cherenkov cone for the given ring to the detector plane,
       *  in Global coordinates.
       *
       *  @param ring Point to the Ring object to use as the basis of the ray tracing
       *  @param nPoints       Number of points to ray trace around the ring
       *  @param mode          The ray-tracing mode configuration object
       *  @param forceTracing  Force the ray tracing
       *
       *  @return StatusCode for the ray tracing
       *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
       *  @retval StatusCode::FAILURE Ray tracing failed
       */
      virtual StatusCode
      rayTrace ( LHCb::RichRecRing * ring,
                 const unsigned int nPoints = 100,
                 const LHCb::RichTraceMode mode = LHCb::RichTraceMode(),
                 const bool forceTracing = false ) const = 0;

      /** Ray trace the Cherenkov cone for the given ring to the detector plane,
       *  in Global coordinates.
       *
       *  @param rich          The RICH detector in which to ray trace
       *  @param emissionPoint The emission point for the ray tracing
       *  @param direction     The direction around which to project the Cherenkov cone
       *  @param ckTheta       The Cherenkov angle to use
       *  @param ring          Pointer to the Ring object to use as the basis of the ray tracing
       *  @param nPoints       Number of points to ray trace around the ring
       *  @param mode          The ray-tracing mode configuration object
       *  @param forceTracing  Force the ray tracing
       *
       *  @return StatusCode for the ray tracing
       *  @retval StatusCode::SUCCESS Ray tracing was successful, vector points is valid
       *  @retval StatusCode::FAILURE Ray tracing failed
       */
      virtual StatusCode
      rayTrace ( const Rich::DetectorType rich,
                 const Gaudi::XYZPoint & emissionPoint,
                 const Gaudi::XYZVector & direction,
                 const double ckTheta,
                 LHCb::RichRecRing * ring,
                 const unsigned int nPoints = 100,
                 const LHCb::RichTraceMode mode = LHCb::RichTraceMode(),
                 const bool forceTracing = false ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H
