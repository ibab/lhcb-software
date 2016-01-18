
//-----------------------------------------------------------------------------
/** @file IRichPhotonEmissionPoint.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IPhotonEmissionPoint
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   31/10/2007
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRichPhotonEmissionPoint_H
#define RICHRECTOOLS_IRichPhotonEmissionPoint_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichPhotonEmissionPoint( "Rich::Rec::IPhotonEmissionPoint", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class IPhotonEmissionPoint IRichPhotonEmissionPoint.h
     *
     *  Interface for tool providing the best emission point for a 
     *  given track segment.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   31/10/2007
     */
    //-----------------------------------------------------------------------------

    class IPhotonEmissionPoint : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichPhotonEmissionPoint; }

      /** Get the best emission point for the given segment and pixel
       *
       *  @param[in] segment   RichRecSegment to determine the best emission point for
       *  @param[in] pixel     Pixel to be used for later photon reconstruction 
       *                       (not all implementations use this variable)
       *  @param[out] emissPnt The determined emission point
       *
       *  @return boolean indicating status of request
       *  @retval TRUE  Emission point was successfully determined
       *  @retval FALSE Emission point was NOT successfully determined
       */
      virtual bool emissionPoint( const LHCb::RichRecSegment * segment,
                                  const LHCb::RichRecPixel * pixel,
                                  Gaudi::XYZPoint & emissPnt ) const = 0;

      /** Get the best emission point for the given segment
       *
       *  @param[in] segment   RichRecSegment to determine the best emission point for
       *  @param[out] emissPnt The determined emission point
       *
       *  @return boolean indicating status of request
       *  @retval TRUE  Emission point was successfully determined
       *  @retval FALSE Emission point was NOT successfully determined
       */
      inline bool emissionPoint( const LHCb::RichRecSegment * segment,
                                 Gaudi::XYZPoint & emissPnt ) const
      {
        return emissionPoint( segment, nullptr, emissPnt );
      }

      /** Get the best emission point for the given segment and pixel, at the given fractional
       *  distance along the radiator trajectory (0 for start point, 1 for end point)
       *
       *  @param[in] segment   RichRecSegment to determine the best emission point for
       *  @param[in] pixel     Pixel to be used for later photon reconstruction 
       *                       (not all implementations use this variable)
       *  @param[in] fracDist  The fractional distance along the trajectory 
       *                       (0 for start point, 1 for end point)
       *  @param[out] emissPnt The determined emission point
       *
       *  @return boolean indicating status of request
       *  @retval TRUE  Emission point was successfully determined
       *  @retval FALSE Emission point was NOT successfully determined
       */
      virtual bool emissionPoint( const LHCb::RichRecSegment * segment,
                                  const LHCb::RichRecPixel * pixel,
                                  const double fracDist,
                                  Gaudi::XYZPoint & emissPnt ) const = 0;

      /** Get the best emission point for the given segment and pixel, at the given fractional
       *  distance along the radiator trajectory (0 for start point, 1 for end point)
       *
       *  @param[in] segment   RichRecSegment to determine the best emission point for
       *  @param[in] fracDist  The fractional distance along the trajectory 
       *                       (0 for start point, 1 for end point)
       *  @param[out] emissPnt The determined emission point
       *
       *  @return boolean indicating status of request
       *  @retval TRUE  Emission point was successfully determined
       *  @retval FALSE Emission point was NOT successfully determined
       */
      inline bool emissionPoint( const LHCb::RichRecSegment * segment,
                                 const double fracDist,
                                 Gaudi::XYZPoint & emissPnt ) const
      {
        return emissionPoint( segment, nullptr, fracDist, emissPnt );
      }

    };

  }
}

#endif // RICHRECTOOLS_IRichPhotonEmissionPoint_H
