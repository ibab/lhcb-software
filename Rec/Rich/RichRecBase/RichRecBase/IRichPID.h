
//--------------------------------------------------------------------------
/** @file IRichPID.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::IRichPID
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHPID_H
#define RICHRECBASE_IRICHPID_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
#include "Event/Track.h"
#include "Event/RichPID.h"

static const InterfaceID IID_IRichPID ( "IRichPID", 1, 0 );

namespace Rich
{
  namespace Rec
  {

    /** @class IRichPID IRichPID.h RichRecBase/IRichPID.h
     *
     *  Interface to tools that provide on-demand access to RichPID objects
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2008-03-01
     */

    class IRichPID : virtual public IAlgTool
    {

    public:

      // Return the interface ID
      static const InterfaceID& interfaceID() { return IID_IRichPID; }

    public:

      /** Access the RichPID associated to the given Track
       * 
       *  @param[in] track Pointer to the LHCb::Track object
       *
       *  @return Pointer to the RichPID obect for the given track
       *  @retval NULL  No RichPID object is available for the given track
       *  @retval !NULL RICH information is available for the given track
       *
       *  @attention The user does NOT own the returned RichPID data object
       *             and thus should not delete it
       */
      virtual const LHCb::RichPID * pid( const LHCb::Track * track ) const = 0;

      /** Access the RichPID data objects for the given Tracks
       *  
       *  This method is provided for bulk operations. For instance in the 
       *  Global PID approach where it is benificial to process the tracks
       *  simulataneously.
       *
       *  @param[in] tracks    The list of tracks to get the RichPID data objects for
       *  @param[out] richpids The associated RichPID data objects (same sorting as
       *                       original track vector).
       * 
       *  @attention The returned RichPID vector may contain NULL pointers, 
       *             indicating RICH information was not available for that track.
       *             It is the responsibility of the user to check for such NULL pointers.
       */
      virtual void pids( const LHCb::Track::ConstVector & tracks,
                         LHCb::RichPID::ConstVector & richpids ) const = 0;

    };

  }
}

#endif // RICHRECBASE_IRICHPID_H
