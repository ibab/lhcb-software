// $Id: SubClusterSelectorSwissCross.h,v 1.2 2005-11-07 12:12:44 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// Revision 1.3  2002/04/07 18:15:04  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.2  2001/11/08 20:07:06  ibelyaev
//  new tools are added into  the package
//
// Revision 1.1  2001/11/08 10:58:35  ibelyaev
//  new tools are added for selection of subclusters within the cluster
// 
// ============================================================================
#ifndef CALOTOOLS_SUBCLUSTERSELECTORSwissCross_H 
#define CALOTOOLS_SUBCLUSTERSELECTORSwissCross_H 1
// Include files
// CaloTools 
#include "SubClusterSelectorBase.h"
// CaloUtils 
#include "CaloUtils/CellSwissCross.h"

/** @class SubClusterSelectorSwissCross SubClusterSelectorSwissCross.h
 *  
 *  The simplest concrete "subcluster selector" - 
 *  it just tag/select digits which form
 *  SwissCross sub-matrix centered with the seed cells 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   07/11/2001
 */

class SubClusterSelectorSwissCross
  : public SubClusterSelectorBase  
{
  /// frined factory for instantiation
  friend class ToolFactory<SubClusterSelectorSwissCross>;
  
public:  

  /** standard initiliazation 
   *  @see SubClusterSelectorBase
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlTool
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
  SubClusterSelectorSwissCross( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent );
  
  /// destructor 
  virtual ~SubClusterSelectorSwissCross();
  
private:

  ///  default constructor  is private 
  SubClusterSelectorSwissCross();
  ///  copy    constructor  is private 
  SubClusterSelectorSwissCross
  ( const SubClusterSelectorSwissCross& );
  ///  assigenemtn operator is private 
  SubClusterSelectorSwissCross& operator=
  ( const SubClusterSelectorSwissCross& );

private:
  
  CellSwissCross        m_matrix;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORSwissCross_H
// ============================================================================
