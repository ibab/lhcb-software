// $Id: MCSTDepositChecker.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _MCSTDepositChecker_H
#define _MCSTDepositChecker_H

#include <vector>
#include <string>

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class MCSTDeposit;
};

class DeSTDetector;

/** @class MCSTDepositChecker MCSTDepositChecker.h
 *
 *  Class for checking MCSTDeposits
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class AIDA::IHistogram2D;

class MCSTDepositChecker : public GaudiHistoAlg {

public:
 
  /// constructer
  MCSTDepositChecker(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~MCSTDepositChecker();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();


private:

  virtual StatusCode initHistograms();
  virtual StatusCode fillHistograms(const LHCb::MCSTDeposit* aDeposit) const;

  DeSTDetector* m_tracker;
  std::string m_detType;
  std::string m_depositLocation;

  std::vector<IHistogram2D*> m_XvsYHistos;

};

#endif // _MCSTDepositChecker_H














