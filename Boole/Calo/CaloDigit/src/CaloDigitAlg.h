// $Id: CaloDigitAlg.h,v 1.6 2005-11-10 16:44:02 ocallot Exp $ 
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

  IRndmGenSvc* rndmSvc() const  { return m_rndmSvc ; } 

private:   

  std::string m_detectorName;      ///< Detector element location
  std::string m_inputData;         ///< Input container
  std::string m_outputData;        ///< Output container
  std::string m_inputPrevData;     ///< Input data (prev.BX)
  std::string m_triggerName;       ///< Name of the trigger bank

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
  double      m_triggerEtScale;    ///< Scale for trigger ADC
  double      m_triggerThreshold;  ///< Zero suppress for trigger
  int         m_zSupThreshold;     ///< Zero suppression for data, PreShower only
  bool        m_triggerIsBit;      ///< Trigger data is a bit bank
  std::vector<double> m_corrArea;  ///< Correction factor for trigger, area dependence

  int         m_numberOfCells     ; ///< Number of cells of this detector. 
  int         m_saturatedAdc      ; ///< Value to set in case ADC is saturated.
  int         m_maxAdcValue       ; ///< Maximum ADC content, after ped. substr
  double      m_activeToTotal     ; ///< Ratio activeE to total energy in ADC.
};
#endif //    CALODIGIT_CALODIGITALG_H
