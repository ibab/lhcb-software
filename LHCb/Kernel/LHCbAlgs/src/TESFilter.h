// $Id: TESFilter.h,v 1.1.1.1 2004-06-14 12:40:38 cattanem Exp $
#ifndef TESFILTER_H 
#define TESFILTER_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class TESFilter TESFilter.h
 *  Removes selected containers from transient event store. 
 *  By default on all events (can be used on real data)
 *  Can be restricted to one event type for MC data
 *
 *  @author Marco Cattaneo
 *  @date   2003-04-30
 */
class TESFilter : public GaudiAlgorithm {
public:
  /// Standard constructor
  TESFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TESFilter() {}; ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::vector<std::string> m_containers; ///< Containers to be filtered
  int m_evtType; ///< Event type on which to filter. Default=0 (all event types)
};
#endif // TESFILTER_H
