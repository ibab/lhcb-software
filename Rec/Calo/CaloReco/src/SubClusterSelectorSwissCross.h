// $Id: SubClusterSelectorSwissCross.h,v 1.5 2010-03-08 01:19:40 odescham Exp $

#ifndef CALORECO_SUBCLUSTERSELECTORSwissCross_H 
#define CALORECO_SUBCLUSTERSELECTORSwissCross_H 1
// Include files
// CaloTools 
#include "SubClusterSelectorBase.h"
// CaloUtils 
#include "CaloUtils/CellSwissCross.h"


class SubClusterSelectorSwissCross  : public SubClusterSelectorBase{
  friend struct ToolFactory<SubClusterSelectorSwissCross>;
  
public:
  virtual StatusCode initialize(); 

  virtual StatusCode tag        ( LHCb::CaloCluster* cluster)const;
  virtual StatusCode untag      ( LHCb::CaloCluster* cluster ) const ;
  
protected:
  
  SubClusterSelectorSwissCross( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent );
  
  virtual ~SubClusterSelectorSwissCross();
  
private:
  
  CellSwissCross        m_matrix;
  
};

#endif 
