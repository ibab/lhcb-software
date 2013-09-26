#ifndef OLDVPSPILLOVERSUBTRACTION_H
#define OLDVPSPILLOVERSUBTRACTION_H 1
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/VPDigit.h"
#include <string>

/** @class OldVPSpilloverSubtraction OldVPSpilloverSubtraction.h
 *
 *  Class for killing VP spillover 
 *
 *  @author M. Kucharczyk
 *  @date   19/10/2009
 */

namespace LHCb {
  class VPChannelID;
}

class OldVPSpilloverSubtraction : public GaudiAlgorithm {
public:
  /// Standard constructor
  OldVPSpilloverSubtraction(const std::string& name,
                              ISvcLocator* pSvcLocator);
  virtual ~OldVPSpilloverSubtraction();  //< Destructor
  virtual StatusCode initialize();         //< Algorithm initialization
  virtual StatusCode execute   ();         //< Algorithm execution

protected:

private:
  void mvDigits(LHCb::VPDigits* digitContCentr,
                LHCb::VPDigits* digitContPrev,
                LHCb::VPDigits* digitContPrevPrev,
                std::vector<LHCb::VPDigit*>& selDigits);
  std::string m_inputLocation;
  bool m_switchOn;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;

};

#endif // OLDVPSPILLOVERSUBTRACTION_H
