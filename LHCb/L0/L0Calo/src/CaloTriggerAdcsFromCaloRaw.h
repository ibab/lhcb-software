#ifndef CALOTRIGGERADCSFROMCALORAW_H 
#define CALOTRIGGERADCSFROMCALORAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"            // Interface

class DeCalorimeter ;
class ICaloDataProvider ;

/** @class CaloTriggerAdcsFromCaloRaw CaloTriggerAdcsFromCaloRaw.h
 *  Compute Trigger Adcs from Calo Adcs
 *
 *  @author Patrick Robbe
 *  @date   2014-03-04
 */
class CaloTriggerAdcsFromCaloRaw : public GaudiTool, virtual public ICaloTriggerAdcsFromRaw {
public: 
  /// Standard constructor
  CaloTriggerAdcsFromCaloRaw( const std::string& type, 
                              const std::string& name,
                              const IInterface* parent);

  virtual ~CaloTriggerAdcsFromCaloRaw( ); ///< Destructor

  virtual StatusCode initialize() ;

  virtual const std::vector<LHCb::L0CaloAdc>&    adcs( ) ;
  virtual const std::vector<LHCb::L0CaloAdc>&    adcs( int source ) ;
  virtual const std::vector<LHCb::L0CaloAdc>&    adcs( LHCb::RawBank* bank ) ;
  virtual const std::vector<LHCb::L0CaloAdc>&    pinAdcs( ) ;

  virtual std::string _rootInTES() { return "" ; } ;
  virtual StatusCode  _setProperty(const std::string& ,const std::string& ) { return StatusCode::SUCCESS ; } ;
  virtual bool getBanks() { return true ; } ;
  virtual void setBanks(const std::vector<LHCb::RawBank*>* ) { return ; } ;
  virtual void clear() { m_data.clear() ; }
  virtual void cleanData(int ) { clear() ; } ;
  virtual LHCb::RawBankReadoutStatus& status() {
    m_theSt = LHCb::RawBankReadoutStatus( LHCb::RawBank::EcalTrig ) ; 
    return m_theSt ; 
  } ;
  virtual void putStatusOnTES() { return ; } ;
  virtual bool ok() { return true ; } ;
  virtual DeCalorimeter* deCalo() { return m_calo ; } ;

protected:

private:
  LHCb::RawBankReadoutStatus m_theSt ;
  std::vector< LHCb::L0CaloAdc > m_data ;
  std::string m_detectorName ;
  ICaloDataProvider * m_adcs ;
  DeCalorimeter * m_calo ;

  bool m_doubleScale ;

  int l0adcFromAdc( const int adc , const LHCb::CaloCellID & id ) const ;
};
#endif // CALOTRIGGERADCSFROMCALORAW_H
