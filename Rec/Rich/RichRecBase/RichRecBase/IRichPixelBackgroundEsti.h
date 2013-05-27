
//-----------------------------------------------------------------------------
/** @file IRichPixelBackgroundEsti.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::Rec::IPixelBackgroundEsti
 *
 *  CVS Log :-
 *  $Id: IRichPixelBackgroundEsti.h,v 1.1 2008-03-25 16:01:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/03/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRichPixelBackgroundEsti_H
#define RICHRECBASE_IRichPixelBackgroundEsti_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
#include "Event/RichRecTrack.h"

/// Static Interface Identification
static const InterfaceID IID_IRichPixelBackgroundEsti( "Rich::Rec::IPixelBackgroundEsti" , 1 , 0 );

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class IPixelBackgroundEsti IRichPixelBackgroundEsti.h
     *
     *  Interface to tools which estimate the pixel background terms
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/03/2008
     */
    //-----------------------------------------------------------------------------

    class IPixelBackgroundEsti : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichPixelBackgroundEsti; }

      /** Compute the pixel background estimates using all tracks
       */
      virtual void computeBackgrounds( ) const = 0;

      /** Compute the pixel background estimates using the given tracks only
       */
      virtual void computeBackgrounds( const LHCb::RichRecTrack::Vector & tracks ) const = 0;

      /** Compute the pixel background estimates using the given track only
       */
      virtual void computeBackgrounds( const LHCb::RichRecTrack * track ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRichPixelBackgroundEsti_H
