// $Id: EventHeaderFromRaw.h,v 1.1 2004-10-26 13:10:15 cattanem Exp $
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

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // COMPONENT_EVENTHEADERFROMRAW_H
