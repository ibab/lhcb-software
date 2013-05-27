
//-----------------------------------------------------------------------------
/** @file IRichTrackEffectiveRefractiveIndex.h
 *
 *  Header file for tool interface : Rich::Rec::ITrackEffectiveRefractiveIndex
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   27/11/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRichTrackEffectiveRefractiveIndex_H
#define RICHRECBASE_IRichTrackEffectiveRefractiveIndex_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"

// Event
#include "Event/RichRecSegment.h"

/// Static Interface Identification
static const InterfaceID IID_IRichTrackEffectiveRefractiveIndex( "Rich::Rec::ITrackEffectiveRefractiveIndex", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    /** @class ITrackEffectiveRefractiveIndex RichRecBase/IRichTrackEffectiveRefractiveIndex.h
     *
     *  Interface for tool to calculate effective refractive indices for
     *  each reconstructed track.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   27/11/2008
     */

    class ITrackEffectiveRefractiveIndex : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichTrackEffectiveRefractiveIndex; }

      /** Calculates the refractive index for the given track segment at a
       *  given photon energy.
       *
       *  @param segment The track segment
       *  @param energy  The photon energy at which the refractive index is to be calculated
       *
       *  @return The refractive index at the requested energy for the given segment
       */
      virtual double refractiveIndex ( const LHCb::RichRecSegment * segment,
                                       const double energy ) const = 0;

      /** Calculates the average refractive index for track segment
       *  for all visable photon energies.
       *
       *  @param segment The track segment
       *
       *  @return The overall average refractive index
       */
      virtual double refractiveIndex ( const LHCb::RichRecSegment * segment ) const = 0;

      /** Calculates the refractive index R.M.S. for the radiator associated
       *  to the given track segment
       *
       *  @param segment The track segment
       *
       *  @return The refractive index R.M.S.
       */
      virtual double refractiveIndexRMS ( const LHCb::RichRecSegment * segment ) const = 0;

      /** Calculates the refractive index S.D. for the radiator associated
       *  to the given track segment
       *
       *  @param segment The track segment
       *
       *  @return The refractive index S.D.
       */
      virtual double refractiveIndexSD ( const LHCb::RichRecSegment * segment ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRichTrackEffectiveRefractiveIndex_H
