#ifndef PROPERTIMECHECKER_H 
#define PROPERTIMECHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

#include "Kernel/Particle2MCLinker.h"


class ILifetimeFitter;
class IGeomDispCalculator;
class IContextTool;
class IRelatedPV;
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
 *  @author Yuehong Xie
 *  @date   2006-5-25
 */
class ProperTimeChecker : public DVAlgorithm {
public: 
  /// Standard constructor
  ProperTimeChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProperTimeChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  const LHCb::MCVertex* PV2MCVertex(const LHCb::RecVertex* pv);
  int countMatchedPVTrks(const LHCb::RecVertex* pv,  const LHCb::MCVertex* mcPV);
  const LHCb::RecVertex* MCPV2PV(const LHCb::MCVertex* mcpv);
  const LHCb::RecVertex* closestPV(const LHCb::Particle* part);

private:

  ILifetimeFitter* m_timeFitter;
  IContextTool* m_contextTool;
  const IGeomDispCalculator* m_geomTool;
  IRelatedPV* m_relatedPV;
  IPVReFitter* m_pvReFitter;

  Particle2MCLinker* m_pLinker;
  std::string m_particlePath;

//  Object2MCLinker<LHCb::Track>* m_track2MCLink;

  int m_pidToCheck;

  bool m_reFitPV;

};
#endif 
