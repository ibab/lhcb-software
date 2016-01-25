#ifndef HCPEDESTALCORRECTIONTREE_H
#define HCPEDESTALCORRECTIONTREE_H 1

#include "TF2.h"
#include "TF1.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"


// Local
#include "HCMonitorBase.h"

/** @class HCPedestalCorrectionTree HCPedestalCorrectionTree.h
 *
 *
 */

class HCPedestalCorrectionTree : public HCMonitorBase {
 public:
  /// Standard constructor
  HCPedestalCorrectionTree(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~HCPedestalCorrectionTree();

  virtual StatusCode initialize();  ///< Algorithm initializatio
  virtual StatusCode finalize();    ///< Algorithm finalizationn
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  /// TES location of HC digits.
  std::string m_digitLocation;
  //std::vector<std::vector<std::vector<AIDA::IHistogram2D*> > > m_hCorrelation;
  //std::vector<std::vector<double> >  m_sig;
  //std::vector<std::vector<double> >  m_ref;
  double m_sig[10][10];
  double m_ref[10];
  //AIDA::IHistogram1D* m_bxID;
  



};
#endif
