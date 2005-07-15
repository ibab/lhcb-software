// $Id: EventNodeKiller.h,v 1.1 2005-07-15 16:41:41 cattanem Exp $
#ifndef EVENTNODEKILLER_H 
#define EVENTNODEKILLER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <vector>
#include <string>

/** @class EventNodeKiller EventNodeKiller.h
 *  Algorithm to kill nodes in the event data store. Useful if you want to
 *  make the application "forget" about the previous history of the input file
 *
 *  @author Marco Cattaneo
 *  @date   2005-07-14
 */
class EventNodeKiller : public GaudiAlgorithm {
public: 
  /// Standard constructor
  EventNodeKiller( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventNodeKiller( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::vector<std::string> m_nodes; ///< String property "Nodes"
};
#endif // EVENTNODEKILLER_H
