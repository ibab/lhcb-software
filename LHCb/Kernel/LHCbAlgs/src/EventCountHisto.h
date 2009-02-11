// $Id: EventCountHisto.h,v 1.1 2009-02-11 17:12:33 rlambert Exp $
#ifndef EVENTCOUNTHISTO_H 
#define EVENTCOUNTHISTO_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class EventCountHisto EventCountHisto.h
 *  
 *
 *  @author Robert Lambert
 *  @date   2007-01-08
 */
class EventCountHisto : public GaudiHistoAlg {
public: 
  /// Standard constructor
  EventCountHisto( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventCountHisto( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  int m_nEvents; //number of events processed

};
#endif // EVENTCOUNTHISTO_H
