
//-----------------------------------------------------------------------------
/** @file RichPhotonEmissionPoint.h
 *
 *  Header file for tool : Rich::Rec::PhotonEmissionPoint
 *
 *  CVS Log :-
 *  $Id: RichPhotonEmissionPoint.h,v 1.1.1.1 2007-11-26 17:28:18 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPhotonEmissionPoint_H
#define RICHRECTOOLS_RichPhotonEmissionPoint_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichPhotonEmissionPoint.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonEmissionPoint RichPhotonEmissionPoint.h
     *
     *  Tool providing the best emission point for a given track segment.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class PhotonEmissionPoint : public Rich::Rec::ToolBase,
                                virtual public IPhotonEmissionPoint
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonEmissionPoint( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

      /// Destructor
      virtual ~PhotonEmissionPoint();

    public: // methods (and doxygen comments) inherited from public interface

      // Get the best emission point for the given segment and pixel
      bool emissionPoint( const LHCb::RichRecSegment * segment,
                          const LHCb::RichRecPixel * pixel,
                          Gaudi::XYZPoint & emissPnt ) const;

      // Get the best emission point for the given segment and pixel, at the given fractional
      // distance along the radiator trajectory (0 for start point, 1 for end point)
      bool emissionPoint( const LHCb::RichRecSegment * segment,
                          const LHCb::RichRecPixel * pixel,
                          const double fracDist,
                          Gaudi::XYZPoint & emissPnt ) const;

    };

  }
}

#endif // RICHRECTOOLS_RichPhotonEmissionPoint_H
