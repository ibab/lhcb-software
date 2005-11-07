// $Id: CaloPhotonMomentumTool.h,v 1.2 2005-11-07 12:12:43 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:41  ibelyaev
// new package 
//
// Revision 1.1  2002/06/13 12:32:39  ibelyaev
//  version update
// 
// ============================================================================
#ifndef CALOPHOTONMOMENTUMTOOL_H 
#define CALOPHOTONMOMENTUMTOOL_H 1
// Include files
// from STL
#include <string>
// CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloUtil
#include "CaloUtils/CaloPhotonMomentum.h"
// from Kernel
#include "Kernel/CaloHypotheses.h"

/** @class CaloPhotonMomentumTool CaloPhotonMomentumTool.h
 *
 *  Concrete tool for evaluation of 4-momentumn of the photon  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   18/05/2002
 */

class CaloPhotonMomentumTool:
  public virtual ICaloHypoTool,
  public virtual      CaloTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloPhotonMomentumTool>;

public:
  
  /** The main processing method. 
   *
   *  Return codes:
   *  
   *   - 500 : CaloHypo* points to NULL
   *  
   *   - 501 : Invalid Hypothesis
   *
   *   - 502 : CaloPosition* points to NULL for hypo object
   *
   *   - 503 : CaloMomentum* is already estimated for hypo object
   *
   *   - 504 : CaloMomentum* could not be estimated for hypo object
   *
   *  @see ICaloHypo
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface).
   *
   *  Return codes:
   *  
   *   - 500 : CaloHypo* points to NULL
   *  
   *   - 501 : Invalid Hypothesis
   *
   *   - 502 : CaloPosition* points to NULL for hypo object
   *
   *   - 503 : CaloMomentum* is already estimated for hypo object
   *
   *   - 504 : CaloMomentum* could not be estimated for hypo object
   *
   *  @see ICaloHypo
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
  virtual StatusCode    initialize ();
  
protected: 
  
  /** Standard constructor
   *  @see CaloTool
   *  @see  AlgTool
   *  @param type tool type (?) 
   *  @param name tool name 
   *  @param parent pointer to the parent
   */
  CaloPhotonMomentumTool
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~CaloPhotonMomentumTool();
  
private:
  
  ///   default   constructor   is   private 
  CaloPhotonMomentumTool();
  ///   copy      constructor   is   private 
  CaloPhotonMomentumTool
  (const CaloPhotonMomentumTool& );
  ///   assignement  operator   is   private 
  CaloPhotonMomentumTool& operator=
  (const CaloPhotonMomentumTool& );  
  
private:
  
  /// evaluator 
  CaloPhotonMomentum                              m_evaluator ;
  
  typedef std::vector<CaloHypotheses::Hypothesis> Hypotheses  ;
  Hypotheses                                      m_hypos     ;

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOPHOTONMOMENTUMTOOL_H
// ============================================================================
