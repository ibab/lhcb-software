// $Id: CaloDigitAlg.h,v 1.5 2005-01-12 09:14:33 ocallot Exp $ 
#ifndef   CALODIGIT_CALODIGITALG_H
#define   CALODIGIT_CALODIGITALG_H 1
// ============================================================================
// from Gaudi 
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiAlg/GaudiAlgorithm.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

/** @class CaloDigitAlg CaloDigitAlg.h   
 *
 *  a (sub)Algorithm responsible 
 *  for digitisation of MC-information 
 *  
 *  @author: Olivier Callot
 *   @date:   5 June 2000
 */

class CaloDigitAlg : public GaudiAlgorithm {  

public:
  CaloDigitAlg( const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~CaloDigitAlg();  ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  
  // ** Convert a floating ADC value to a digit: quantification + saturation
  
  int adcCount( double Value ) { 
    int  vadc  = (int) floor( Value + 0.5 );
    if ( vadc > m_maxAdcValue) { vadc = m_saturatedAdc ; } 
    return vadc;
  }
  
  IRndmGenSvc* rndmSvc() const  { return m_rndmSvc ; } 

private:   

  std::string m_detectorName;      ///< Detector element location
  std::string m_inputData;         ///< Input container
  std::string m_outputData;        ///< Output container
  std::string m_inputPrevData;  ///< Input data (prev.BX)

  DeCalorimeter*         m_calo;           ///< Detector element pointer
  mutable IRndmGenSvc*   m_rndmSvc;        ///< random number service 
 
  double      m_coherentNoise     ; ///< magnitude of coherent noise 
  double      m_incoherentNoise   ; ///< magnitude of incoherent noise
  double      m_gainError         ; ///< error in PMT gain
  double      m_fracPrev          ; ///< fraction of previous BX subtracted
  double      m_pedShift          ; ///< faction of noise giving pedestal shift
  double      m_pedestalShift     ; ///< pedestal shift due to noise
  double      m_pePerMeV          ; ///< Number of photo electrons per MeV
  double      m_deadCellFraction  ; ///< Fraction of dead cells

  int         m_numberOfCells     ; ///< Number of cells of this detector. 
  int         m_saturatedAdc      ; ///< Value to set in case ADC is saturated.
  int         m_maxAdcValue       ; ///< Maximum ADC content, after ped. substr
  double      m_activeToTotal     ; ///< Ratio activeE to total energy in ADC.
};
#endif //    CALODIGIT_CALODIGITALG_H
