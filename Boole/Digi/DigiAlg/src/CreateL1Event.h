// $Id: CreateL1Event.h,v 1.1 2003-10-06 16:16:39 cattanem Exp $
#ifndef CREATEL1EVENT_H 
#define CREATEL1EVENT_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


/** @class CreateL1Event CreateL1Event.h
 *  Stupid little algorithm to create the L1Event from the L1Buffer at the
 *  beginning of the L1Trigger sequence. This algorithm should be included
 *  in any timing measurement of the L1 trigger
 *
 *  @author Marco Cattaneo
 *  @date   2003-10-03
 */
class CreateL1Event : public Algorithm {
public:
  /// Standard constructor
  CreateL1Event( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CreateL1Event( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // CREATEL1EVENT_H
