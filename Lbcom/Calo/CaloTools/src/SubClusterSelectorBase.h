// $Id: SubClusterSelectorBase.h,v 1.1 2001-11-08 10:58:35 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOTOOLS_SUBCLUSTERSELECTORBASE_H 
#define CALOTOOLS_SUBCLUSTERSELECTORBASE_H 1
// Include files
// from STL
// from CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"
// from CaloGen
#include "CaloGen/CaloTool.h"
// forwarde declaration 
class    CaloCluster   ; ///< from CaloEvent package

/** @class SubClusterSelectorBase SubClusterSelectorBase.h
 *  
 *  The common base clas for different "sub-cluster"
 *   selectors 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   07/11/2001
 */

class SubClusterSelectorBase : 
  public virtual  ICaloClusterTool ,
  public                  CaloTool 
{
  
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
  virtual StatusCode process    ( CaloCluster* cluster ) const  ;
  
  /** The main processing method with hypothesis 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @param hypo    processing hypothesis 
   *  @return status code 
   */  
  virtual StatusCode process    
  ( CaloCluster* cluster                   , 
    const CaloHypotheses::Hypothesis& hypo ) const ;

protected: 
  
  /** untag/unselect the all digits/subclusters in the clusters
   *  @param cluster pointer to cluster 
   *  @return status code 
   */
  StatusCode untag( CaloCluster* cluster ) const ;
  
protected:
  
  /** Standard Tool Constructor
   *  @param type type of the tool (useless ? )
   *  @param name name of the tool 
   *  @param parent the tool parent 
   */
  SubClusterSelectorBase( const std::string& type   , 
                          const std::string& name   ,
                          const IInterface*  parent );
  
  virtual ~SubClusterSelectorBase( ); ///< Destructor
  
protected:

};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORBASE_H
// ============================================================================
