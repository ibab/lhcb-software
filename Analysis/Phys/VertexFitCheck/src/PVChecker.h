#ifndef PVCHECKER_H 
#define PVCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

#include "Kernel/Particle2MCLinker.h"

class IContextTool;
class IRelatedPV;

/** @class PVChecker PVChecker.h
 *  
 *  PVChecker is an algorithm to check
 *  primary vertex fit performances
 *  using MC truth. It fills a ntuple with
 *  fitted values, true values and pulls of PV position.
 *
 *  @author Yuehong Xie
 *  @date   2006-5-25
 */
class PVChecker : public DVAlgorithm {
public: 
  /// Standard constructor
  PVChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  const LHCb::MCVertex* PV2MCVertex(const LHCb::RecVertex* pv, int & nTrMatched);
  int countMatchedPVTrks(const LHCb::RecVertex* pv,  const LHCb::MCVertex* mcPV);
  const LHCb::RecVertex* MCPV2PV(const LHCb::MCVertex* mcpv, int & nTrMatched);

private:

  IContextTool* m_contextTool;
  IRelatedPV* m_relatedPV;

};
#endif 
