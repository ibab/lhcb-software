// $Id: ClusterCovarianceMatrixEstimator.h,v 1.1 2001-11-04 15:30:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CLUSTERCOVARIANCEMATRIXESTIMATOR_H 
#define CLUSTERCOVARIANCEMATRIXESTIMATOR_H 1
// Include files
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"
// CaloGen 
#include "CaloGen/CaloTool.h"

// forwarde declaration
template <class TYPE>
class       ToolFactory;

/** @class ClusterCovarianceMatrixEstimator 
 *         ClusterCovarianceMatrixEstimator.h
 *  
 *  Concrete tool for calculation of covariance matrix 
 *  for the whole cluster object 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class ClusterCovarianceMatrixEstimator :
  public virtual ICaloClusterTool ,
  public          CaloTool
{
  /// factory for instantiation
  
protected:

  virtual ~ClusterCovarianceMatrixEstimator(){}; ///< Destructor

private: 

  /// Standard constructor
  ClusterCovarianceMatrixEstimator( ); 


protected:

private:

};

// ============================================================================
// The End 
// ============================================================================
#endif // CLUSTERCOVARIANCEMATRIXESTIMATOR_H
// ============================================================================
