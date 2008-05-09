
//-----------------------------------------------------------------------------
/** @file IRichIsolatedTrack.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IIsolatedTrack
 *
 *  CVS Log :-
 *  $Id: IRichIsolatedTrack.h,v 1.1 2008-05-09 13:59:20 jonrob Exp $
 *
 *  @author Susan Haines
 *  @author Chris Jones
 *  @date   15/02/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRichIsolatedTrack_H
#define RICHRECBASE_IRichIsolatedTrack_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// RichRecBase
#include "RichRecBase/RichTrackID.h"

// Kernel
#include "Kernel/RichParticleIDType.h"

// Event model
namespace LHCb
{
  class RichRecTrack;
  class Track;
}

static const InterfaceID IID_IRichIsolatedTrack ( "Rich::Rec::IIsolatedTrack", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------
    /** @class IIsolatedTrack IRichIsolatedTrack.h RichRecBase/IRichIsolatedTrack.h
     *
     *  Interface to a utility tool which finds isolated RICH tracks.
     *
     *  @author Susan Haines
     *  @author Chris Jones
     *  @date   15/02/2008
     */
    //---------------------------------------------------------------------------------

    class IIsolatedTrack : virtual public IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID & interfaceID() { return IID_IRichIsolatedTrack; }

      /** Is this segment isolated in the RICH system for the given mass hypothesis
       *  @param[in] segment The track segment
       *  @param[in] pid     The mass hypothesis to consider
       *  @return Boolean indicating if the track is isolated or not
       */
      virtual bool isIsolated( const LHCb::RichRecSegment * segment,
                               const Rich::ParticleIDType pid ) const = 0;

      /** Is this segment isolated in the RICH system for any mass hypothesis
       *  @param[in] segment The track segment
       *  @return Boolean indicating if the track is isolated or not
       */
      virtual bool isIsolated( const LHCb::RichRecSegment * segment ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRichIsolatedTrack_H
