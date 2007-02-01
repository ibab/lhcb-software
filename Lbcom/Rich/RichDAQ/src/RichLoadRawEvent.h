
//-----------------------------------------------------------------------------
/** @file RichLoadRawEvent.h
 *
 *  Header file for RICH DAQ algorithm : Rich::DAQ::LoadRawEvent
 *
 *  CVS Log :-
 *  $Id: RichLoadRawEvent.h,v 1.3 2007-02-01 17:42:29 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichLoadRawEvent_H
#define RICHDAQ_RichLoadRawEvent_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Event model
#include "Event/RawEvent.h"

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

  //-----------------------------------------------------------------------------
  /** @namespace DAQ
   *
   *  namespace for RICH DAQ software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class LoadRawEvent RichLoadRawEvent.h
     *
     *  Trivial algorithm to load the RawEvent data object. Useful for timing studies
     *  to ensure the RawEvent object is in memory before decoding.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-06
     */
    //-----------------------------------------------------------------------------

    class LoadRawEvent : public RichAlgBase
    {

    public:

      /// Standard constructor
      LoadRawEvent( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~LoadRawEvent( ); ///< Destructor

      virtual StatusCode execute   (); ///< Algorithm execution

    };

  }
}

#endif // RICHDAQ_RichLoadRawEvent_H
