#ifndef   CALODIGIT_CALOZSUPALG_H
#define   CALODIGIT_CALOZSUPALG_H 1

// from Gaudi 
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiAlg/GaudiAlgorithm.h"

// from CaloKernel
#include "Kernel/CaloCellID.h"
#include "CaloKernel/CaloVector.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

// CaloDAQ
#include "CaloDAQ/ICaloEnergyFromRaw.h"

/** @class CaloZSupAlg CaloZSupAlg.h   
 *
 *  a (sub)Algorithm responsible 
 *  for digitisation of MC-information 
 *  
 *  @author: Olivier Callot
 *   @date:   5 June 2000
 */

class CaloZSupAlg : public GaudiAlgorithm { 
  friend class AlgFactory<CaloZSupAlg>;
public:
  
  CaloZSupAlg( const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~CaloZSupAlg();
  
  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  
protected:

private:   
  std::string m_detectorName;       ///< Detector element name
  std::string m_inputToolName;
  std::string m_inputToolType;
  std::string m_outputADCData;         ///< Output container for CaloAdc
  std::string m_outputDigitData;       ///< Output container for CaloDigit
  std::string m_zsupMethod        ; ///< Name of Zero Suppression method
  bool        m_zsup2D            ; ///< do we use 2D-zero sup. scheme ?
  int         m_zsupThreshold     ; ///< Initial threshold, in ADC counts 
  int         m_zsupNeighbor      ; ///< zsup (ADC) for neighbors for 2D method
  bool m_adcOnTES;
  bool m_digitOnTES;
  std::string m_outputType;
  std::string m_extension;
  

  DeCalorimeter*         m_calo;    ///< Detector element pointer
  ICaloEnergyFromRaw*    m_adcTool; ///< acces to adcs in Raw buffer
  int         m_numberOfCells     ; ///< Number of cells of this detector.
  bool m_statusOnTES;
};


#endif //    CALODIGIT_CALOZSUPALG_H
