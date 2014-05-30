// $Id: HltLumiWriter.h,v 1.3 2010-04-07 16:59:08 jpalac Exp $
#ifndef HLTLUMIWRITER_H 
#define HLTLUMIWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/RawEvent.h"

/** @class HltLumiWriter HltLumiWriter.h
 *  Fills the Raw Buffer banks for the LumiSummary
 *
 *  @author Jaap Panman
 *  @date   2004-07-22
 */
class HltLumiWriter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltLumiWriter( const std::string& name, ISvcLocator* pSvcLocator );

  ~HltLumiWriter( ) override = default; ///< Destructor

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution
  StatusCode finalize  () override;    ///< Algorithm finalization

private:

  void fillDataBankShort ( );

  std::string m_inputBank;

  // Statistics  
  double m_totDataSize;
  int m_nbEvents;
  std::vector<unsigned int> m_bank;
  LHCb::RawBank::BankType m_bankType;  
  std::string m_inputRawEventLocation;
  std::vector<std::string> m_rawEventLocations;

};
#endif // HLTLUMIWRITER_H
