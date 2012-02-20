// $Id: $
#ifndef EXERCISE3SOLUTION_TUTORIALPIXCONTAINERALG_H 
#define EXERCISE3SOLUTION_TUTORIALPIXCONTAINERALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"



/** @class TutorialPixContainerAlg TutorialPixContainerAlg.h Exercise3Solution/TutorialPixContainerAlg.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-02-13
 */
class TutorialPixContainerAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TutorialPixContainerAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TutorialPixContainerAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

private:

};
#endif // EXERCISE3SOLUTION_TUTORIALPIXCONTAINERALG_H
