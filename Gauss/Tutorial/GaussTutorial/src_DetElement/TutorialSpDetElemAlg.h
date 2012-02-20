// $Id: $
#ifndef TUTORIALSPDETELEMALG_H 
#define TUTORIALSPDETELEMALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"



/** @class TutorialSpDetElemAlg TutorialSpDetElemAlg.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-02-13
 */
class TutorialSpDetElemAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TutorialSpDetElemAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TutorialSpDetElemAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // TUTORIALSPDETELEMALG_H
