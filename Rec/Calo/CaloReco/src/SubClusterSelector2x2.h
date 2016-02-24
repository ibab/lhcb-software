// $Id: SubClusterSelector3x3.h,v 1.5 2010-03-08 01:19:40 odescham Exp $
#ifndef CALORECO_SUBCLUSTERSELECTOR2x2_H
#define CALORECO_SUBCLUSTERSELECTOR2x2_H 1
// Include files
#include "SubClusterSelectorBase.h"
#include "CaloUtils/CellMatrix2x2.h"


class SubClusterSelector2x2: public SubClusterSelectorBase{
  friend struct ToolFactory<SubClusterSelector2x2>;
  
public:

  virtual StatusCode initialize(); 
  virtual StatusCode tag        ( LHCb::CaloCluster* cluster ) const ;
  virtual StatusCode untag      ( LHCb::CaloCluster* cluster ) const ;

  
protected:

  StatusCode choice2x2 (LHCb::CaloCluster* cluster,
                        CellMatrix2x2::SubMatrixType &type,
                        double &energy) const ;
  
  SubClusterSelector2x2( const std::string& type   ,
                         const std::string& name   ,
                         const IInterface*  parent );
  
  virtual ~SubClusterSelector2x2();
  
private:
  const DeCalorimeter* m_det;
  CellMatrix2x2        m_matrix;  
};

#endif // SUBCLUSTERSELECTOR2X2_H
