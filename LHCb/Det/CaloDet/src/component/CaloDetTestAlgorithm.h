// $Id: CaloDetTestAlgorithm.h,v 1.1 2001-12-15 18:28:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALODET_CALODETTESTALGORITHM_H 
#define CALODET_CALODETTESTALGORITHM_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloDetTestAlgorithm CaloDetTestAlgorithm.h
 *  
 *  Simple Test         Algorithm 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   14/12/2001
 */

class CaloDetTestAlgorithm : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloDetTestAlgorithm>;

public:
  
  /** standard algorithm initialization 
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloDetTestAlgorithm( const std::string& name   , 
                        ISvcLocator*       svcloc );
  
  /** destructor
   */
  virtual ~CaloDetTestAlgorithm();
  
private:
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CaloDetTestAlgorithm_H
// ============================================================================
