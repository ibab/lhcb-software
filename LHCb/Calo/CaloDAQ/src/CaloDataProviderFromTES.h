// $Id: CaloDataProviderFromTES.h,v 1.3 2009-06-18 21:07:26 odescham Exp $
#ifndef CALODATAPROVIDERFROMTES_H 
#define CALODATAPROVIDERFROMTES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloDataProvider.h"            // Interface
#include "CaloDet/DeCalorimeter.h"

/** @class CaloDataProviderFromTES CaloDataProviderFromTES.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-08-22
 */
class CaloDataProviderFromTES : public GaudiTool, virtual public ICaloDataProvider {
public: 
  /// Standard constructor
  CaloDataProviderFromTES( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~CaloDataProviderFromTES( ); ///< Destructor


  virtual StatusCode initialize();
  virtual int    adc(LHCb::CaloCellID id);
  virtual double digit(LHCb::CaloCellID id);
  virtual CaloVector<LHCb::CaloAdc>& adcs(int source=-1);
  virtual CaloVector<LHCb::CaloDigit>& digits(int source=-1);
  virtual CaloVector<LHCb::CaloAdc>& adcs(std::vector<int> sources);
  virtual CaloVector<LHCb::CaloDigit>& digits(std::vector<int> sources);
  virtual void clear();
  virtual void cleanData(int feb);
  virtual bool getBanks();
  //
  std::string _rootInTES(){return rootInTES(); };
  StatusCode  _setProperty(const std::string& p,const std::string& v){return  setProperty(p,v);}; 
  //
  void setBanks(const std::vector<LHCb::RawBank*>* /* bank */ ){}; // harmless method
  LHCb::RawBankReadoutStatus status(){return LHCb::RawBankReadoutStatus(); }; // harmless method
  unsigned int nTell1s(){return 0;}; // harmless method
  void putStatusOnTES(){}; // harmless method


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
  bool m_getRaw;
  std::string m_raw;
  bool m_extraHeader;
  bool m_packed;
  bool m_cleanCorrupted;
  LHCb::RawBankReadoutStatus m_status;

  std::string m_data;
  std::string m_adcLoc;
  std::string m_digLoc;
  std::string m_loc;

  std::vector<int> m_readSources;
  
  LHCb::CaloDigits* m_digCont;
  LHCb::CaloAdcs* m_adcCont;


};
#endif // CALODATAPROVIDERFROMTES_H
