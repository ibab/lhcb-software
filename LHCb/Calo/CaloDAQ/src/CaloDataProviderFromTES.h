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
  
  int    adc(LHCb::CaloCellID id,int def=0);
  double digit(LHCb::CaloCellID id,double def=0.);
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
  virtual LHCb::RawBankReadoutStatus& status(){m_status=LHCb::RawBankReadoutStatus();return m_status; }; // harmless method
  unsigned int nTell1s(){return 0;}; // harmless method
  void putStatusOnTES(){return;}; // harmless method
  virtual DeCalorimeter* deCalo(){return m_calo;}

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
  

  CaloVector<LHCb::CaloAdc>    m_adcs;
  CaloVector<LHCb::CaloDigit> m_digits;
  DeCalorimeter*   m_calo;
  //
  std::string  m_detectorName;
  std::string m_raw;
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
  LHCb::RawBankReadoutStatus m_status;
};
#endif // CALODATAPROVIDERFROMTES_H
