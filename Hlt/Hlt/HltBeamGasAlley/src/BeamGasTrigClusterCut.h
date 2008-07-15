// $Id: BeamGasTrigClusterCut.h,v 1.1.1.1 2008-07-15 13:53:25 phopchev Exp $
#ifndef INCLUDE_BEAMGASTRIGCLUSTERCUT_H
#define INCLUDE_BEAMGASTRIGCLUSTERCUT_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

namespace Tf { class DefaultVeloRHitManager; }
  
/** @class BeamGasTriggerClusterCut BeamGasTrigClusterCut.h
 * A simple HltAlgorithm that counts the number of unused R clusters
 * by the PatVeloRTracking. Uses a subset of the VELO Stations,
 * set by two job-options. If the fraction of unused R clusters
 * is above a given treshold (set by a job-option) it gives a positive 
 * decision. 
 *
 * @author Plamen Hopchev <plamen.hopchev@cern.ch>
 * @date   2008-06-16
 */
class BeamGasTrigClusterCut : public GaudiAlgorithm {

public:

  /// Standard Constructor
  BeamGasTrigClusterCut(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~BeamGasTrigClusterCut(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode    execute(); ///< Algorithm event execution
  virtual StatusCode   finalize(); ///< Algorithm finalize

private:
  std::string 	m_hitManagerName;
  int		m_sensorsBegin;		///< The first sensor used for counting the unused clusters
  int		m_sensorsEnd;		///< The last sensor used for counting the unused clusters
  float         m_leftClustersFracTrig; ///< Cut to decide further BG inspectation  
  
  Tf::DefaultVeloRHitManager* m_rHitManager;
  
};
#endif // INCLUDE_BEAMGASTRIGCLUSTERCUT_H

