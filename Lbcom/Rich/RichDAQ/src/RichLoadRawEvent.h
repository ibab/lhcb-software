
//-----------------------------------------------------------------------------
/** @file RichLoadRawEvent.h
 *
 *  Header file for RICH DAQ algorithm : RichLoadRawEvent
 *
 *  CVS Log :-
 *  $Id: RichLoadRawEvent.h,v 1.1 2006-10-04 10:46:47 jonrob Exp $
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
/** @class RichLoadRawEvent RichLoadRawEvent.h
 *
 *  Trival algorithm to load the RawEvent data object. Useful for timing studies
 *  to ensure the RawEvent object is in memory before decoding.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//-----------------------------------------------------------------------------

class RichLoadRawEvent : public RichAlgBase
{

public:

  /// Standard constructor
  RichLoadRawEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichLoadRawEvent( ); ///< Destructor

  virtual StatusCode execute   (); ///< Algorithm execution

};

#endif // RICHDAQ_RichLoadRawEvent_H
