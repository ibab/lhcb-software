// $Id: SubClusterSelectorSwissCross.h,v 1.1 2001-11-08 10:58:35 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
   *  @return status code 
   */
  virtual StatusCode initialize();
  
  
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
  SubClusterSelectorSwissCross( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent );
  
  /** destructor 
   */
  virtual ~SubClusterSelectorSwissCross();
  
private:
  
  CellSwissCross        m_matrix;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORSwissCross_H
// ============================================================================
