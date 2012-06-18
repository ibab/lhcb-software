// $Id: $
#ifndef PRKERNEL_PRHITLAYER_H 
#define PRKERNEL_PRHITLAYER_H 1

// Include files
#include "PrKernel/PrHit.h"
#include "Kernel/DetectorSegment.h"

/** @class PrHitLayer PrHitLayer.h PrKernel/PrHitLayer.h
 *  Store the information of a layer in the T stations
 *
 *  @author Olivier Callot
 *  @date   2012-03-13
 */
class PrHitLayer {
public: 
  /// Standard constructor
  PrHitLayer( int number) {
    m_number = number;
    m_z      =  1.e9;
    m_xMin   = -1.e9;
    m_xMax   =  1.e9;
    m_yMin   = -1.e9;
    m_yMax   =  1.e9;
  }

  virtual ~PrHitLayer( ) {}; ///< Destructor

  void setGeometry( DetectorSegment& seg ) { 
    m_z      = seg.z( 0. );
    m_dxDy   = ( seg.x(100.) - seg.x(0.) ) / 100.;
    m_dzDy   = ( seg.z(100.) - seg.z(0.) ) / 100.;
    m_isX    = fabs( m_dxDy ) < 0.010;
  }; 

  PrHits&  hits( )                { return m_hits;   }

  unsigned int number()     const { return m_number; }
  float z( float y = 0.)          { return m_z + m_dzDy * y; }
  float dxDy()              const { return m_dxDy; }
  float dzDy()              const { return m_dzDy; }
  bool isX()                const { return m_isX; }

  void reset()                    { m_hits.clear(); }

  void addHit( PrHit* hit )       { m_hits.push_back( hit ); }

  /// Set the size of the surrounding box
  void setBoundaries( float xMin, float xMax, float yMin, float yMax ) {
    m_xMin = xMin;
    m_xMax = xMax;
    m_yMin = yMin;
    m_yMax = yMax;
  }

  /// Is the point in the surrounding box?
  bool isInside( float x, float y ) {
    if ( x < m_xMin ) return false;
    if ( x > m_xMax ) return false;
    if ( y < m_yMin ) return false;
    if ( y > m_yMax ) return false;
    return true;
  }

protected:

private:
  unsigned int  m_number;
  float         m_z;
  float         m_dxDy;
  float         m_dzDy;
  bool          m_isX;
  float         m_xMin;
  float         m_xMax;
  float         m_yMin;
  float         m_yMax;
  PrHits        m_hits;
};
typedef std::vector<PrHitLayer*> PrHitLayers;
#endif // PRKERNEL_PRHITLAYER_H
