// $Id: MCSTDepositChecker.h,v 1.2 2006-12-22 12:23:00 jvantilb Exp $
#ifndef MCSTDepositChecker_H
#define MCSTDepositChecker_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class MCSTDeposit;
};

class DeSTDetector;

/** @class MCSTDepositChecker MCSTDepositChecker.h
 *
 *  Class for monitoring MCSTDeposits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class MCSTDepositChecker : public GaudiHistoAlg {

public:
 
  /// constructer
  MCSTDepositChecker( const std::string& name, 
                      ISvcLocator *svcloc );

  /// destructer
  virtual ~MCSTDepositChecker();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  StatusCode fillHistograms(const LHCb::MCSTDeposit* aDeposit) const;

  DeSTDetector* m_tracker;
  std::string m_detType;
  std::string m_depositLocation;
};

#endif // MCSTDepositChecker_H
