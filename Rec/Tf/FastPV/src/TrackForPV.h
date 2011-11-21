// $Id: $
#ifndef TRACKFORPV_H 
#define TRACKFORPV_H 1

// Include files
#include "Event/Track.h"

/** @class TrackForPV TrackForPV.h
 *  Workable track
 *
 *  @author Olivier Callot
 *  @date   2011-11-15
 */
class TrackForPV {
public: 
  /// Standard constructor
  TrackForPV( LHCb::Track* track, double xBeam, double yBeam ); 

  virtual ~TrackForPV( ); ///< Destructor

  double zAtBeam( ) const { return m_zAtBeam; }
  
  double rAtBeam( ) const { return m_rAtBeam; }

  LHCb::Track* track() { return m_track; }

  double tx() { return m_dir.x(); }
  double ty() { return m_dir.y(); }
  double x0() { return m_point.x() - m_dir.x() * m_point.z(); }
  double y0() { return m_point.y() - m_dir.y() * m_point.z(); }
  double wx() { return m_wx; }
  double wy() { return m_wy; }  

  double chi2( Gaudi::XYZPoint& pt ) {
    double dx = m_point.x() + m_dir.x() * ( pt.z() - m_point.z() ) - pt.x();
    double dy = m_point.y() + m_dir.y() * ( pt.z() - m_point.z() ) - pt.y();
    return m_wx * dx * dx + m_wy * dy * dy;
  }

  struct LowerByZ  {
    bool operator() ( const TrackForPV* lhs, const TrackForPV* rhs) const { return lhs->zAtBeam() < rhs->zAtBeam(); }
  };

  
protected:

private:
  LHCb::Track* m_track;
  Gaudi::XYZPoint  m_point;
  Gaudi::XYZVector m_dir;
  double m_wx;
  double m_wy;
  double m_weight;
  double m_zAtBeam;
  double m_rAtBeam;
};
#endif // TRACKFORPV_H
