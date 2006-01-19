// $Id: CaloFillRawBuffer.h,v 1.5 2006-01-19 12:18:51 cattanem Exp $
#ifndef CALOFILLRAWBUFFER_H 
#define CALOFILLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawEvent.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloFillRawBuffer CaloFillRawBuffer.h
 *  Fills the Raw Buffer banks for the calorimeter
 *
 *  @author Olivier Callot
 *  @date   2004-12-17
 */
class CaloFillRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloFillRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloFillRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void fillDataBankShort ( );

  void fillPackedBank ( );

  void fillTriggerBank ( );

private:
  std::string m_inputBank;
  std::string m_triggerBank;
  std::string m_detectorName;
  std::string m_detectorLocation;
  LHCb::RawBank::BankType m_bankType;
  LHCb::RawBank::BankType m_triggerBankType;
  int    m_numberOfBanks;
  int    m_dataCodingType;

  DeCalorimeter* m_calo;
  CaloReadoutTool* m_roTool;

  // Statistics
  
  double m_totDataSize;
  std::vector<double> m_dataSize;
  double m_totTrigSize;
  int m_nbEvents;
  std::vector< std::vector<unsigned int> > m_banks;
  std::vector< std::vector<unsigned int> > m_trigBanks;
};
#endif // CALOFILLRAWBUFFER_H
