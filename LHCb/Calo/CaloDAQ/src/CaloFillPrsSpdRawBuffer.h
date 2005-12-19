// $Id: CaloFillPrsSpdRawBuffer.h,v 1.4 2005-12-19 19:29:14 ocallot Exp $
#ifndef CALOFILLPRSSPDRAWBUFFER_H 
#define CALOFILLPRSSPDRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "CaloDet/DeCalorimeter.h"
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloFillPrsSpdRawBuffer CaloFillPrsSpdRawBuffer.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2005-01-04
 */
class CaloFillPrsSpdRawBuffer : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloFillPrsSpdRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloFillPrsSpdRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void fillDataBank ( );

  void fillDataBankShort ( );

  void fillTriggerBank ( );

  void fillTriggerBankShort ( );

  void fillPackedBank ( );

private:
  std::string m_inputBank;
  std::string m_prsBank;
  std::string m_spdBank;
  LHCb::RawBank::BankType m_bankType;
  LHCb::RawBank::BankType m_triggerBankType;
  int    m_numberOfBanks;
  int    m_dataCodingType;

  DeCalorimeter* m_calo;
  CaloReadoutTool* m_roTool;

  // Statistics
  
  double m_totDataSize;
  double m_totTrigSize;
  std::vector<double> m_dataSize;
  int m_nbEvents;
  std::vector< std::vector<unsigned int> > m_banks;
  std::vector< std::vector<unsigned int> > m_trigBanks;
};
#endif // CALOFILLPRSSPDRAWBUFFER_H
