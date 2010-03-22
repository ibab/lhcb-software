// $Id:
#ifndef CALODATAPROVIDER_H 
#define CALODATAPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloDataProvider.h"            // Interface
#include "CaloReadoutTool.h"

/** @class CaloDataProvider CaloDataProvider.h
 *  Fast access to calorimeter data from raw
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-27
 */
class CaloDataProvider : public CaloReadoutTool, virtual public ICaloDataProvider {

public: 
  /// Standard constructor
  CaloDataProvider( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloDataProvider( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  void clear();
  void cleanData(int feb);
  int    adc(LHCb::CaloCellID id, int def = 0);
  double digit(LHCb::CaloCellID id, double def = 0.);
  unsigned int nTell1s(){return m_tell1s;};
  const CaloVector<LHCb::CaloAdc>& adcs(int source=-1,bool clean=true);
  const CaloVector<LHCb::CaloAdc>& adcs(std::vector<int> source,bool clean=true);
  const CaloVector<LHCb::CaloDigit>& digits(int source=-1,bool clean=true); 
  const CaloVector<LHCb::CaloDigit>& digits(std::vector<int> sources,bool clean=true);
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

protected:
  void adc2digit();
  bool decodeCell(LHCb::CaloCellID id);
  bool decodeTell1(int tell1);
  bool decodeBank(LHCb::RawBank* bank);
  bool decodePrsTriggerBank(LHCb::RawBank* bank);
private:
  LHCb::CaloAdc fillAdc(LHCb::CaloCellID id, int adc){
    LHCb::CaloAdc temp(id,adc);
    if( id.area() != CaloCellCode::PinArea ){
      if( adc < m_minADC.adc()  )m_minADC = temp;
      if( adc > m_maxADC.adc()  )m_maxADC = temp;
    }else{
      if( adc < m_minPinADC.adc() )m_minPinADC = temp;
      if( adc > m_maxPinADC.adc() )m_maxPinADC = temp;
    }     
    return temp;
  }
  double   m_pedShift;
  CaloVector<LHCb::CaloAdc>    m_adcs;
  CaloVector<LHCb::CaloDigit> m_digits;
  unsigned int m_tell1s;
  LHCb::CaloAdc m_minADC;
  LHCb::CaloAdc m_minPinADC;
  LHCb::CaloAdc m_maxADC;
  LHCb::CaloAdc m_maxPinADC;
};
#endif // CALODATAPROVIDER_H
