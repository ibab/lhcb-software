#ifndef TRACKTOOLS_TRAJPOCA_H 
#define TRACKTOOLS_TRAJPOCA_H 1

// Include files
#include <algorithm>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Tr/TrackInterfaces
#include "Kernel/ITrajPoca.h"

// from Kernel/LHCbKernel
#include "Kernel/Trajectory.h"

/** @class TrajPoca TrajPoca.h
 *  
 *  Calculates points of closest approach
 *  between two trajectories
 *
 *  @author Steve Schaffner (babar implementation)
 *  @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 *  @date   2005-11-23
 */

class TrajPoca : public extends<GaudiTool,ITrajPoca> {

public:

  /// Default constructor
  TrajPoca( const std::string& type, 
            const std::string& name,
            const IInterface* parent );

  /// Default destructor
  ~TrajPoca() override = default;

  /// Find points along trajectories at which the distance between the
  /// trajectories is at its minimum. The precision parameter is the desired
  /// numerical accuracy of mu1 and mu2. If the restrictrange flag is true, mu
  /// is restricted to the range of the trajectory.
  StatusCode minimize( const LHCb::Trajectory& traj1,
                       double& mu1, 
                       bool restrictRange1,
                       const LHCb::Trajectory& traj2,
                       double& mu2, 
                       bool restrictRange2, 
                       Gaudi::XYZVector& distance,
                       double precision ) const override;

  /// Find point along trajectory at which the distance to point 'p'
  /// is minimum. The precision parameter is the desired numerical accuracy of
  /// the expansion parameter mu. If the restrictrange flag is true, mu is
  /// restricted to the range of the trajectory.
  StatusCode minimize( const LHCb::Trajectory& traj,
                       double& mu,
                       bool restrictRange,
                       const Gaudi::XYZPoint& pt,
                       Gaudi::XYZVector& distance,
                       double precision ) const override;
private:
  bool stepTowardPoca( const LHCb::Trajectory& traj1,
                       double& mu1,
                       bool restrictRange1,
                       const LHCb::Trajectory& traj2,
                       double& mu2, 
                       bool restrictRange2,
                       double precision ) const;

  inline bool restrictToRange(double& l, const LHCb::Trajectory& t) const
  {
    const auto lmax = std::max(t.beginRange(),t.endRange());
    const auto lmin = std::min(t.beginRange(),t.endRange());
    const auto oldl = l;
    l = std::max(lmin, std::min(lmax, l));
    return oldl != l;
  }

private:

  // jobOptions
  int m_maxnOscillStep;
  int m_maxnDivergingStep;
  int m_maxnStuck;
  int m_maxnTry;
  double m_maxDist;
  double m_maxExtrapTolerance;

  // replace the statics in stepTowardPoca
  mutable Gaudi::XYZPoint m_p1, m_p2;
  mutable Gaudi::XYZVector m_dp1dmu1, m_dp2dmu2;
  mutable Gaudi::XYZVector m_d2p1dmu12, m_d2p2dmu22;
};

#endif // TRACKTOOLS_TRAJPOCA_H
