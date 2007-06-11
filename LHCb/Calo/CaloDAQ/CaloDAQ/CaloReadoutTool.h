// $Id: CaloReadoutTool.h,v 1.10 2007-06-11 15:45:48 odescham Exp $
#ifndef CALODAQ_CALOREADOUTTOOL_H 
#define CALODAQ_CALOREADOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "Event/RawEvent.h"
#include "CaloDet/DeCalorimeter.h"

static const InterfaceID IID_CaloReadoutTool ( "CaloReadoutTool", 1, 0 );

/** @class CaloReadoutTool CaloReadoutTool.h CaloDAQ/CaloReadoutTool.h
 *  
 * base class for Calo readout tools :
 * (CaloEnergyFromRaw, CaloTriggerAdcsFromRaw and CaloTriggerBitsFromRaw)
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-01
 */
class CaloReadoutTool : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_CaloReadoutTool; }

  /// Standard constructor
  CaloReadoutTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~CaloReadoutTool( ); ///< Destructor


  StatusCode getCaloBanksFromRaw();

protected:
  int findCardbyCode(std::vector<int> feCards, int code );
  void checkCards(int nCards, std::vector<int> feCards );
  std::string  m_detectorName;
  bool m_packedIsDefault; 
  const std::vector<LHCb::RawBank*>* m_banks;
  DeCalorimeter*   m_calo;
  LHCb::RawBank::BankType m_packedType;
  LHCb::RawBank::BankType m_shortType;
  bool m_getRaw;
  std::string m_raw;
  bool m_extraHeader;
  bool m_packed;
private:
};
#endif // CALODAQ_CALOREADOUTTOOL_H
