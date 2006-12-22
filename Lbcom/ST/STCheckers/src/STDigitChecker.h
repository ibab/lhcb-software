// $Id: STDigitChecker.h,v 1.2 2006-12-22 12:23:01 jvantilb Exp $
#ifndef STDigitChecker_H
#define STDigitChecker_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
 class STDigit;
};

class ISTSignalToNoiseTool;
class DeSTDetector;

/** @class STDigitChecker STDigitChecker.h
 *
 *  Class for monitoring STDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STDigitChecker : public GaudiHistoAlg{

public:
 
  /// constructer
  STDigitChecker( const std::string& name, 
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

#endif // STDigitChecker_H














