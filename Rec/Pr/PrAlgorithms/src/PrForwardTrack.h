#ifndef PRFORWARDTRACK_H 
#define PRFORWARDTRACK_H 1

// Include files
#include "Event/Track.h"
#include "PrKernel/PrHit.h"

/** @class PrForwardTrack PrForwardTrack.h
 *  This is the working class inside the T station pattern
 *
 *  @author Olivier Callot
 *  @date   2012-03-22
 */
class PrForwardTrack {
public: 
  /// Constructor with only a Velo track
  PrForwardTrack( const LHCb::Track* track, float zRef ) 
    : m_track( track ), m_zRef( zRef )
  {
    init();
  }; 

  /// Constructor with Velo track and list of hits
  PrForwardTrack( const LHCb::Track* track, float zRef, PrHits& hits )
    : m_track( track ), m_zRef( zRef ), m_hits( hits ) 
  {
    init();
    if ( m_hits.size() > 0 ) m_zone = m_hits.front()->zone();
  }; 

  virtual ~PrForwardTrack( ) {}; ///< Destructor

  const LHCb::Track* track()  const { return m_track; }
  float xFromVelo( float z )  const { return m_x0 + (z-m_z0) * m_tx; }
  float yFromVelo( float z )  const { return m_y0 + (z-m_z0) * m_ty; }
  float qOverP()              const { return m_qOverP; }
  
  float slX()                 const { return m_tx;  }
  float slY()                 const { return m_ty;  }
  float slX2()                const { return m_tx2; }
  float slY2()                const { return m_ty2; }
  float slope2()              const { return m_slope2; }
  
  /// Handling of hits: acceess, insertion
  PrHits& hits()                   { return m_hits; }
  const PrHits& hits()       const { return m_hits; }
  void addHit( PrHit* hit )        { m_hits.push_back( hit ); m_zone = m_hits.front()->zone(); }
  void addHits( PrHits& hits ) {
    m_hits.reserve( m_hits.size() + hits.size() );
    m_hits.insert( m_hits.end(), hits.begin(), hits.end() );
    m_zone = m_hits.front()->zone();
  }
  unsigned int zone() const { return m_zone; }
     
  /// Parameters of the trajectory in the T stations
  void setParameters( float ax, float bx, float cx, float dx, float ay, float by ) {
    m_ax = ax;
    m_bx = bx;
    m_cx = cx;
    m_dx = dx;
    m_ay = ay;
    m_by = by;
  }
  
  void updateParameters( float dax, float dbx, float dcx,
                         float ddx=0., float day=0., float dby= 0.  ) {
    m_ax += dax;
    m_bx += dbx;
    m_cx += dcx;
    m_dx += ddx;
    m_ay += day;
    m_by += dby;
  }

  float x( float z )         const { float dz = z-m_zRef; return m_ax + dz*( m_bx + dz*( m_cx + dz*m_dx ) ); }
  float xSlope( float z )    const { float dz = z-m_zRef; return m_bx + dz*( 2 * m_cx + 3 * dz * m_dx ); }
  float y( float z )         const { return m_ay + (z-m_zRef) *  m_by; }
  float ySlope( )            const { return m_by; }  
  float xStraight( float z ) const { return m_ax + (z-m_zRef) * m_bx; }

  float yOnTrack( PrHit* hit ) const { return hit->yOnTrack( m_ay -m_zRef * m_by, m_by ); }

  float distance( PrHit* hit ) const { 
    float yTra = yOnTrack( hit );
    return hit->x( yTra ) - x( hit->z(yTra) );
  }

  float chi2( PrHit* hit )     const { float d = distance( hit ); return d * d * hit->w(); }

  float deltaY( PrHit* hit )   const {
    if ( 0 == hit->dxDy() ) return 0.;
    return distance( hit ) / hit->dxDy();
  }

  bool valid()                  const { return m_valid; }
  void setValid( bool v )             { m_valid = v; }

  void setChi2( float chi2, int nDoF ) { m_chi2 = chi2; m_nDoF = nDoF; }
  float chi2()                  const { return m_chi2; }
  float chi2PerDoF()            const { return m_chi2 / m_nDoF; }
  int   nDoF()                  const { return m_nDoF; }

  void setDXCoord( float dxCoord )    { m_dXCoord = dxCoord; }
  float dXCoord()               const { return m_dXCoord; }

  void setMeanDy( float meanDy )      { m_meanDy = meanDy; }
  float meanDy()                const { return m_meanDy; }
protected:

private:
  void init() {
    m_valid = true;
    m_hits.reserve( 32 );
    const LHCb::State& state = m_track->closestState( 0. );
    m_x0     = state.x();
    m_y0     = state.y();
    m_z0     = state.z();
    m_tx     = state.tx();
    m_ty     = state.ty();
    m_qOverP = state.qOverP();
    m_tx2    = m_tx * m_tx;
    m_ty2    = m_ty * m_ty;
    m_slope2 = m_tx2 + m_ty2;
    
    m_ax = 0.;
    m_bx = 0.;
    m_cx = 0.;
    m_dx = 0.;
    m_by = m_ty;
    m_ay = m_y0 + (m_zRef-m_z0) * m_by;

    m_chi2 = 0.;
    m_nDoF = -1;
    m_zone = 0;
    m_dXCoord = 0.;
    m_meanDy  = 0.;
  }
  
  const LHCb::Track* m_track;
  float  m_zRef;
  PrHits m_hits;
  bool   m_valid;
  unsigned int m_zone;
  /// Parameters of the track 
  float m_x0;
  float m_y0;
  float m_z0;
  float m_tx;
  float m_ty;
  float m_qOverP;
  float m_tx2;
  float m_ty2;
  float m_slope2;
  
  float m_ax;
  float m_bx;
  float m_cx;
  float m_dx;
  float m_ay;
  float m_by;

  float m_chi2;
  int   m_nDoF;
  float m_dXCoord;
  float m_meanDy;
};

typedef std::vector<PrForwardTrack> PrForwardTracks;

#endif // PRFORWARDTRACK_H
