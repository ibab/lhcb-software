// $Id: STDigitMonitor.h,v 1.2 2007-01-10 16:02:23 cattanem Exp $
#ifndef STDigitMonitor_H
#define STDigitMonitor_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
 class STDigit;
};

class DeSTDetector;

/** @class STDigitMonitor STDigitMonitor.h
 *
 *  Class for monitoring STDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STDigitMonitor : public GaudiHistoAlg{

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

  DeSTDetector* m_tracker;
  std::string m_detType; 
  std::string m_dataLocation;
};

#endif // STDigitMonitor_H
