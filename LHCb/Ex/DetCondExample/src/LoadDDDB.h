// $Id: LoadDDDB.h,v 1.1 2005-11-17 16:52:46 marcocle Exp $
#ifndef LOADDDDB_H 
#define LOADDDDB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class LoadDDDB LoadDDDB.h
 *  
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

};
#endif // LOADDDDB_H
