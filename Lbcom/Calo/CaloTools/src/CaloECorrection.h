// $Id: CaloECorrection.h,v 1.1 2002-06-21 11:02:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOEX_CALOECorrection_H 
#define CALOEX_CALOECorrection_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from CaloUtils 
#include "CaloUtils/ClusterFunctors.h"

/** @class CaloECorrection CaloECorrection.h
 *
 *  Logitudinal correction.
 *  The actual "code" is imported from Olivier Deschamps code 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */

class CaloECorrection: 
  public virtual ICaloHypoTool , 
  public              CaloTool  
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloECorrection>;
  
public:
  
  /** The main processing method
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* hypo  ) const ;
  
  /** standard initialization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;

  /** standard finalization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service or tool)
   */
  CaloECorrection
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloECorrection() ;
  
private:

  ///  default  constructor   is   private
  CaloECorrection();
  ///  copy     constructor   is   private
  CaloECorrection( const CaloECorrection& );
  ///  assignement operator   is   private
  CaloECorrection& operator=( const CaloECorrection& );
  
private:

  // correction coefficients per area
  std::vector<double>          m_const           ;
  // allowed hypos
  typedef std::vector<int>     Hypotheses        ;
  Hypotheses                   m_hypos           ;
  
  // evaluator fo "cluster area"
  ClusterFunctors::ClusterArea m_area            ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOECorrection_H
// ============================================================================
