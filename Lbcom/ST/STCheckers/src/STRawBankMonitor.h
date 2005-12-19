//$Id: STRawBankMonitor.h,v 1.1.1.1 2005-12-19 15:44:30 mneedham Exp $
#ifndef _STRawBankMonitor_H
#define _STRawBankMonitor_H

#include <string>

#include "Event/RawBank.h"

// base class
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class STRawBankMonitor STRawBankMonitor.h 
 * ITCheckers/STRawBankMonitor.h
 *
 *  Class for checking ST RAW buffer
 *
 *  @author M.Needham
 *  @date   21/11/2005
 */

class STTell1ID;

class STRawBankMonitor : public GaudiHistoAlg{

public:
 
  /// constructer
  STRawBankMonitor(const std::string& name, 
                   ISvcLocator *svcloc );

  /// destructer
  virtual ~STRawBankMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:
  
  StatusCode configureBankType();
 
  std::string m_type;
  LHCb::RawBank::BankType m_bankType;

};

#endif // _STRawBankMonitor














