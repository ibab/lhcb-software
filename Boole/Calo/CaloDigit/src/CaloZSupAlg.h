// $Id: CaloZSupAlg.h,v 1.6 2005-01-18 12:41:58 ocallot Exp $ 
#ifndef   CALODIGIT_CALOZSUPALG_H
#define   CALODIGIT_CALOZSUPALG_H 1

// from Gaudi 
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiAlg/GaudiAlgorithm.h"

// from CaloKernel
#include "CaloKernel/CaloVector.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

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

  int bufferNumber( const CaloCellID& ID )  {
    int det  = ID.calo();
    int area = ID.area();
    int row  = ID.row();
    int col  = ID.col();
    int bNum = 0;

    //== PreShower: 1 TELL1 per region, except the outer one with 2 boards.

    if ( 1 >= det ) {    // PRS
      bNum = 1 + area;
      if ( 0 == area && 34 > row ) bNum = 0;  //== Two L1 boards
      if ( 31 < col ) bNum += 4;

      //== ECAL : 2 TELL1 per region, except in region 2. 10 boards in total

    } else if ( 2 == det ) {  // ECAL
      bNum = 2*area;
      if ( 0 == area && 33 < row ) bNum += 1; //== Two L1 boards
      if ( 1 == area && 31 < row ) bNum += 1; //== Two L1 boards
      if ( 31 < col ) bNum += 5;

      //== HCAL : 4 Tell1 boards, 1 per region. 4 in total

    } else if ( 3 == det ) {
      bNum = area;
      if ( 15 < col ) bNum += 2;
    }
    return bNum;
  }
  
private:   
  std::string m_detectorName;       ///< Detector element name
  std::string m_inputData;          ///< Input container
  std::string m_outputData;         ///< Output container
  std::string m_inputMCData;        ///< Input data container
  std::string m_zsupMethod        ; ///< Name of Zero Suppression method
  std::string m_triggerName       ; ///< Name of the trigegr bank
  bool        m_zsup2D            ; ///< do we use 2D-zero sup. scheme ?
  int         m_zsupThreshold     ; ///< Initial threshold, in ADC counts 
  double      m_energyScale       ; ///< To convert energy to RawBuffer
  double      m_triggerEtScale    ; ///< Scale for trigger ADC
  double      m_triggerThreshold  ; ///< Zero suppress for trigger
  bool        m_triggerIsBit;       ///< Trigger data is a bit bank
  std::vector<double> m_corrArea  ; ///< Correction factor, area dependence

  DeCalorimeter*         m_calo;           ///< Detector element pointer
      
  int         m_numberOfCells     ; ///< Number of cells of this detector.

};


#endif //    CALODIGIT_CALOZSUPALG_H
