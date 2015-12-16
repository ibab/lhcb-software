
//-----------------------------------------------------------------------------
/** @file IRichRecGeomTool.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IGeomTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHRECGEOMTOOL_H
#define RICHRECTOOLS_IRICHRECGEOMTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
namespace LHCb
{
  class RichRecPixel;
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichRecGeomTool( "Rich::Rec::IGeomTool" , 1 , 0 );

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class IGeomTool IRichRecGeomTool.h
     *
     *  Interface for a tool to answer simple geometrical questions
     *  using the reconstruction event model
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IGeomTool : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichRecGeomTool; }

      /** Computes the square of the distance separating the pixel hit and track hit
       *  position extrapolated using the RichRecSegment direction in local
       *  HPD panel coordinates
       *
       *  @param segment Pointer to the RichRecSegment
       *  @param pixel   Pointer to the RichRecPixel
       *
       *  @return The separation squared in local HPD panel coordinates
       */
      virtual double trackPixelHitSep2( const LHCb::RichRecSegment * segment,
                                        const LHCb::RichRecPixel * pixel ) const = 0;

      /** Computes the fraction of the Cherenkov cone for a given segment and mass hypothesis
       *  that lies within the average HPD panel acceptance
       *
       *  @param segment Pointer the a RichRecSegment
       *  @param id      The mass hypothesis
       *
       *  @return The fraction of the cherenkov ring that is within the averge HPD acceptance
       */
      virtual double hpdPanelAcceptance( LHCb::RichRecSegment * segment,
                                         const Rich::ParticleIDType id ) const = 0;

      /** Set the corrected pixel position for the average
       *  optical distortion for the given RICH detector
       *
       *  @param pixel Pointer to the RichRecPixel
       *  @param rich  The RICH detector
       */
      virtual void
      setCorrLocalPos( const LHCb::RichRecPixel * pixel,
                       const Rich::DetectorType rich ) const = 0;

      /** Returns the corrected pixel position for the average
       *  optical distortion for the given radiator.
       *
       *  @param lpos  The local position to correct
       *  @param rad   The radiator type to correct for
       *  @return      The corrected local position
       */
      virtual Gaudi::XYZPoint radCorrLocalPos ( const Gaudi::XYZPoint & lpos,
                                                const Rich::RadiatorType rad ) const = 0;
      
    };

  }
}

#endif // RICHRECTOOLS_IRICHRECGEOMTOOL_H
