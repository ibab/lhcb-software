//
// ============================================================================
#ifndef CALORECO_CALOSELECTCLUSTERWITHPRS_H
#define CALORECO_CALOSELECTCLUSTERWITHPRS_H 1
// ============================================================================
#include <string>
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
// ============================================================================

class CaloSelectClusterWithPrs : 
  public virtual ICaloClusterSelector ,
  public          GaudiTool
{
  /// friend factory for instantiation
  friend struct ToolFactory<CaloSelectClusterWithPrs>;
public:
  
  
public:
  
  virtual bool select( const LHCb::CaloCluster* cluster ) const ;  
  virtual bool operator()( const LHCb::CaloCluster* cluster ) const ;
  virtual StatusCode initialize () ;
  
public:
  
  
protected:
  
  CaloSelectClusterWithPrs( const std::string& type   , 
                            const std::string& name   ,
                            const IInterface*  parent );  
  virtual ~CaloSelectClusterWithPrs(); 

private:  
  CaloSelectClusterWithPrs(); 
  CaloSelectClusterWithPrs(const CaloSelectClusterWithPrs& ); 
  CaloSelectClusterWithPrs& operator=(const CaloSelectClusterWithPrs& );

private:
  double m_cut;
  double m_mult;
  ICaloHypo2Calo* m_toPrs;
  std::string m_det;
};
#endif // CALORECO_CALOSELECTCLUSTERWITHPRS_H
