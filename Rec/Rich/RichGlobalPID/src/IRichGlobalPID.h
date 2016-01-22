
//--------------------------------------------------------------------------
/** @file IRichGlobalPID.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::IRichGlobalPID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_IRICHGLOBALPID_H
#define RICHGLOBALPID_IRICHGLOBALPID_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
#include "Event/RichGlobalPIDTrack.h"
#include "Event/RichGlobalPID.h"

static const InterfaceID IID_IRichGlobalPID ( "IRichGlobalPID", 1, 0 );

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      /** @class IRichGlobalPID IRichGlobalPID.h
       *
       *   Interface to RichGlobalPID maker
       *
       *  @author Chris Jones
       *  @date   2008-03-01
       */
      class IRichGlobalPID : virtual public IAlgTool
      {

      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichGlobalPID; }

      public:

        /** Access the RichGlobalPID associated to the given Track
         *
         *  @param[in] track Pointer to the LHCb::Track object
         *
         *  @return Pointer to the RichGlobalPID obect for the given track
         *  @retval NULL  No RichGlobalPID object is available for the given track
         *  @retval !NULL RICH information is available for the given track
         */
        virtual const LHCb::RichGlobalPID * pid( LHCb::RichGlobalPIDTrack * track ) const = 0;

        /** Create the RichGlobalPID data objects for the given Tracks
         *
         *  @param[in] tracks The list of tracks to get the RichGlobalPID data objects for
         */
        virtual void pids( const LHCb::RichGlobalPIDTrack::Vector & tracks ) const = 0;

      };

    }
  }
}

#endif // RICHGLOBALPID_IRICHGLOBALPID_H
