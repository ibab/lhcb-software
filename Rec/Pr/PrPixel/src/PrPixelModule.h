#ifndef PRPIXELMODULE_H
#define PRPIXELMODULE_H 1

// Local
#include "PrPixelHit.h"

/** @class PrPixelModule PrPixelModule.h
 *  Class to hold hits for one VP module
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */
class PrPixelModule {

 public:
  /// Constructor
  PrPixelModule(const unsigned int number, const bool right)
      : m_lastHitX(-1),
        m_number(number),
        m_previous(-1),
        m_empty(true),
        m_isRight(right) {
    reset();
  }
  /// Destructor
  virtual ~PrPixelModule() {}

  bool empty() const { return m_empty; }
  float lastHitX() const { return m_lastHitX; }
  void setLastHitX(const float x) { m_lastHitX = x; }
  float firstHitX() const { return m_firstHitX; }
  void setFirstHitX(const float x) { m_firstHitX = x; }
  unsigned int number() const { return m_number; }
  int previous() const { return m_previous; }
  bool isRight() const { return m_isRight; }
  float z() const { return m_z; }
  PrPixelHits &hits() { return m_hits; }

  void setPrevious(const int prev) { m_previous = prev; }
  void setZ(const float z) { m_z = z; }

  /// Remove all stored hits.
  void reset() {
    m_hits.clear();
    m_empty = true;
  }
  /// Add a new (pointer to) hit.
  void addHit(PrPixelHit *hit) {
    m_hits.push_back(hit);
    m_empty = false;
  }

 private:
  float m_lastHitX;
  float m_firstHitX;
  /// Module number
  unsigned int m_number;
  // Number of neighbouring same-side module towards smaller z
  int m_previous;
  /// Z-position
  float m_z;
  /// Vector of pointers to hits
  PrPixelHits m_hits;
  bool m_empty;
  /// Right or left side of VELO
  bool m_isRight;
};
#endif  // PRPIXELMODULE_H
