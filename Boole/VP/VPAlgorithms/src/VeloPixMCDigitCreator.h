#// $Id: VeloPixMCDigitCreator.h,v 1.1.1.1 2009-12-04 14:16:52 marcin Exp $
#ifndef VELOPIXMCDIGITCREATOR_H
#define VELOPIXMCDIGITCREATOR_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class VeloPixMCDigitCreator.h 
 *  VeloPixAlgorithms/VeloPixMCDigitCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   20/09/09
 */

class VeloPixMCDigitCreator : public GaudiAlgorithm {
public:
  /// Standard constructor
  VeloPixMCDigitCreator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VeloPixMCDigitCreator();   ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalise

protected:

private:

  void createMCDigits(const LHCb::MCVeloPixDeposits* depositCont,
                      LHCb::MCVeloPixDigits* digitsCont);
  bool keepAdding(const LHCb::MCVeloPixDeposit* firstDep,
                  const LHCb::MCVeloPixDeposit* secondDep) const;
  std::string m_inputLocation;
  std::string m_outputLocation;
  std::vector<std::string> m_sampleNames;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;
  std::vector<std::string> m_outPaths;
  bool m_isDebug; 
  bool m_isVerbose;

};
#endif // VeloPixMCDigitCreator_H
