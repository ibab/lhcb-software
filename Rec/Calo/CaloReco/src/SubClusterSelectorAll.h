// $Id: SubClusterSelectorAll.h,v 1.1.1.1 2002-11-13 20:46:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/07 18:15:02  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.2  2001/11/08 20:07:05  ibelyaev
//  new tools are added into  the package
//
// Revision 1.1  2001/11/08 10:58:35  ibelyaev
//  new tools are added for selection of subclusters within the cluster
// 
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
  
  /** The main processing method  
   *  @see ICaloSubClusterTag 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode tag 
  ( CaloCluster* cluster ) const ;
  
  /** The main processing method  
   *  @see ICaloSubClusterTag 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode untag 
  ( CaloCluster* cluster ) const ;
  
protected:
  
  /** Standard Tool Constructor
   *  @param type type of the tool (useless ? )
   *  @param name name of the tool 
   *  @param parent the tool parent 
   */
  SubClusterSelectorAll
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor 
  virtual ~SubClusterSelectorAll();
  
private:
  
  /// default constructor   is  private 
  SubClusterSelectorAll();
  /// copy    constructor   is  private 
  SubClusterSelectorAll
  ( const SubClusterSelectorAll& );
  /// assignement operator  is  private 
  SubClusterSelectorAll* operator=
  ( const SubClusterSelectorAll& );
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORALL_H
// ============================================================================
