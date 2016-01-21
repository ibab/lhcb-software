// $Id: $
#ifndef INCLUDE_REFITPARTICLETRACKS_H
#define INCLUDE_REFITPARTICLETRACKS_H 1

#include "Kernel/DaVinciAlgorithm.h"

class IGhostProbability;
class ITrackFitter;

/** @class RefitParticleTracks RefitParticleTracks.h
 *  @brief do some easy tracking tasks to all tracks in a decay chain.
 *
 *  DoFit: does fitting. otherwise perform remaining tasks on unREfitted tracks
 *  DoProbability: reevaluate probabilities (like track likelihood or ghostprobability)
 *  ReplaceTracks: replace candidate tracks by refitted tracks
 *  UpdateProbability: keep original tracks (fit parameters) but overwrite likelihood
 *
 *                               UpdateProbability               ! UpdateProbability
 *                          ReplaceTracks   ! ReplaceTracks   ReplaceTracks  ! ReplaceTracks
 *
 *  DoFit   DoProbability      okay[3]           okay[a]        not okay[2]     okay
 *         !DoProbability     not okay[1]      not okay[1,a]     okay           okay
 *
 *  !DoFit  DoProbability      okay[3]         same as left     not okay[2]     okay
 *         !DoProbability     not okay[1]      not okay[1]     waste of time   use TracksFromParticles
 *
 * @author Paul Seyfert <pseyfert@cern.ch>
 * @date   2012-11-23
 */
class RefitParticleTracks : public DaVinciAlgorithm {

public:

  /// Standard Constructor
  RefitParticleTracks(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~RefitParticleTracks(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode    execute(); ///< Algorithm event execution
//  virtual StatusCode   finalize(); ///< Algorithm finalize

protected:

  virtual StatusCode fit(std::vector<LHCb::Track*> *vec);
  virtual StatusCode manipulate(std::vector<LHCb::Track*> *vec);

private:

  ITrackFitter*        m_trackFitter;
  IGhostProbability*   m_ghostprobability;
  bool                 m_overwrite;
  bool                 m_fit;
  bool                 m_manipulate;
  bool                 m_update;
  bool                 m_writeOutput;
  bool                 m_validationmode;
  std::string          m_fittername;
  std::string          m_manipulatorname;

};
#endif // INCLUDE_REFITPARTICLETRACKS_H

