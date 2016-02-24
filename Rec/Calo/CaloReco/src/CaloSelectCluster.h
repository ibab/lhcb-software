//
// ============================================================================
#ifndef CALORECO_CALOSELECTCLUSTER_H
#define CALORECO_CALOSELECTCLUSTER_H 1
// ============================================================================
#include <string>
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICaloClusterSelector.h"
// ============================================================================

class CaloSelectCluster : 
  public virtual ICaloClusterSelector ,
  public          GaudiTool
{
  /// friend factory for instantiation
  friend struct ToolFactory<CaloSelectCluster>;
public:
  
  
public:
  
  virtual bool select( const LHCb::CaloCluster* cluster ) const ;  
  virtual bool operator()( const LHCb::CaloCluster* cluster ) const ;
  virtual StatusCode initialize () ;
  
public:
  
  
protected:
  
  CaloSelectCluster( const std::string& type   , 
                            const std::string& name   ,
                            const IInterface*  parent );  
  virtual ~CaloSelectCluster(); 

private:  
  CaloSelectCluster(); 
  CaloSelectCluster(const CaloSelectCluster& ); 
  CaloSelectCluster& operator=(const CaloSelectCluster& );

private:
  double m_cut;
  double m_etCut;
  int m_mult;
  int m_multMin;
};
#endif // CALORECO_CALOSELECTCLUSTER_H
