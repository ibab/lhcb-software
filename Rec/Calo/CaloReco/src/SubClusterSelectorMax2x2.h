// $Id: SubClusterSelectorMax2x2.h,v 1.2 2005-03-07 15:37:15 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOTOOLS_SUBCLUSTERSELECTORMAX2X2_H 
#define CALOTOOLS_SUBCLUSTERSELECTORMAX2X2_H 1
// Include files
// CaloTools 
#include "SubClusterSelectorBase.h"
// CaloUtils 
#include "CaloUtils/CellMatrix2x2.h"
//
/** @class SubClusterSelectorMax2x2 SubClusterSelectorMax2x2.h
 *  
 *  The simplest concrete "subcluster selector" - 
 *  it just tag/select digits which form
 *  2x2 sub-matrix with maximal energy deposition 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/11/2001
 */

class SubClusterSelectorMax2x2 
  : public SubClusterSelectorBase
{
  /// frined factory for instantiation
  friend class ToolFactory<SubClusterSelectorMax2x2>;
public:
  
  /** standard initiliazation 
   *  @see SubclusterSelectorBase 
   *  @see CaloTool 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize();
  
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
  SubClusterSelectorMax2x2( const std::string& type   , 
                            const std::string& name   ,
                            const IInterface*  parent );
  
  /// destructor 
  virtual ~SubClusterSelectorMax2x2();
  
private:

  /// default  constructor  is private 
  SubClusterSelectorMax2x2();
  /// copy     constructor  is private 
  SubClusterSelectorMax2x2 
  ( const SubClusterSelectorMax2x2& );
  /// assignement operator  is private 
  SubClusterSelectorMax2x2& operator=
  ( const SubClusterSelectorMax2x2& );

private:
  
  /** tag the certain submatrix 
   *  @param  cluster pointer to cluster 
   *  @param  Type    submatrix type 
   *  @return status code 
   */
  StatusCode tag2x2
  ( CaloCluster* cluster                    ,
    const CellMatrix2x2::SubMatrixType Type ) const ;
  
  /** tag the certain submatrix 
   *  @param  cluster pointer to cluster 
   *  @param  Type    submatrix type 
   *  @return status code 
   */
  StatusCode untag2x2
  ( CaloCluster* cluster                    ,
    const CellMatrix2x2::SubMatrixType Type ) const ;
  
private:

  CellMatrix2x2    m_matrix ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORMAX2X2_H
// ============================================================================
