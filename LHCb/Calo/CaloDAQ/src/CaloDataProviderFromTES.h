// $Id: CaloDataProviderFromTES.h,v 1.7 2009-10-27 10:11:26 odescham Exp $
#ifndef CALODATAPROVIDERFROMTES_H  
#define CALODATAPROVIDERFROMTES_H 1 

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloDataProvider.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
#include "CaloDet/DeCalorimeter.h"
/** @class CaloDataProviderFromTES CaloDataProviderFromTES.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-08-22
 */
class CaloDataProviderFromTES 
  : public GaudiTool
    , virtual public ICaloDataProvider 
    , virtual public IIncidentListener{

public: 
  /// Standard constructor
  CaloDataProviderFromTES( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~CaloDataProviderFromTES( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  // =========================================================================
  /// Inform that a new incident has occurred
  virtual void handle(const Incident& /* inc */ ) { 
    m_getRaw = true;
  }
  // =========================================================================
  
  int    adc(LHCb::CaloCellID id);
  double digit(LHCb::CaloCellID id);
  const CaloVector<LHCb::CaloAdc>& adcs(int source=-1,bool clean=true);
  const CaloVector<LHCb::CaloDigit>& digits(int source=-1,bool clean=true);
  const CaloVector<LHCb::CaloAdc>& adcs(std::vector<int> sources,bool clean=true);
  const CaloVector<LHCb::CaloDigit>& digits(std::vector<int> sources,bool clean=true);
  void clear();
  void cleanData(int feb);
  bool getBanks();
  ICaloDataProvider::CaloAdcPair adcRange(){
    LHCb::CaloAdc min = (m_minADC.cellID() == LHCb::CaloCellID()) ? LHCb::CaloAdc(LHCb::CaloCellID(), 0) : m_minADC;
    LHCb::CaloAdc max = (m_maxADC.cellID() == LHCb::CaloCellID()) ? LHCb::CaloAdc(LHCb::CaloCellID(), 0) : m_maxADC;
    return std::make_pair(min,max) ;
  }
  ICaloDataProvider::CaloAdcPair pinRange(){ 
    LHCb::CaloAdc min = (m_minPinADC.cellID() == LHCb::CaloCellID()) ? LHCb::CaloAdc(LHCb::CaloCellID(), 0) : m_minPinADC;
    LHCb::CaloAdc max = (m_maxPinADC.cellID() == LHCb::CaloCellID()) ? LHCb::CaloAdc(LHCb::CaloCellID(), 0) : m_maxPinADC;
    return std::make_pair(min,max) ;
  }
  bool ok(){
    if(m_getRaw)getBanks();
    return m_ok;
  };

  
  //
  std::string _rootInTES(){return rootInTES(); };
  StatusCode  _setProperty(const std::string& p,const std::string& v){return  setProperty(p,v);}; 
  //
  void setBanks(const std::vector<LHCb::RawBank*>* /* bank */ ){}; // harmless method
  LHCb::RawBankReadoutStatus status(){return LHCb::RawBankReadoutStatus(); }; // harmless method
  unsigned int nTell1s(){return 0;}; // harmless method
  void putStatusOnTES(){return;}; // harmless method

protected:

private:
  bool checkSrc(int source);
  bool fromAdc(){
    std::string out( m_data );
    std::transform( m_data.begin() , m_data.end() , out.begin () , ::toupper ) ;
    if( out == "ADC" || out == "CALOADC" || out == "ADCS" || out == "CALOADCS")return true;
    return false;
  }
  bool fromDigit(){
    std::string out( m_data );
    std::transform( m_data.begin() , m_data.end() , out.begin () , ::toupper ) ;
    if( out == "DIGIT" || out == "CALODIGIT" || out == "DIGITS" || out == "CALODIGITS")return true;
    return false;
  }
  


  double   m_pedShift;
  CaloVector<LHCb::CaloAdc>    m_adcs;
  CaloVector<LHCb::CaloDigit> m_digits;
  DeCalorimeter*   m_calo;
  unsigned int m_tell1s;
  //
  std::string  m_detectorName;
  bool m_packedIsDefault; 
  const std::vector<LHCb::RawBank*>* m_banks;
  LHCb::RawBank::BankType m_packedType;
  LHCb::RawBank::BankType m_shortType;
  LHCb::RawBank::BankType m_errorType;
  std::string m_raw;
  bool m_extraHeader;
  bool m_packed;
  bool m_cleanCorrupted;
  std::string m_data;
  std::string m_adcLoc;
  std::string m_digLoc;
  std::string m_loc;
  std::vector<int> m_readSources;  
  LHCb::CaloDigits* m_digCont;
  LHCb::CaloAdcs* m_adcCont;
  bool m_ok;
  LHCb::CaloAdc m_minADC;
  LHCb::CaloAdc m_minPinADC;
  LHCb::CaloAdc m_maxADC;
  LHCb::CaloAdc m_maxPinADC;
  bool m_getRaw;
};
#endif // CALODATAPROVIDERFROMTES_H
