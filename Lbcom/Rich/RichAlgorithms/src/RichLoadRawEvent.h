
//-----------------------------------------------------------------------------
/** @file RichLoadRawEvent.h
 *
 *  Header file for RICH DAQ algorithm : Rich::DAQ::LoadRawEvent
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

namespace Rich
{
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

    class LoadRawEvent : public Rich::AlgBase
    {

    public:

      /// Standard constructor
      LoadRawEvent( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~LoadRawEvent( ); ///< Destructor

      virtual StatusCode execute(); ///< Algorithm execution

    private:

      /// Raw Event location
      std::string m_rawEventLoc;

    };

  }
}

#endif // RICHDAQ_RichLoadRawEvent_H
