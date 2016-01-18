#ifndef HCPEDESTALCORRECTION_H
#define HCPEDESTALCORRECTION_H 1

#include "TF2.h"
#include "TF1.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"


// Local
#include "HCMonitorBase.h"

/** @class HCPedestalCorrection HCPedestalCorrection.h
 *
 *
 */

class HCPedestalCorrection : public HCMonitorBase {
 public:
  /// Standard constructor
  HCPedestalCorrection(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCPedestalCorrection();

  virtual StatusCode initialize();  ///< Algorithm initializatio
  virtual StatusCode finalize();    ///< Algorithm finalizationn
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  /// TES location of HC digits.
  std::string m_digitLocation;
  std::vector<std::vector<std::vector<AIDA::IHistogram2D*> > > m_hCorrelation;
  AIDA::IHistogram1D* m_bxID;
  


};
#endif
