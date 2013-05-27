// ============================================================================
#ifndef CALODET_CALODETTESTALGORITHM_H 
#define CALODET_CALODETTESTALGORITHM_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "GaudiAlg/GaudiAlgorithm.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h" 

/** @class CaloDetTestAlgorithm CaloDetTestAlgorithm.h
 *  
 *  Simple Test         Algorithm 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   14/12/2001
 */

class CaloDetTestAlgorithm : public GaudiAlgorithm 
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
  std::string m_DetData;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CaloDetTestAlgorithm_H
// ============================================================================
