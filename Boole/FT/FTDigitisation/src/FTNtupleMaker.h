#ifndef FTNTUPLEMAKER_H 
#define FTNTUPLEMAKER_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"


/** @class FTNtupleMaker FTNtupleMaker.h
 *  
 *
 *  @author Eric Cogneras
 *  @date   2015-04-14
 */
class FTNtupleMaker : public GaudiTupleAlg {
public: 
  /// Standard constructor
  FTNtupleMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FTNtupleMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode ClusterWithLargestMCHitInfo(Tuple &tpl); ///< Fill cluster data with largest MC Hit information
  StatusCode ClusterWithAllMCHitInfo(Tuple &tpl); ///< Fill cluster data with all MC Hit information

protected:

private:
  std::string m_tupleName;
  std::string m_hitLocation;          ///< FTHit Container
  std::string m_digitLocation;        ///< FTDigit Container
  std::string m_clusterLocation;      ///< FTCluster Container

  uint m_ntupHitMaxSize;
  uint m_ntupDigitMaxSize;
  uint m_ntupClusterMaxSize; 

  DeFTDetector* m_deFT; ///< pointer to FT detector description
};
#endif // FTNTUPLEMAKER_H
