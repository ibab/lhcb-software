#ifndef MCSTDigitMonitor_H
#define MCSTDigitMonitor_H 1

#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
  class MCSTDigit;
}

/** @class MCSTDigitMonitor MCSTDigitMonitor.h
 *
 *  Class for monitoring MCSTDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class MCSTDigitMonitor : public ST::HistoAlgBase{

public:
 
  /// constructer
  MCSTDigitMonitor( const std::string& name, 
                    ISvcLocator *svcloc );

  /// destructer
  virtual ~MCSTDigitMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  void fillHistograms(const LHCb::MCSTDigit* aDigit) const;
  

  std::string m_digitLocation;


};

#endif // MCSTDigitMonitor_H













