// $Id: MCSTDigitChecker.h,v 1.2 2006-12-22 12:23:01 jvantilb Exp $
#ifndef MCSTDigitChecker_H
#define MCSTDigitChecker_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class MCSTDigit;
};

/** @class MCSTDigitChecker MCSTDigitChecker.h
 *
 *  Class for monitoring MCSTDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class MCSTDigitChecker : public GaudiHistoAlg{

public:
 
  /// constructer
  MCSTDigitChecker( const std::string& name, 
                    ISvcLocator *svcloc );

  /// destructer
  virtual ~MCSTDigitChecker();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  StatusCode fillHistograms(const LHCb::MCSTDigit* aDigit) const;
  
  std::string m_detType;
  std::string m_digitLocation;
  std::string m_depositLocation;

};

#endif // MCSTDigitChecker_H













