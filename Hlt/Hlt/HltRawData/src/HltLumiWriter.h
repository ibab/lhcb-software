// $Id: HltLumiWriter.h,v 1.1 2008-08-19 11:03:23 graven Exp $
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

  virtual ~HltLumiWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void fillDataBankShort ( );

private:
  std::string m_inputBank;

  int m_LastKey;
  // Statistics  
  double m_totDataSize;
  int m_nbEvents;
  std::vector<unsigned int> m_bank;
  LHCb::RawBank::BankType m_bankType;  

};
#endif // HLTLUMIWRITER_H
