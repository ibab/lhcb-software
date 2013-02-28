#ifndef PROCSTATUSCHECK_H 
#define PROCSTATUSCHECK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class ProcStatusCheck ProcStatusCheck.h
 *  
 * A simple algorithm that checks for the events aborted by reconstruction 
 * algorithms. 
 * 
 * Returns True if at least one record is present in ProcStatus structure. 
 * 
 * If OutputLevel == DEBUG, prints out the names and status of the failed 
 * algorithms 
 *
 *  @author Anton Poluektov
 *  @date   2010-06-24
 */
class ProcStatusCheck : public GaudiAlgorithm
{

public: 

  /// Standard constructor
  ProcStatusCheck( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProcStatusCheck( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

};
#endif // PROCSTATUSCHECK_H
