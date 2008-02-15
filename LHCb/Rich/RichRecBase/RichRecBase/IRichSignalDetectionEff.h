
//-----------------------------------------------------------------------------
/** @file IRichSignalDetectionEff.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::ISignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: IRichSignalDetectionEff.h,v 1.9 2008-02-15 10:08:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHSIGNALDETECTIONEFF_H
#define RICHRECBASE_IRICHSIGNALDETECTIONEFF_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichSignalDetectionEff( "Rich::Rec::ISignalDetectionEff", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class ISignalDetectionEff IRichSignalDetectionEff.h
     *
     *  Interface for tools providing calculations for the photon
     *  detection efficiencies.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class ISignalDetectionEff : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichSignalDetectionEff; }

      /** Computes the detection efficiency for average cherenkov photon of a given energy,
       *  that are emitted from a given RichRecSegment.
       *
       *  @param  segment The RichRecSegment for which the efficiency should be computed
       *  @param  hypo    The mass hypothesis to consider (defines the ring on the detector plane).
       *  @param  energy  The photon energy
       *
       *  @return The average detection efficiency for photons of the given energy
       */
      virtual double photonDetEfficiency( LHCb::RichRecSegment * segment,
                                          const Rich::ParticleIDType hypo,
                                          const double energy ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHSIGNALDETECTIONEFF_H
