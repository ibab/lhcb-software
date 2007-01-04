// $Id: MCSTDepositMonitor.h,v 1.1 2007-01-04 11:08:36 jvantilb Exp $
#ifndef MCSTDepositMonitor_H
#define MCSTDepositMonitor_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class MCSTDeposit;
};

class DeSTDetector;

/** @class MCSTDepositMonitor MCSTDepositMonitor.h
 *
 *  Class for monitoring MCSTDeposits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class MCSTDepositMonitor : public GaudiHistoAlg {

public:
 
  /// constructer
  MCSTDepositMonitor( const std::string& name, 
                      ISvcLocator *svcloc );

  /// destructer
  virtual ~MCSTDepositMonitor();

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

#endif // MCSTDepositMonitor_H
