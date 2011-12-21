#ifndef MCSTDepositMonitor_H
#define MCSTDepositMonitor_H 1

#include "Kernel/STHistoAlgBase.h"

namespace LHCb{
  class MCSTDeposit;
}

class DeSTDetector;

/** @class MCSTDepositMonitor MCSTDepositMonitor.h
 *
 *  Class for monitoring MCSTDeposits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class MCSTDepositMonitor : public ST::HistoAlgBase {

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

  void fillHistograms(const LHCb::MCSTDeposit* aDeposit) const;

  std::string m_depositLocation;
};

#endif // MCSTDepositMonitor_H
