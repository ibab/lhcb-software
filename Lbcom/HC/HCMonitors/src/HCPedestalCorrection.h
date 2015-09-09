#ifndef HC_DIGIT_MONITOR_H
#define HC_DIGIT_MONITOR_H 1

// AIDA
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Local
#include "HCMonitorBase.h"

/** @class HCPedestalCorrection HCPedestalCorrection.h
 *
 *  Algorithm to determine pedestal correlations between 
 *  signal and spare channels.
 *
 */

class HCPedestalCorrection : public HCMonitorBase {
 public:
  /// Standard constructor
  HCPedestalCorrection(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCPedestalCorrection();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution
  virtual StatusCode finalize();    ///< Algorithm finalization

 private:
  /// TES location of digits
  std::string m_digitLocation;

  /// Correlation histograms for each quadrant.
  std::vector<AIDA::IHistogram2D*> m_hCorrelation;
  /// Profile histograms for each quadrant.
  std::vector<AIDA::IProfile1D*> m_hProfile;
};

#endif
