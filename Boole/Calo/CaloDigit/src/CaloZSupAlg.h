// $Id: CaloZSupAlg.h,v 1.1.1.1 2002-11-04 10:29:51 ocallot Exp $ 
#ifndef   CALODIGIT_CALOZSUPALG_H
#define   CALODIGIT_CALOZSUPALG_H 1
// ============================================================================
// from STL 
#include <cmath>
#include <string>
#include <vector>
// from Gaudi 
#include "GaudiKernel/IRndmGenSvc.h" 
// from CaloKernel
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloDititisationAlg   CaloDititisationAlg.h   
 *
 *  a (sub)Algorithm responsible 
 *  for digitisation of MC-information 
 *  
 *  @author: Olivier Callot
 *   @date:   5 June 2000
 */

class CaloZSupAlg : public CaloAlgorithm { 
  friend AlgFactory<CaloZSupAlg>;
public:
  
  // ** Constructor of this form must be provided
  
  CaloZSupAlg(const std::string& name, 
              ISvcLocator* pSvcLocator);
  
  // ** Standard Destructor
  
  virtual ~CaloZSupAlg();
  
  // ** Three mandatory member functions of any algorithm
  
  StatusCode initialize();
  StatusCode execute   ();
  StatusCode finalize  ();
  
protected:
  
private:   

  std::string m_inputMCData  ;    ///< Input data container
  std::string m_tagData ;       ///< tag for Z-sup, for Prs

  DeCalorimeter*         m_calo;           ///< Detector element pointer

  std::string m_zsupMethod        ; ///< Name of Zero Suppression method
  bool        m_zsup2D            ; ///< do we use 2D-zero sup. scheme ?
  int         m_zsupThreshold     ; ///< Initial threshold, in ADC counts 
      
  int         m_numberOfCells     ; ///< Number of cells of this detector. 
};


#endif //    CALODIGIT_CALOZSUPALG_H
