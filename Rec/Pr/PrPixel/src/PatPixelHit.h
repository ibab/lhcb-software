#ifndef PATPIXELHIT_H
#define PATPIXELHIT_H 1

// Include files
#include "Kernel/LHCbID.h"
#include "GaudiKernel/Point3DTypes.h"

/** @class PatPixelHit PatPixelHit.h
 *  This defines the VP hits to be used in the pattern recognition.
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */

class PatPixelHit {
public:
  /// Standard constructor
  PatPixelHit() :
    m_x(0.), m_y(0.), m_z(0.),
    m_id(0),
    m_wx(0.), m_wy(0.),
    m_module(0),
    m_isUsed(false) {

  }
  /// Destructor
  virtual ~PatPixelHit() {}

  void setHit(const LHCb::LHCbID id,
              const Gaudi::XYZPoint& point,
              const double dx,
              const double dy,
              const unsigned int module) {
    m_id = id;
    m_x = point.x();
    m_y = point.y();
    m_z = point.z();
    m_wx = 1. / (dx * dx);
    m_wy = 1. / (dy * dy);
    m_module = module;
    m_isUsed = false;
  }

  LHCb::LHCbID id() const {return m_id;}
  double x() const {return m_x;}
  double y() const {return m_y;}
  double z() const {return m_z;}
  double wx() const {return m_wx;}
  double wy() const {return m_wy;}
  int module() const {return m_module;}
  bool isUsed() const {return m_isUsed;}
  void setUsed(const bool flag) {m_isUsed = flag;}

  /// Calculate distance in X-Y to given X-Y point
  double distance(const double x, const double y) const {
    const double dx = x - m_x;
    const double dy = y - m_y;
    return sqrt(dx * dx + dy * dy);
  }

  /// Calculate distance-square / sigma-square
  double chi2(const double x, const double y) const {
    const double dx = x - m_x;
    const double dy = y - m_y;
    return dx * dx * m_wx + dy * dy * m_wy;
  }

  // Operators for sorting the vectors of pointers to hits
  struct DecreasingByZ { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->z() > rhs->z(); } };
  struct IncreasingByZ { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->z() < rhs->z(); } };
  struct LowerByX      { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->x() < rhs->x(); } };
  struct LowerByY      { bool operator() (const PatPixelHit* lhs, const PatPixelHit* rhs) const { return lhs->y() < rhs->y(); } };

private:
  /// Global position
  double m_x;
  double m_y;
  double m_z;
  /// Channel ID
  LHCb::LHCbID m_id;
  /// Weights (1 / squared error) in X and Y
  double m_wx;
  double m_wy;
  // Module number
  unsigned int m_module;
  // Already used by (associated to) a track?
  bool m_isUsed;

};

typedef std::vector<PatPixelHit*> PatPixelHits;  // vector of hits

#endif // PATPIXELHIT_H
