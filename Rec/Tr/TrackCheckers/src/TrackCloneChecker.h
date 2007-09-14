// $Id: TrackCloneChecker.h,v 1.1 2007-09-14 13:01:15 jonrob Exp $
#ifndef TRACKCLONECHECKER_H 
#define TRACKCLONECHECKER_H 1

#include "TrackCheckerBase.h"

// from Event/LinkerEvent
#include "Linker/LinkedFrom.h"

/** @class TrackCloneChecker TrackCloneChecker.h
 *  
 *  produce some simple plots for the Clone linker information
 *  
 *  @author Chris Jones
 *  @date   2007-09-13
 */

class TrackCloneChecker : public TrackCheckerBase
{

public: 

  /// Standard constructor
  TrackCloneChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackCloneChecker( ); ///< Destructor

  virtual StatusCode execute();  ///< Algorithm execution

private:

  /// Clone linker information location in TES
  std::string m_cloneInfoTES;

};

#endif // TRACKCLONECHECKER_H
