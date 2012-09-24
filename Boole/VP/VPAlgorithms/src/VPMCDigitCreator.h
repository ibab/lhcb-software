#// $Id: VPMCDigitCreator.h,v 1.1.1.1 2009-12-04 14:16:52 marcin Exp $
#ifndef VPMCDIGITCREATOR_H
#define VPMCDIGITCREATOR_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class VPMCDigitCreator.h 
 *  VPAlgorithms/VPMCDigitCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   20/09/09
 */

class VPMCDigitCreator : public GaudiAlgorithm {
public:
  /// Standard constructor
  VPMCDigitCreator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VPMCDigitCreator();   ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalise

protected:

private:

  void createMCDigits(const LHCb::MCVPDeposits* depositCont,
                      LHCb::MCVPDigits* digitsCont);
  bool keepAdding(const LHCb::MCVPDeposit* firstDep,
                  const LHCb::MCVPDeposit* secondDep) const;
  std::string m_inputLocation;
  std::string m_outputLocation;
  std::vector<std::string> m_sampleNames;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;
  std::vector<std::string> m_outPaths;
  bool m_isDebug; 
  bool m_isVerbose;

};
#endif // VPMCDigitCreator_H
