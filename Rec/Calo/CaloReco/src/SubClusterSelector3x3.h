// $Id: SubClusterSelector3x3.h,v 1.1.1.1 2002-11-13 20:46:43 ibelyaev Exp $
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
// Revision 1.1  2001/11/08 10:58:34  ibelyaev
//  new tools are added for selection of subclusters within the cluster
// 
// ============================================================================
#ifndef CALOTOOLS_SUBCLUSTERSELECTOR3x3_H 
#define CALOTOOLS_SUBCLUSTERSELECTOR3x3_H 1
// Include files
// CaloTools 
#include "SubClusterSelectorBase.h"
// CaloUtils 
#include "CaloUtils/CellMatrix3x3.h"

/** @class SubClusterSelector3x3 SubClusterSelector3x3.h
 *  
 *  The simplest concrete "subcluster selector" - 
 *  it just tag/select digits which form
 *  3x3 sub-matrix centered with the seed cells 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   07/11/2001
 */

class SubClusterSelector3x3
  : public SubClusterSelectorBase  
{
  /// frined factory for instantiation
  friend class ToolFactory<SubClusterSelector3x3>;
  
public:

  /** standard initiliazation 
   *  @see SubClusterSelectorBase 
   *  @see CaloTool 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize(); 
  
  /** The main processing method
   *
   *  @see ICaloSubClusterTag
   *
   *  Error codes:  
   *    - 225   cluster points to NULL
   *    - 226   empty cell/digit container for given cluster 
   *    - 227   SeedCell is not found 
   *    - 228   Seed points to NULL
   *
   *  @see ICaloClusterTool
   *  @see ICaloSubClusterTag
   *
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode tag        ( CaloCluster* cluster ) const ;
  
  /** The main method - "undo" of tagging from "tag" method  
   *  @see ICaloSubClusterTag
   *  @param cluster pointer to ClaoCluster object to be untagged 
   *  @return status code 
   */
  virtual StatusCode untag      ( CaloCluster* cluster ) const ;
  
protected:
  
  /** Standard Tool Constructor
   *  @param type type of the tool (useless ? )
   *  @param name name of the tool 
   *  @param parent the tool parent 
   */
  SubClusterSelector3x3( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent );
  
  /** destructor 
   */
  virtual ~SubClusterSelector3x3();
  
private:
  
  CellMatrix3x3        m_matrix;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTOR3x3_H
// ============================================================================
