#ifndef OLDVPMCDIGITCREATOR_H
#define OLDVPMCDIGITCREATOR_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class OldVPMCDigitCreator.h 
 *  VPDigitisation/OldVPMCDigitCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   20/09/09
 */

class OldVPMCDigitCreator : public GaudiAlgorithm {
public:
  /// Standard constructor
  OldVPMCDigitCreator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~OldVPMCDigitCreator();   ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

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
#endif // OldVPMCDigitCreator_H
