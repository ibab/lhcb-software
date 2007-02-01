
//-----------------------------------------------------------------------------
/** @file IRichSignalDetectionEff.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::ISignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: IRichSignalDetectionEff.h,v 1.7 2007-02-01 17:26:22 jonrob Exp $
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

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rich::Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
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
       *  @param segment  The RichRecSegment for which the efficiency should be computed
       *  @param  energy  The photon energy
       *
       *  @return The average detection efficiency for photons of the given energy
       */
      virtual double photonDetEfficiency( LHCb::RichRecSegment * segment,
                                          const double energy ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHSIGNALDETECTIONEFF_H
