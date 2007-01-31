// $Id: LoadDDDB.h,v 1.1 2007-01-31 13:44:47 marcocle Exp $
#ifndef LOADDDDB_H 
#define LOADDDDB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class LoadDDDB LoadDDDB.h
 *  
 *  Load entries in the detector transient store using IDataSvc::preLoad().
 *  The node to be loaded is set with the option LoadDDDB.Node.
 *
 *  @author Marco Clemencic
 *  @date   2005-10-14
 */
class LoadDDDB : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LoadDDDB( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LoadDDDB( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::string m_treeToLoad;

};
#endif // LOADDDDB_H
