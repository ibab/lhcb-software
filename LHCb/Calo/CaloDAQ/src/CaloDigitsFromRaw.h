// $Id: CaloDigitsFromRaw.h,v 1.6 2007-12-06 09:31:24 odescham Exp $
#ifndef CALOEVENT_CALODIGITSFROMRAW_H 
#define CALOEVENT_CALODIGITSFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from LHCb
#include "Event/CaloDigit.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"
#include "CaloDAQ/ICaloEnergyFromRaw.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/RawEvent.h"
#include "Event/RawBankReadoutStatus.h"

/** @class CaloDigitsFromRaw CaloDigitsFromRaw.h component/CaloDigitsFromRaw.h
 *  Create the CaloDIgits containers from the Raw buffer
 *
 *  @author Olivier Callot
 *  @date   2003-11-18
 */
class CaloDigitsFromRaw : public GaudiAlgorithm {
public:
  /// Standard constructor
  CaloDigitsFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloDigitsFromRaw( ); ///< Destructor

  virtual StatusCode initialize ();    ///< Algorithm initialization
  virtual StatusCode execute    ();    ///< Algorithm execution

  class IncreasingByCellID {
  public:
    inline bool operator ()
      ( const LHCb::CaloDigit* dig1 , 
        const LHCb::CaloDigit* dig2 ) const {
      return 
      ( 0 == dig1 ) ? true  :
      ( 0 == dig2 ) ? false : 
      dig1->cellID().all() < dig2->cellID().all() ;
    }
  };

protected:
  
  void convertSpd( std::string containerName, double energyScale );
  
  void convertCaloEnergies( std::string containerName );

private: 
  
private:
  std::string m_extension ; ///< Added to the default container name, for tests
  int         m_detectorNum      ;

  ICaloTriggerBitsFromRaw* m_spdTool;
  ICaloEnergyFromRaw*  m_energyTool;

  bool m_adcOnTES;
  bool m_digitOnTES;
  std::string m_outputType;
  std::string m_pinContainerName;
  std::string m_outputDigits;
  std::string m_outputADCs;
  DeCalorimeter* m_calo;
  bool m_statusOnTES;  
};
#endif // CALOEVENT_CALODIGITSFROMRAW_H
