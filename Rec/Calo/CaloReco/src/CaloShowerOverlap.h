#ifndef CALOSHOWEROVERLAP_H 
#define CALOSHOWEROVERLAP_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CellID.h"
#include "SubClusterSelectorTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloInterfaces/ICaloShowerOverlapTool.h"
#include "Event/CaloCluster.h"

/** @class CaloShowerOverlap CaloShowerOverlap.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2014-06-02
 */
class CaloShowerOverlap : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloShowerOverlap( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloShowerOverlap( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int m_dMin;
  double m_etMin;
  double m_etMin2;
  int m_iter;
  std::string m_input;
  std::vector<std::string> m_taggerP  ;  
  std::vector<std::string> m_taggerE  ;  
  std::string m_det;
  const DeCalorimeter* m_detector;
  ICaloShowerOverlapTool* m_oTool;
  SubClusterSelectorTool* m_tagger  ;
};
#endif // CALOSHOWEROVERLAP_H
