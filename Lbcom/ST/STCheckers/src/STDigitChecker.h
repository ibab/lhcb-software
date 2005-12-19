// $Id: STDigitChecker.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _STDigitChecker_H
#define _STDigitChecker_H

#include <vector>
#include <string>
#include <map>

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
 class STDigit;
};

class ISTSignalToNoiseTool;
class DeSTDetector;

/** @class STDigitChecker STDigitChecker.h
 *
 *  Class for checking STDigits
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class STDigitChecker : public GaudiHistoAlg{

public:
 
  /// constructer
  STDigitChecker(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STDigitChecker();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();


private:

  virtual StatusCode fillHistograms(const LHCb::STDigit* aDigit) ;

  DeSTDetector* m_tracker;
  std::string m_detType; 
 
  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;

  std::string m_dataLocation;


};

#endif // _STDigitChecker_H














