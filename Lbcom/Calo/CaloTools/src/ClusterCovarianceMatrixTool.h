// $Id: ClusterCovarianceMatrixTool.h,v 1.6 2002-04-07 18:15:01 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/12/09 14:33:09  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.4  2001/11/25 15:50:40  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.3  2001/11/23 11:44:51  ibelyaev
//  new tool for calculation of cluster spread
//
// Revision 1.2  2001/11/08 10:58:34  ibelyaev
//  new tools are added for selection of subclusters within the cluster
//
// Revision 1.1  2001/11/04 15:30:40  ibelyaev
// add concrete tool
//
// ============================================================================
#ifndef CLUSTERCOVARIANCEMATRIXTOOL_H 
#define CLUSTERCOVARIANCEMATRIXTOOL_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/Property.h"
// CaloInterfaces
#include  "CaloInterfaces/ICaloClusterTool.h"
// CaloKernel
#include  "CaloKernel/CaloTool.h"
// CaloUtil 
#include  "CaloUtils/CovarianceEstimator.h"

class CovarianceEstimator;

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
  public                 CaloTool
{
  /// friend class for instantiation
  friend class ToolFactory<ClusterCovarianceMatrixTool>;

public:
  
  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode initialize ();
  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode finalize   ();
  

  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloCluster* cluster ) const ;
  
  /** The main processing method (functor interface) 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloCluster* cluster ) const ;
  
protected:

  /** Standard constructor
   *  @param type tool type (useless)
   *  @param name tool name
   *  @param parent pointer to parent object (service, algorithm or tool)  
   */
  ClusterCovarianceMatrixTool
  ( const std::string& type   ,
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
  ClusterCovarianceMatrixTool& operator=
  ( const ClusterCovarianceMatrixTool& );

private:
  
  ///
  CovarianceEstimator     m_estimator ;
  ///
  SimpleProperty<double>  m_a         ;
  SimpleProperty<double>  m_gainErr   ;
  SimpleProperty<double>  m_noiseIn   ;
  SimpleProperty<double>  m_noiseCo   ;
  ///
  
}; ///< end of class ClusterCovarianceMatrixTool


// ============================================================================
// The End 
// ============================================================================
#endif // CLUSTERCOVARIANCEMATRIXTOOL_H
// ============================================================================
