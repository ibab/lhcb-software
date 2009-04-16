//
// ============================================================================
#ifndef CALORECO_CALOSELECTNEUTRALCLUSTERWITHSPD_H
#define CALORECO_CALOSELECTNEUTRALCLUSTERWITHSPD_H 1
// ============================================================================
#include <string>
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
// ============================================================================

class CaloSelectNeutralClusterWithSpd : 
  public virtual ICaloClusterSelector ,
  public          GaudiTool
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectNeutralClusterWithSpd>;
public:
  
  
public:
  
  virtual bool select( const LHCb::CaloCluster* cluster ) const ;  
  virtual bool operator()( const LHCb::CaloCluster* cluster ) const ;
  virtual StatusCode initialize () ;
  
public:
  
  
protected:
  
  CaloSelectNeutralClusterWithSpd( const std::string& type   , 
                                   const std::string& name   ,
                                   const IInterface*  parent );  
  virtual ~CaloSelectNeutralClusterWithSpd(); 

private:  
  CaloSelectNeutralClusterWithSpd(); 
  CaloSelectNeutralClusterWithSpd(const CaloSelectNeutralClusterWithSpd& ); 
  CaloSelectNeutralClusterWithSpd& operator=(const CaloSelectNeutralClusterWithSpd& );

private:
  int    m_cut;
  ICaloHypo2Calo* m_toSpd;
  std::string m_det;
};
#endif // CALORECO_CALOSELECTNEUTRALCLUSTERWITHSPD_H
