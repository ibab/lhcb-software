// $Id: CaloReadoutTool.h,v 1.12 2009-10-12 16:03:54 odescham Exp $
#ifndef CALODAQ_CALOREADOUTTOOL_H 
#define CALODAQ_CALOREADOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
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
class CaloReadoutTool 
  : public GaudiTool 
    , virtual public ICaloReadoutTool 
    , virtual public IIncidentListener{
public: 

  /// Standard constructor
  CaloReadoutTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~CaloReadoutTool( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual std::string _rootInTES(){ return rootInTES(); };
  virtual StatusCode  _setProperty(const std::string& p,const std::string& v){return  setProperty(p,v);};
  
  // Useful methods  to set/get m_banks externally 
  // e.g. : avoid the call to getCaloBanksFromRaw() at each call of adc(bank)
  virtual bool getBanks(){
    counter("getCaloBanks") += 1;
    m_getRaw = false;
    clear();
    m_ok = getCaloBanksFromRaw();    
    return m_ok;
  };
  virtual void setBanks(const std::vector<LHCb::RawBank*>* bank ){
    m_getRaw = false;
    clear();
    m_banks = bank;
    m_ok = true;
  };
  //actual implementation MUST BE in the parent tool
  virtual void clear(){Warning("DUMMY CLEARING : THIS MESSAGE MUST NOT APPEAR").ignore() ; return;}; 
  //
  virtual void cleanData(int ){return; } ;// to be implemented in the parent tool
  virtual LHCb::RawBankReadoutStatus status(){return m_status;};
  virtual void putStatusOnTES();
  virtual bool ok(){
    if(m_getRaw)getBanks() ;
    return m_ok;
  };
  

  // =========================================================================
  /// Inform that a new incident has occurred
  virtual void handle(const Incident& /* inc */ ) { 
    debug() << "IIncident Svc reset" << endmsg;
    m_getRaw = true ;  
  } 
  // =========================================================================

protected:  
  
  bool getCaloBanksFromRaw();
  int findCardbyCode(std::vector<int> feCards, int code );
  bool checkCards(int nCards, std::vector<int> feCards );
  void checkCtrl(int word, int sourceID);
  bool checkSrc(int source);

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
  bool m_ok;
private:
  bool m_first;
};
#endif // CALODAQ_CALOREADOUTTOOL_H
