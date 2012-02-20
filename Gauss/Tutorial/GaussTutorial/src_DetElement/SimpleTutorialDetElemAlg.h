// $Id: $
#ifndef SIMPLETUTORIALDETELEMALG_H 
#define SIMPLETUTORIALDETELEMALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"



/** @class SimpleTutorialDetElemAlg SimpleTutorialDetElemAlg.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-02-13
 */
class SimpleTutorialDetElemAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  SimpleTutorialDetElemAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SimpleTutorialDetElemAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  void dumpTutorialPVs(const ILVolume* lv, std::string de);

};
#endif // SIMPLETUTORIALDETELEMALG_H
