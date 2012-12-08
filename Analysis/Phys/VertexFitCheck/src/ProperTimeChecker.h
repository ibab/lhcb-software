// $Id: ProperTimeChecker.h,v 1.7 2010-01-18 08:45:30 pkoppenb Exp $
#ifndef PROPERTIMECHECKER_H
#define PROPERTIMECHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

#include "Kernel/Particle2MCLinker.h"

class ILifetimeFitter;
class IPVReFitter;

/** @class ProperTimeChecker ProperTimeChecker.h
 *
 *  ProperTimeChecker is an algorithm to check
 *  vertex fit and propertime fit performances
 *  using MC truth. Taking as input the reconstructed
 *  B (or other) particles, it fills a ntuple with
 *  fitted values, true values and pulls of PV+SV
 *  position, momentum and proper time.
 *
 *  @todo This algorithm duplicates some functionality of decaytreetuple
 *  Can it be removed? Can some variables be obtained from a loaded tuple tool?
 *
 *  @author Yuehong Xie
 *  @date   2006-5-25
 */
class ProperTimeChecker : public DaVinciTupleAlgorithm
{

public:
  /// Standard constructor
  ProperTimeChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProperTimeChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  const LHCb::MCVertex* PV2MCVertex(const LHCb::RecVertex* pv);
  int countMatchedPVTrks(const LHCb::RecVertex* pv,  const LHCb::MCVertex* mcPV);
  const LHCb::RecVertex* MCPV2PV(const LHCb::MCVertex* mcpv);
  const LHCb::RecVertex* closestPV(const LHCb::Particle* part);

private:

  ILifetimeFitter* m_timeFitter;
  IPVReFitter* m_pvReFitter;
  IPVReFitter* m_cheatedPVReFitter;

  Particle2MCLinker* m_pLinker;
  std::string m_particlePath;

  //  Object2MCLinker<LHCb::Track>* m_track2MCLink;

  int m_pidToCheck;

  bool m_reFitPV;
  bool m_removeBFromPV;
  bool m_removeMCSecondaryFromPV;

  std::string m_PVReFitterName;

};
#endif
