// $Id: CreateL1Event.h,v 1.1 2004-06-28 11:41:03 cattanem Exp $
#ifndef CREATEL1EVENT_H 
#define CREATEL1EVENT_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CreateL1Event CreateL1Event.h
 *  Little algorithm to create the L1Event from the L1Buffer
 *  Must be called after all modifications to L1Buffer have been made
 *
 *  @author Marco Cattaneo
 *  @date   2003-10-03
 */
class CreateL1Event : public GaudiAlgorithm {
public:
  /// Standard constructor
  CreateL1Event( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CreateL1Event( ) {}; ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

};
#endif // CREATEL1EVENT_H
