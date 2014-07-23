#ifndef PRPIXELHIT_H
#define PRPIXELHIT_H 1

#include "GaudiKernel/Point3DTypes.h"

#include "Kernel/LHCbID.h"


/** @class PrPixelHit PrPixelHit.h
 *  This defines the VP hits to be used in the pattern recognition.
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */

class PrPixelHit {
 public:
  /// Standard constructor
  PrPixelHit()
      : m_x(0.),
        m_y(0.),
        m_z(0.),
        m_wx(0.),
        m_wy(0.),
        m_module(0),
        m_id(0),
        m_isUsed(false) {}
  /// Destructor
  virtual ~PrPixelHit() {}

  void setHit(const LHCb::LHCbID id, const Gaudi::XYZPoint &point,
              const float wx, const float wy, const unsigned int module) {
    m_id = id;
    m_x = point.x();
    m_y = point.y();
    m_z = point.z();
    m_wx = wx;
    m_wy = wy;
    m_module = module;
    m_isUsed = false;
  }

  void setHit(const LHCb::LHCbID id, const float x, const float y,
              const float z, const float wx, const float wy,
              const unsigned int module) {
    m_id = id;
    m_x = x;
    m_y = y;
    m_z = z;
    m_wx = wx;
    m_wy = wy;
    m_module = module;
    m_isUsed = false;
  }

  LHCb::LHCbID id() const { return m_id; }
  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }
  float wx() const { return m_wx; }
  float wy() const { return m_wy; }
  int module() const { return m_module; }
  bool isUsed() const { return m_isUsed; }
  void setUsed(const bool flag) { m_isUsed = flag; }
  
  /// Pointer to x,y,z,wx
  const float* p_x() const { return &m_x; }

  /// Calculate distance in X-Y to given X-Y point
  float distance(const float x, const float y) const {
    const float dx = x - m_x;
    const float dy = y - m_y;
    return sqrt(dx * dx + dy * dy);
  }

  /// Calculate distance-square / sigma-square
  float chi2(const float x, const float y) const {
    const float dx = x - m_x;
    const float dy = y - m_y;
    return dx * dx * m_wx + dy * dy * m_wy;
  }

  // Operators for sorting the vectors of pointers to hits
  struct DecreasingByZ {
    bool operator()(const PrPixelHit *lhs, const PrPixelHit *rhs) const {
      return lhs->z() > rhs->z();
    }
  };
  struct IncreasingByZ {
    bool operator()(const PrPixelHit *lhs, const PrPixelHit *rhs) const {
      return lhs->z() < rhs->z();
    }
  };
  struct LowerByX {
    bool operator()(const PrPixelHit *lhs, const PrPixelHit *rhs) const {
      return lhs->x() < rhs->x();
    }
  };
  struct LowerByY {
    bool operator()(const PrPixelHit *lhs, const PrPixelHit *rhs) const {
      return lhs->y() < rhs->y();
    }
  };

 private:
  /// Global position
  float m_x;
  float m_y;
  float m_z;
  /// Weight (1 / squared error) in X
  float m_wx;
  /// Weight (1 / squared error) in Y 
  float m_wy;
  /// Module number
  unsigned int m_module;
  /// Channel ID
  LHCb::LHCbID m_id;
  /// Already used by (associated to) a track?
  bool m_isUsed;
};

typedef std::vector<PrPixelHit *> PrPixelHits;

#endif  // PRPIXELHIT_H
