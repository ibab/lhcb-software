
//-----------------------------------------------------------------------------
/** @file IRichIsolatedTrack.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IIsolatedTrack
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
  class RichRecSegment;
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

    public:

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

    public:

      /** Is this segment isolated in the RICH system for the given mass hypothesis
       *  @param[in] track The track
       *  @param[in] pid     The mass hypothesis to consider
       *  @return Boolean indicating if the track is isolated or not
       */
      virtual bool isIsolated( const LHCb::RichRecTrack * track,
                               const Rich::ParticleIDType pid ) const = 0;

      /** Is this segment isolated in the RICH system for any mass hypothesis
       *  @param[in] track The track
       *  @return Boolean indicating if the track is isolated or not
       */
      virtual bool isIsolated( const LHCb::RichRecTrack * track ) const = 0;

    public:

      /** Is this segment isolated in the RICH system for the given mass hypothesis
       *  @param[in] track The track
       *  @param[in] pid     The mass hypothesis to consider
       *  @return Boolean indicating if the track is isolated or not
       */
      virtual bool isIsolated( const LHCb::Track * track,
                               const Rich::ParticleIDType pid ) const = 0;

      /** Is this segment isolated in the RICH system for any mass hypothesis
       *  @param[in] track The track
       *  @return Boolean indicating if the track is isolated or not
       */
      virtual bool isIsolated( const LHCb::Track * track ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRichIsolatedTrack_H
