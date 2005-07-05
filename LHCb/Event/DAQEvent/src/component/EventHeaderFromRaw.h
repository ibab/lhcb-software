// $Id: EventHeaderFromRaw.h,v 1.2 2005-07-05 11:54:24 cattanem Exp $
#ifndef COMPONENT_EVENTHEADERFROMRAW_H 
#define COMPONENT_EVENTHEADERFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class EventHeaderFromRaw EventHeaderFromRaw.h component/EventHeaderFromRaw.h
 *  Trivial algorithm to decode the EventHeader from the RawBuffer
 *
 *  @author Marco Cattaneo
 *  @date   2004-10-21
 */
class EventHeaderFromRaw : public GaudiAlgorithm {
public: 
  /// Standard constructor
  EventHeaderFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventHeaderFromRaw( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

};
#endif // COMPONENT_EVENTHEADERFROMRAW_H
