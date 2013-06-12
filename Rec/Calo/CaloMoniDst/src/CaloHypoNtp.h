#ifndef CALOHYPONTP_H 
#define CALOHYPONTP_H 1
#include <vector>
#include "GaudiAlg/GaudiTupleAlg.h"
#include "CaloInterfaces/ICaloHypoEstimator.h"
#include "CaloInterfaces/ICalo2MCTool.h"
#include "GaudiKernel/IEventTimeDecoder.h"
// ============================================================================

class CaloHypoNtp : public GaudiTupleAlg{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypoNtp>;
public: 
 /// standard algorithm initialization
  CaloHypoNtp( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CaloHypoNtp();
  virtual StatusCode initialize();
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  /// destructor (virtual and protected)
private:
  /// default  constructor is private
  CaloHypoNtp();
  /// copy     constructor is private
  CaloHypoNtp( const CaloHypoNtp& );
  /// assignement operator is private
  CaloHypoNtp &operator=( const CaloHypoNtp& );
private:
  bool inRange( std::pair<double,double> range, double value);
  bool inRange( std::pair<int, int > range, int value);
  
  bool m_extrapol;
  bool m_seed;
  bool m_neig;  

  std::pair<double,double> m_et;
  std::pair<double,double> m_prsE;
  std::pair<double,double> m_spdM;
  std::pair<double,double> m_e;
  ICaloHypoEstimator* m_estimator ;   
  ICaloHypoEstimator* estimator(){return m_estimator;}
  IEventTimeDecoder* m_odin;
  bool m_tupling;
  std::string m_vrtLoc;
  std::string m_trkLoc;
  std::string m_tabLoc;
  std::vector<std::string> m_locs;
  bool m_checker;
  ICalo2MCTool* m_2MC;
  ICalo2MCTool* calo2MC(){return m_2MC;}
  int m_mcID;
  std::vector<std::string> m_hypos;
  bool m_print;
  bool m_stat;
};
#endif // CALOHYPONTP_H
