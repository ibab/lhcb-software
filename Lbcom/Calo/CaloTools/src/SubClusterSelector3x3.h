// $Id: SubClusterSelector3x3.h,v 1.1 2001-11-08 10:58:34 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
