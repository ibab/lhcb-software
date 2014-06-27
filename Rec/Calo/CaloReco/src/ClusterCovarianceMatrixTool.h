// $Id: ClusterCovarianceMatrixTool.h,v 1.5 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
#ifndef CLUSTERCOVARIANCEMATRIXTOOL_H 
#define CLUSTERCOVARIANCEMATRIXTOOL_H 1
// Include files
#include  "GaudiKernel/Property.h"
#include  "CaloInterfaces/ICaloClusterTool.h"
#include  "GaudiAlg/GaudiTool.h"
#include  "CaloUtils/CovarianceEstimator.h"
#include  "CaloCorrectionBase.h"


/** @class ClusterCovarianceMatrixTool 
 *         ClusterCovarianceMatrixTool.h
 *  
 *  Concrete tool for calculation of covariance matrix 
 *  for the whole cluster object 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class ClusterCovarianceMatrixTool:
  public virtual ICaloClusterTool ,
  public                 GaudiTool
{
  friend class ToolFactory<ClusterCovarianceMatrixTool>;

public:
  
  virtual StatusCode initialize ();
  virtual StatusCode finalize   ();
  virtual StatusCode process    ( LHCb::CaloCluster* cluster ) const ;  
  virtual StatusCode operator() ( LHCb::CaloCluster* cluster ) const ;
  
protected:

  ClusterCovarianceMatrixTool( const std::string& type   ,
                               const std::string& name   ,
                               const IInterface*  parent );
  
  /// destructor, virtual and protected 
  virtual ~ClusterCovarianceMatrixTool();

private:
  
  /// default constructor  is private 
  ClusterCovarianceMatrixTool();
  /// copy    constructor  is private 
  ClusterCovarianceMatrixTool
  ( const ClusterCovarianceMatrixTool& );  
  /// assignement operator is private 
  ClusterCovarianceMatrixTool& operator=( const ClusterCovarianceMatrixTool& );

private:
  
  CovarianceEstimator     m_estimator ;
  SimpleProperty<double>  m_a         ;
  SimpleProperty<double>  m_gainErr   ;
  SimpleProperty<double>  m_noiseIn   ;
  SimpleProperty<double>  m_noiseCo   ;
  std::string m_detData;
  const DeCalorimeter* m_det;
  bool m_useDB;
  CaloCorrectionBase*      m_dbAccessor;
  std::string m_conditionName;
  }; ///< end of class ClusterCovarianceMatrixTool
// ============================================================================
#endif // CLUSTERCOVARIANCEMATRIXTOOL_H
