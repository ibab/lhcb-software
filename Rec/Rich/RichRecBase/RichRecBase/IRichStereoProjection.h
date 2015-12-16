
//-----------------------------------------------------------------------------
/** @file IRichStereoProjection.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IStereoProjection
 *
 *  @author Luigi Delbuono   delbuono@in2p3.fr
 *  @date   20/06/2007
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHSTEREOPROJECTION_H
#define RICHRECBASE_IRICHSTEREOPROJECTION_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
  class RichRecRing;
}

/// Static Interface Identification
static const InterfaceID IID_IRichStereoProjection( "IRichStereoProjection", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //--------------------------------------------------------------------------------------
    /** @class IStereoProjection IRichStereoProjection.h RichRecBase/IRichStereoProjection.h
     *
     *  Interface for tool to perform a stereographic projection for the RICH photons
     *
     *  @author Luigi Delbuono  delbuono@in2p3.fr
     *  @date   20/06/2007
     */
    //--------------------------------------------------------------------------------------

    class IStereoProjection : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichStereoProjection; }

      /** Projects (RichRec) photons in the stereo plane perpendicular to the segment direction
       *
       *  @param[in] segment The RichRecSegment which photons have to be projected
       *  @param[out] recRing (free pointer) with usual info and (x,y) stereo plane projection in "ringPoints()" field
       *  @return status flag
       *  @retval 0 means abnormal termination
       *  @retval 1 means normal completion
       */
      virtual int Project( const LHCb::RichRecSegment *segment, LHCb::RichRecRing &recRing ) const = 0;

      /** Projects (RichRec) photons in the stereo plane perpendicular to the segment direction
       *
       *  @param[in] richRecPhotons vector of photons to be projected
       *  @param[out] recRing (free pointer) with usual info and (x,y) stereo plane projection in "ringPoints()" field
       *  @return status flag
       *  @retval 0 means abnormal termination
       *  @retval 1 means normal completion
       */
      virtual int Project( LHCb::RichRecPhoton::ConstVector & richRecPhotons, LHCb::RichRecRing &recRing ) const = 0;

      /** Computes the average Cerenkov photons emission point in the radiator along a segment direction
       *
       *  @param[in] segment The RichRecSegment which photons have to be projected
       *  @param[out] avgPoint XYZ vector of the average point (global coordinates)
       *  @return status flag
       *  @retval 0 means abnormal termination
       *  @retval 1 means normal completion
       */
      virtual int avgEmissionPnt(const LHCb::RichRecSegment *segment, Gaudi::XYZPoint &avgPoint) const = 0;

      /** Computes the average Cerenkov photons emission point in radiator (for a vector of photons associated to a segment)
       *
       *  @param[in] richRecPhotons vector of photons to be projected
       *  @param[out] avgPoint XYZ vector of the average point (global coordinates)
       *  @return status flag
       *  @retval 0 means abnormal termination
       *  @retval 1 means normal completion
       */
      virtual int avgEmissionPnt(LHCb::RichRecPhoton::ConstVector & richRecPhotons, Gaudi::XYZPoint &avgPoint) const = 0;

      /** Computes the rotation matrix from the global frame to the stereo frame (Z along segment direction)
       *
       *  @param[in] segment The RichRecSegment which photons have to be projected
       *  @param[out] rot Rotation matrix from the global frame to the stereo frame (Z along segment direction)
       *  @return status flag
       *  @retval 0 means abnormal termination
       *  @retval 1 means normal completion
       */
      virtual int toUnitSphere(const LHCb::RichRecSegment *segment, Gaudi::Rotation3D &rot) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHSTEREOPROJECTION_H
