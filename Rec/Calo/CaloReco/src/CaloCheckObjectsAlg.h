// $Id: CaloCheckObjectsAlg.h,v 1.1 2004-09-05 15:25:19 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Lof:$
// ============================================================================
#ifndef CALORECO_CALOCHECKOBJECTSALG_H 
#define CALORECO_CALOCHECKOBJECTSALG_H 1
// ============================================================================
// Include files
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================


/** @class CaloCheckObjectsAlg CaloCheckObjectsAlg.h
 *
 *  Simple algorithm to check the existence of some objects in TES.
 *  also useful to load items 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru 
 *  @date   2004-09-05
 */
class CaloCheckObjectsAlg : public CaloAlgorithm
{
  friend class AlgFactory<CaloCheckObjectsAlg> ; 
  
public:
  
  /// execution of the algorithm 
  virtual StatusCode execute() ;
  
protected: 
  
  /** Standard constructor
   *  @param name algorithm instance name 
   *  @param psvc pointer to Service Locator 
   */ 
  CaloCheckObjectsAlg
  ( const std::string& name , 
    ISvcLocator*       psvc ) ; 
  
  virtual ~CaloCheckObjectsAlg();
  
private:
  
  CaloCheckObjectsAlg();
  CaloCheckObjectsAlg           ( const  CaloCheckObjectsAlg& ) ;
  CaloCheckObjectsAlg& operator=( const  CaloCheckObjectsAlg& ) ;
  
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALORECO_CALOCHECKOBJECTSALG_H
// ============================================================================
