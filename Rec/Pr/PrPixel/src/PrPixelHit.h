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
        m_wxerr(0.),
        m_wyerr(0.),
        m_module(0),
        m_id(0),
        m_isUsed(false) {}
  /// Destructor
  virtual ~PrPixelHit() {}

  void setHit(const LHCb::LHCbID &id, 
              const float x, const float y, const float z,
              const float wxerr, const float wyerr,
              const unsigned int module) {
    m_id = id;
    m_x = x;
    m_y = y;
    m_z = z;
    m_wxerr = wxerr;
    m_wyerr = wyerr;
    m_module = module;
    m_isUsed = false;
  }

  LHCb::LHCbID id() const { return m_id; }
  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }
  float wx() const { return m_wxerr * m_wxerr; }
  float wy() const { return m_wyerr * m_wyerr; }
  float wxerr() const { return m_wxerr; }
  float wyerr() const { return m_wyerr; }
  int module() const { return m_module; }
  bool isUsed() const { return m_isUsed; }
  void setUsed(const bool flag) { m_isUsed = flag; }
  
  /// Pointer to x,y,z,wx
  const float* p_x() const { return &m_x; }

  /// Calculate distance-square / sigma-square
  float chi2(const float x, const float y) const {
    const float dx = m_wxerr * (x - m_x);
    const float dy = m_wyerr * (y - m_y);
    return dx * dx + dy * dy;
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
  /// Global x position
  float m_x;
  /// Global y position
  float m_y;
  /// Global z position
  float m_z;
  /// Weight (1 / error) in X
  float m_wxerr;
  /// Weight (1 / error) in Y 
  float m_wyerr;
  /// Module number
  unsigned int m_module;
  /// Channel ID
  LHCb::LHCbID m_id;
  /// Already used by (associated to) a track?
  bool m_isUsed;
};

typedef std::vector<PrPixelHit *> PrPixelHits;

#endif  // PRPIXELHIT_H
