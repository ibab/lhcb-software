#ifndef PATPIXELSENSOR_H
#define PATPIXELSENSOR_H 1

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// Local
#include "PatPixelHit.h"

/** @class PatPixelSensor PatPixelSensor.h
 *  Describes one module and its data for the hit manager
 *
 *  @author Olivier Callot
 *  @date   2012-01-05
 */
class PatPixelSensor {

public:
  /// Constructor
  PatPixelSensor(const unsigned int number, const bool right) : 
    m_number(number), m_isRight(right), m_previous(-1) {
    reset();
  }
  /// Destructor
  virtual ~PatPixelSensor() {}

  unsigned int number() const {return m_number;}
  int previous() const {return m_previous;}
  bool isRight() const {return m_isRight;}
  double z() const {return m_z;}
  PatPixelHits& hits() {return m_hits;}

  void setPrevious(const int prev) {m_previous = prev;}
  void setZ(const double z) {m_z = z;}

  /// Remove all stored hits.
  void reset() {m_hits.clear();}
  /// Add a new (pointer to) hit.
  void addHit(PatPixelHit* hit) {m_hits.push_back(hit);} 

private:
  /// Module number
  unsigned int m_number;
  /// Right or left side of VELO
  bool m_isRight;
  // Number of neighbouring same-side module towards smaller z
  int m_previous;
  /// Z-position
  double m_z;
  /// Vector of pointers to hits
  PatPixelHits m_hits;

};
#endif // PATPIXELSENSOR_H
