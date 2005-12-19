// $Id: MCSTDigitChecker.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _MCSTDigitChecker_H
#define _MCSTDigitChecker_H

#include <vector>
#include <string>

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
 class MCSTDigit;
};

/** @class MCSTDigitChecker MCSTDigitChecker.h
 *
 *  Class for checking MCSTDigits
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class MCSTDigitChecker : public GaudiHistoAlg{

public:
 
  /// constructer
  MCSTDigitChecker(const std::string& name, 
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

#endif // _MCSTDigitChecker_H













