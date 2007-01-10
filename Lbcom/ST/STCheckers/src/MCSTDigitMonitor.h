// $Id: MCSTDigitMonitor.h,v 1.2 2007-01-10 16:02:22 cattanem Exp $
#ifndef MCSTDigitMonitor_H
#define MCSTDigitMonitor_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class MCSTDigit;
};

/** @class MCSTDigitMonitor MCSTDigitMonitor.h
 *
 *  Class for monitoring MCSTDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class MCSTDigitMonitor : public GaudiHistoAlg{

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
  
  std::string m_detType;
  std::string m_digitLocation;
  std::string m_depositLocation;

};

#endif // MCSTDigitMonitor_H













