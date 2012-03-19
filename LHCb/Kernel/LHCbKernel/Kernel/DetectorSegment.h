// $Id: $
#ifndef KERNEL_DETECTORSEGMENT_H 
#define KERNEL_DETECTORSEGMENT_H 1

// Include files

/** @class DetectorSegment DetectorSegment.h Kernel/DetectorSegment.h
 *  Simple class to manipulate a segment manly along Y, as the measurements are in T/TT
 *
 *  @author Olivier Callot
 *  @date   2012-03-16
 */
class DetectorSegment {
public: 
  /// Standard constructor
  DetectorSegment( ) {
    m_x0   = 0.;
    m_z0   = 0.;
    m_dxDy = 0.;
    m_dzDy = 0.;
    m_yMin = 0.;
    m_yMax = 0.;
  }
  /// Contstructor with the necessary information
  DetectorSegment( float x0, float z0, float dxDy, float dzDy, float yMin, float yMax ) {
    m_x0   = x0;
    m_z0   = z0;
    m_dxDy = dxDy;
    m_dzDy = dzDy;
    m_yMin = yMin;
    m_yMax = yMax;
    if ( m_yMin > m_yMax ) {
      m_yMin = yMax;
      m_yMax = yMin;
    }
  }  

  virtual ~DetectorSegment( ) {}; ///< Destructor

  float x( float y ) const { return m_x0 + y * m_dxDy; }
  float z( float y ) const { return m_z0 + y * m_dzDy; }

  float xMin() const { return m_dxDy > 0 ? x(m_yMin) : x(m_yMax); }
  float xMax() const { return m_dxDy > 0 ? x(m_yMax) : x(m_yMin); }

  float yMin() const { return m_yMin; }
  float yMax() const { return m_yMax; }

  /// returns the y along the trajectory y = y0 + z * dzDy
  float yOnTrack( float y0, float dyDz ) {
    return ( y0 + dyDz * m_z0 ) / ( 1. - dyDz * m_dzDy );
  }
  

protected:

private:
  float m_x0;    ///< x coordinate at y = 0
  float m_z0;    ///< z coordinate at y = 0
  float m_dxDy;  ///< Slope x vs y, typically 0 for x layers
  float m_dzDy;  ///< Slope z vs y, as detectors are vertical while Z axis is not horizontal
  float m_yMin;  ///< minimum y coordinate along this segment
  float m_yMax;  ///< maximum y coordinate along this segment
};
#endif // KERNEL_DETECTORSEGMENT_H
