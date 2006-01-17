// $Id: TrajPoca.h,v 1.2 2006-01-17 09:15:10 ebos Exp $
#ifndef TRACKTOOLS_TRAJPOCA_H 
#define TRACKTOOLS_TRAJPOCA_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Kernel/LHCbKernel
#include "Kernel/Trajectory.h"

// from Tr/TrackInterfaces
#include "TrackInterfaces/ITrajPoca.h"

/** @class TrajPoca TrajPoca.h
 *  
 *  Calculates points of closest approach
 *  between two trajectories
 *
 *  @author Edwin Bos, Jeroen van Tilburg, Eduardo Rodrigues
 *  @date   2005-11-23
 */

class TrajPoca : public GaudiTool,
                 virtual public ITrajPoca {

public:

  /// Default constructor
  TrajPoca( const std::string& type, 
            const std::string& name,
            const IInterface* parent );

  /// Default destructor
  virtual ~TrajPoca();

  /// Tool initialization
  StatusCode initialize();

  /// Find arclengths along trajectories
  /// having a distance smaller than tolerance
  virtual StatusCode minimize( const Trajectory& traj1,
                               double& arclength1, 
                               bool restrictRange1,
                               const Trajectory& traj2,
                               double& arclength2, 
                               bool restrictRange2, 
                               HepVector3D& distance,
                               double precision );

  /// Find the minimum distance between a point and a Trajectory
  virtual StatusCode minimize( const Trajectory& traj,
                               double& arclength,
                               bool restrictRange,
                               const HepPoint3D& pt,
                               HepVector3D& distance,
                               double precision );
private:

  StatusCode stepTowardPoca( const Trajectory& traj1,
                             double& arclength1,
                             bool restrictRange1,
                             const Trajectory& traj2,
                             double& arclength2, 
                             bool restrictRange2,
                             double precision ) const;

  double restrictLen( double l, const Trajectory& t, bool restrictRange ) const
  {
    return restrictRange ? t.restrictToRange(l) : l ; 
  }
  
  HepVector3D m_distance;

  // jobOptions
  int m_maxnOscillStep;
  int m_maxnDivergingStep;
  int m_maxnStuck;
  int m_maxnTry;
  double m_maxDist;
 
  double m_tolerance;

};

#endif // TRACKTOOLS_TRAJPOCA_H


