// $Id: CaloReadoutTool.h,v 1.6 2008-06-04 09:49:51 odescham Exp $
#ifndef CALODAQ_CALOREADOUTTOOL_H 
#define CALODAQ_CALOREADOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "CaloDAQ/ICaloReadoutTool.h"
#include "CaloDet/DeCalorimeter.h"






/** @class CaloReadoutTool CaloReadoutTool.h CaloDAQ/CaloReadoutTool.h
 *  
 * base class for Calo readout tools :
 * (CaloEnergyFromRaw, CaloTriggerAdcsFromRaw and CaloTriggerBitsFromRaw)
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-01
 */
class CaloReadoutTool : public GaudiTool , virtual public ICaloReadoutTool {
public: 


  /// Standard constructor
  CaloReadoutTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~CaloReadoutTool( ); ///< Destructor


  virtual std::string _rootInTES(){ return rootInTES(); };
  virtual StatusCode  _setProperty(const std::string& p,const std::string& v){return  setProperty(p,v);};
  
  // Useful methods  to set/get m_banks externally 
  // e.g. : avoid the call to getCaloBanksFromRaw() at each call of adc(bank)
  virtual bool getBanks(){
    m_getRaw = false;
    clear();
    return getCaloBanksFromRaw();    
  };
  virtual void setBanks(const std::vector<LHCb::RawBank*>* bank ){
    m_getRaw = false;
    clear();
    m_banks = bank;
  };
  virtual void clear(){return;}; // to be implemented in the parent tool
  virtual void cleanData(int ){return; } ;// to be implemented in the parent tool
  virtual LHCb::RawBankReadoutStatus status(){return m_status;};
  virtual void putStatusOnTES();
  

protected:
  bool getCaloBanksFromRaw();
  int findCardbyCode(std::vector<int> feCards, int code );
  bool checkCards(int nCards, std::vector<int> feCards );
  void checkCtrl(int word, int sourceID);
  bool CaloReadoutTool::checkSrc(int source);

  std::string  m_detectorName;
  bool m_packedIsDefault; 
  const std::vector<LHCb::RawBank*>* m_banks;
  DeCalorimeter*   m_calo;
  LHCb::RawBank::BankType m_packedType;
  LHCb::RawBank::BankType m_shortType;
  LHCb::RawBank::BankType m_errorType;
  bool m_getRaw;
  std::string m_raw;
  bool m_extraHeader;
  bool m_packed;
  bool m_cleanCorrupted;
  LHCb::RawBankReadoutStatus m_status;
  std::vector<int> m_readSources;
private:
  bool m_first;
};
#endif // CALODAQ_CALOREADOUTTOOL_H
