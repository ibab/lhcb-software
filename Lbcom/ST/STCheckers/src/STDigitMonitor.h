#ifndef STDigitMonitor_H
#define STDigitMonitor_H 1

#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
 class STDigit;
}

class DeSTDetector;

/** @class STDigitMonitor STDigitMonitor.h
 *
 *  Class for monitoring STDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STDigitMonitor : public ST::HistoAlgBase{

public:
 
  /// constructer
  STDigitMonitor( const std::string& name, 
                  ISvcLocator *svcloc );

  /// destructer
  virtual ~STDigitMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  void fillHistograms(const LHCb::STDigit* aDigit) ;

  std::string m_dataLocation;
};

#endif // STDigitMonitor_H
