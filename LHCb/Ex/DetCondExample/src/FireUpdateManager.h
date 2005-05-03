// $Id: FireUpdateManager.h,v 1.1 2005-05-03 12:46:19 marcocle Exp $
#ifndef FIREUPDATEMANAGER_H 
#define FIREUPDATEMANAGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IUpdateManagerSvc;

/** @class FireUpdateManager FireUpdateManager.h
 *  
 *  Simple algorithm used to start the UpdateManagerSvc update loop before
 *  calling the other algorithms.
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-03-31
 */
class FireUpdateManager : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FireUpdateManager( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FireUpdateManager( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IUpdateManagerSvc *m_updateMgr;

};
#endif // FIREUPDATEMANAGER_H
