// $Id: SubClusterSelectorMax2x2.h,v 1.1 2001-11-08 20:07:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
   *  @return status code 
   */
  virtual StatusCode initialize();
  
  /** The main processing method (functor interface) 
   *
   *  Error codes:  
   *    - 225   cluster points to NULL
   *    - 226   empty cell/digit container for given cluster 
   *    - 227   SeedCell is not found 
   *    - 228   Seed points to NULL
   *
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
  SubClusterSelectorMax2x2( const std::string& type   , 
                            const std::string& name   ,
                            const IInterface*  parent );
  
  /** destructor 
   */
  virtual ~SubClusterSelectorMax2x2();
  
private:

  /** tag the certain submatrix 
   *  @param  cluster poiunter to cluster 
   *  @param  seed    seed cell
   *  @param  type    submatrix type 
   *  @return status code 
   */
  StatusCode tag( CaloCluster* cluster                    ,
                  const CaloCellID&                  seed ,
                  const CellMatrix2x2::SubMatrixType Type ) const ;
  
private:

  CellMatrix2x2    m_matrix ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORMAX2X2_H
// ============================================================================
