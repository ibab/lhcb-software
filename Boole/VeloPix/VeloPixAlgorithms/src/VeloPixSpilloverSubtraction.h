#ifndef VELOPIXSPILLOVERSUBTRACTION_H
#define VELOPIXSPILLOVERSUBTRACTION_H 1
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/VeloPixDigit.h"
#include <string>

/** @class VeloPixSpilloverSubtraction VeloPixSpilloverSubtraction.h
 *
 *  Class for killing VeloPix spillover 
 *
 *  @author M. Kucharczyk
 *  @date   19/10/2009
 */

namespace LHCb {
  class VeloPixChannelID;
}

class VeloPixSpilloverSubtraction : public GaudiAlgorithm {
public:
  /// Standard constructor
  VeloPixSpilloverSubtraction(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~VeloPixSpilloverSubtraction();  //< Destructor
  virtual StatusCode initialize();         //< Algorithm initialization
  virtual StatusCode execute   ();         //< Algorithm execution
  virtual StatusCode finalize  ();         //< Algorithm finalise

protected:

private:
  void mvDigits(LHCb::VeloPixDigits* digitContCentr,
                LHCb::VeloPixDigits* digitContPrev,
                LHCb::VeloPixDigits* digitContPrevPrev,
                std::vector<LHCb::VeloPixDigit*>& selDigits);
  std::string m_inputLocation;
  bool m_switchOn;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;

};

#endif // VELOPIXSPILLOVERSUBTRACTION_H
