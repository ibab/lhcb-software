// $Id: SubClusterSelector3x3.h,v 1.5 2010-03-08 01:19:40 odescham Exp $
#ifndef CALORECO_SUBCLUSTERSELECTOR3x3_H 
#define CALORECO_SUBCLUSTERSELECTOR3x3_H 1
// Include files
#include "SubClusterSelectorBase.h"
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

class SubClusterSelector3x3  : public SubClusterSelectorBase{
  friend struct ToolFactory<SubClusterSelector3x3>;
  
public:

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

  virtual StatusCode tag ( LHCb::CaloCluster* cluster) const ;
  
  /** The main method - "undo" of tagging from "tag" method  
   *  @see ICaloSubClusterTag
   *  @param cluster pointer to ClaoCluster object to be untagged 
   *  @return status code 
   */
  virtual StatusCode untag      ( LHCb::CaloCluster* cluster ) const ;
  
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

#endif // SUBCLUSTERSELECTOR3x3_H
