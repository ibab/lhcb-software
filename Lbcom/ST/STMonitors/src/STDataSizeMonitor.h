//$Id: STRawBankMonitor.h,v 1.2 2008-10-17 14:10:22 mneedham Exp $
#ifndef _STDataSizeMonitor_H
#define _STDataSizeMonitor_H

#include <string>

#include "Event/RawBank.h"

// base class
#include "Kernel/STHistoAlgBase.h"

/** @class STDataSizeMonitor STDataSizeMonitor.h 
 *
 *  Class for checking ST RAW buffer
 *
 *  @author M.Needham
 *  @date   21/11/2005
 *
 *  Move part to STMonitors to look at data size per tell1
 */

class STTell1ID;

namespace ST{
class STDataSizeMonitor : public ST::HistoAlgBase{

public:
 
  /// constructer
  STDataSizeMonitor(const std::string& name, 
                   ISvcLocator *svcloc );

  /// destructer
  virtual ~STDataSizeMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:
  
  StatusCode configureBankType();
 
  LHCb::RawBank::BankType m_bankType;

  unsigned int m_nTELL1s;
  AIDA::IProfile1D* m_prof_dataSizeVsTELL1;///< Mean data size vs TELL1

};
}
#endif // _STDataSizeMonitor














