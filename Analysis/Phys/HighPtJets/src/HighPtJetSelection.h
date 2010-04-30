#ifndef HighPtJetSelection_H
#define HighPtJetSelection_H 1
// Include files:
// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class HighPtJetSelection HighPtJetSelection.h
 *
 *
 *  @author Marcin Kucharczyk
 *  @date   2010-04-03
 */

class HighPtJetSelection : public DVAlgorithm {
public:
  HighPtJetSelection(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  virtual ~HighPtJetSelection();
  // Algorithm initialization
  virtual StatusCode initialize();
  // Algorithm execution
  virtual StatusCode execute();
  // Algorithm finalization
  virtual StatusCode finalize();

private:
  const IJetMaker* m_jetMaker;
  std::string m_jetMakerName;
  std::string m_inputPVs;
  double m_minChargedPartPt;
  double m_minNeutralPartE;
  double m_min1stJetPt;
  double m_min2ndJetPt;
  unsigned int m_jetMult;

};
#endif // HighPtJetSelection_H
