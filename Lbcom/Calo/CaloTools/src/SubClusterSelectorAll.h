// $Id: SubClusterSelectorAll.h,v 1.1 2001-11-08 10:58:35 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOTOOLS_SUBCLUSTERSELECTORALL_H 
#define CALOTOOLS_SUBCLUSTERSELECTORALL_H 1
// Include files
#include "SubClusterSelectorBase.h"

/** @class SubClusterSelectorAll SubClusterSelectorAll.h
 *  
 *  The simplest concrete "subcluster selector" - 
 *  it just select the whole cluster 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   07/11/2001
 */

class SubClusterSelectorAll
  : public SubClusterSelectorBase  
{
  /// frined factory for instantiation
  friend class ToolFactory<SubClusterSelectorAll>;
  
public:  
  
  /** The main processing method (functor interface) 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloCluster* cluster ) const ;
  
protected:
  
  /** Standard Tool Constructor
   *  @param type type of the tool (useless ? )
   *  @param name name of the tool 
   *  @param parent the tool parent 
   */
  SubClusterSelectorAll( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent );
  
  /** destructor 
   */
  virtual ~SubClusterSelectorAll();
  
private:
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORALL_H
// ============================================================================
