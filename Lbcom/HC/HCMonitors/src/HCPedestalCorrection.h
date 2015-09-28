#ifndef HCPEDESTALCORRECTION_H 
#define HCPEDESTALCORRECTION_H 1

// Local 
#include "HCMonitorBase.h"

#include "TF2.h"
#include "AIDA/IHistogram2D.h"
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

  virtual StatusCode initialize();    ///< Algorithm initializatio
  virtual StatusCode finalize();    ///< Algorithm finalizationn
  virtual StatusCode execute();       ///< Algorithm execution

private:
  /// TAE slot
  int m_tag;
  /// TES location of HC digits.
  std::string m_digitLocation;
  std::map< std::string ,AIDA::IHistogram2D*> m_hCorrelation;
  std::map< std::string , TF2*> m_fit;
};
#endif
