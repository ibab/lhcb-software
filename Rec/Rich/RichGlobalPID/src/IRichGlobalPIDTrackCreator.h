
//--------------------------------------------------------------------------
/** @file IRichGlobalPIDTrackCreator.h
 *
 *  Interface Header file for RICH Global PID tool : Rich::Rec::GlobalPID::TrackCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_IRichGlobalPIDTrackCreator_H
#define RICHGLOBALPID_IRichGlobalPIDTrackCreator_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
namespace LHCb
{
  class RichRecTrack;
  class RichGlobalPIDTrack;
}

static const InterfaceID IID_IRichGlobalPIDTrackCreator ( "IRichGlobalPIDTrackCreator", 1, 0 );

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      /** @class ITrackCreator IRichGlobalPIDTrackCreator.h
       *
       *  Interface to tool to create RichGlobalPIDTrack track objects 
       *  for the RICH Global PID algorithm
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2008-03-01
       */
      class ITrackCreator : virtual public IAlgTool
      {

      public:

        /// Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichGlobalPIDTrackCreator; }

        /// Create a GPID track from a RichRecTrack
        virtual LHCb::RichGlobalPIDTrack * createTrack( LHCb::RichRecTrack * track ) const = 0;

        /// Finalise the given RichGlobalPIDtrack
        virtual void finaliseTrack( LHCb::RichGlobalPIDTrack * track ) const = 0;
        
      };

    }
  }
}

#endif // RICHGLOBALPID_IRichGlobalPIDTrackCreator_H
