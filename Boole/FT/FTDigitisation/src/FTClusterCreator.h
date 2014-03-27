// $Id$
#ifndef FTCLUSTERCREATOR_H 
#define FTCLUSTERCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"

#include "Event/MCFTDigit.h"

//using namespace LHCb;

/** @class FTClusterCreator FTClusterCreator.h
 *  
 *
 *  @author Eric Cogneras
 *  @date   2012-04-06
 */
class FTClusterCreator : public GaudiHistoAlg {
public: 
  /// Standard constructor
  FTClusterCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FTClusterCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  bool keepAdding(LHCb::MCFTDigits::const_iterator clusCandIter);///< Check conditions before adding another digit to the cluster

private:
  // Location
  std::string m_inputLocation;       ///< MCFTDigit input Container
  std::string m_outputLocation;      ///< FTCluster output Container
  std::string m_clusterAlgo;         ///< Clustering algo to be performed (6 or 2 bits) [Temporary] 

  double m_dynamicsLowLimit;         ///< Lower edge of the clustering range
  double m_dynamicsUpLimit;          ///< Upper edge of the clustering range
  uint m_dynamicsBitsNber;           ///< Number of bits of the dynamics

  int m_adcThreshold;   ///<Minimal ADC value for cluster definition
  unsigned int m_clusterMinWidth;   ///< Minimal Width for clusters
  unsigned int m_clusterMaxWidth;   ///< Maximal Width for clusters

  unsigned int m_clusterMinCharge;   ///<Minimal total ADC for clusters
  int m_clusterMinADCPeak;   ///<Minimal ADC for cluster peaks
  //=== Sum for some average information
  int  m_nCluster;
  float m_sumCharge;

  bool m_splitPrevNextMoni;
  int  m_removeITRegion;
  float m_ITScale;

  int     m_nberOfLostHitFromMap;
  int     m_nberOfKeptHitFromMap;
  int     m_evtNbCluster64;
  
  DeFTDetector* m_deFT; ///< pointer to FT detector description

};
#endif // FTCLUSTERCREATOR_H


