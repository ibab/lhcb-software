// ============================================================================
#ifndef CALORECO_CALOMERGEDPI0_H 
#define CALORECO_CALOMERGEDPI0_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloInterfaces/ICaloClusterTool.h"
#include "SubClusterSelectorTool.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloInterfaces/ICaloShowerOverlapTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloCluster.h"
// ============================================================================

/** @class CaloMergedPi0 CaloMergedPi0.h
 *  
 *  Merged pi0 reconstruction with iterativ Method
 *
 * NEW IMPLEMENTATION 
 *
 *  @author Olivier Deschamps 
 *  @date   05/05/2014
 */

class CaloMergedPi0 : public GaudiAlgorithm{
  /// friend factory for instantiation
  friend class AlgFactory<CaloMergedPi0>;
  
public:
  
  CaloMergedPi0( const std::string& name   , ISvcLocator*       svcloc );  
  virtual ~CaloMergedPi0();  

  virtual StatusCode initialize();   
  virtual StatusCode execute   ();   
  virtual StatusCode finalize  ();  
    
private:
  bool isNeighbor(LHCb::CaloCellID id0, LHCb::CaloCellID id1);

  std::string m_clusters ;
  std::string m_mergedPi0s;
  std::string m_splitPhotons;
  std::string m_splitClusters;
  double m_etCut;
  int m_iter ;
  bool m_createClusterOnly;
  
  // tools name
  std::vector<std::string>     m_photonTools;
  std::vector<std::string>     m_pi0Tools;

  // tools interfaces
  std::vector<ICaloHypoTool*>  m_gTools ;
  std::vector<ICaloHypoTool*>  m_pTools ;
  ICaloShowerOverlapTool* m_oTool   ;
  ICaloClusterTool*       m_cov     ; 
  ICaloClusterTool*       m_spread  ;  
  SubClusterSelectorTool* m_tagger  ;
  std::string m_condition;
  std::vector<std::string>          m_taggerE  ;  
  std::vector<std::string>          m_taggerP  ;  
  std::string m_det;
  DeCalorimeter* m_detector;
  bool m_verbose;
  double m_eClusterMin;
};
// ============================================================================
#endif // CALOMERGEDPI0_H
