#ifndef PRFORWARDTRACK_H 
#define PRFORWARDTRACK_H 1

// Include files
#include "Event/Track.h"
#include "Event/StateParameters.h"
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
  PrForwardTrack( const LHCb::Track* track, const float zRef ) 
    : m_track( track ), m_zRef( zRef )
  {
    init();
  }; 

  /// Constructor with Velo track and list of hits
  PrForwardTrack( const LHCb::Track* track, const float zRef, PrHits& hits )
    : m_track( track ), m_zRef( zRef ), m_hits( hits ) 
  {
    init();
    if ( m_hits.size() > 0 ) m_zone = m_hits.front()->zone();
  }; 

  virtual ~PrForwardTrack( ) {}; ///< Destructor

  const LHCb::Track* track()        const { return m_track; }
  /// Predicted x position for a given z, using a straight line from Velo information
  float xFromVelo( const float z )  const { return m_x0 + (z-m_z0) * m_tx; }
  /// Predicted y position for a given z, using a straight line from Velo information
  float yFromVelo( const float z )  const { return m_y0 + (z-m_z0) * m_ty; }
  float qOverP()                    const { return m_qOverP; }
  
  float slX()                       const { return m_tx;  }
  float slY()                       const { return m_ty;  }
  float slX2()                      const { return m_tx2; }
  float slY2()                      const { return m_ty2; }
  float slope2()                    const { return m_slope2; }
  
  /// Handling of hits: access, insertion
  PrHits& hits()                   { return m_hits; }
  const PrHits& hits()       const { return m_hits; }
  void addHit( PrHit* hit )        { m_hits.push_back( hit ); m_zone = m_hits.front()->zone(); }
  void addHits( PrHits& hits ) {
    m_hits.reserve( m_hits.size() + hits.size() );
    m_hits.insert( m_hits.end(), hits.begin(), hits.end() );
    m_zone = m_hits.front()->zone();
  }
  unsigned int zone() const { return m_zone; }
     
  /** Set the parameters of the track. Cubic in x, parabolic in y
   *  @brief  Set the parameters of the track
   *  @param ax constant coefficient for x parametrisation
   *  @param bx linear coefficient for x parametrisation
   *  @param cx quadratic coefficient for x parametrisation
   *  @param dx cubic coefficient for x parametrisation
   *  @param ay constant coefficient for y parametrisation
   *  @param by linear coefficient for y parametrisation
   *  @param cy quadratic coefficient for y parametrisation
   */
  void setParameters( const float ax, const float bx, const float cx, const float dx, 
                      const float ay, const float by, const float cy ) {
    m_ax = ax;
    m_bx = bx;
    m_cx = cx;
    m_dx = dx;
    m_ay = ay;
    m_by = by;
    m_cy = cy;
  }
  
   /** Update the parameters of the track. Cubic in x, parabolic in y
   *  @brief Update the parameters of the track
   *  @param ax constant coefficient for x parametrisation
   *  @param bx linear coefficient for x parametrisation
   *  @param cx quadratic coefficient for x parametrisation
   *  @param dx cubic coefficient for x parametrisation
   *  @param ay constant coefficient for y parametrisation
   *  @param by linear coefficient for y parametrisation
   *  @param cy quadratic coefficient for y parametrisation
   */
  void updateParameters( const float dax, const float dbx, const float dcx,
                         const float ddx=0., const float day=0., const float dby= 0., const float dcy = 0. ) {
    m_ax += dax;
    m_bx += dbx;
    m_cx += dcx;
    m_dx += ddx;
    m_ay += day;
    m_by += dby;
    m_cy += dcy;
  }

  /// Get the x position at a certain z position
  float x( const float z )         const { float dz = z-m_zRef; return m_ax + dz*( m_bx + dz*( m_cx + dz*m_dx ) ); }
  /// Get the x slope at a certain z position
  float xSlope( const float z )    const { float dz = z-m_zRef; return m_bx + dz*( 2 * m_cx + 3 * dz * m_dx ); }
  /// Get the y position at a certain z position
  float y( const float z )         const { float dz = z-m_zRef; return m_ay + dz*( m_by + dz * m_cy); } 
  /// Get the y slope at a certain z position
  float ySlope( const float z )    const { float dz = z-m_zRef; return m_by + dz* 2. * m_cy; }  
  /// Get the x position at a certain z position, assuming the track is a straight line
  float xStraight( const float z ) const { return m_ax + (z-m_zRef) * m_bx; }
  /// Get the y position at a certain z position, assuming the track is a straight line
  float yStraight( const float z ) const { return m_ay + (z-m_zRef) * m_by; }

  /// Calculate the y position of a hit given the parametrised track
  float yOnTrack( PrHit* hit ) const { const float sly =  ySlope( hit->z() ); 
    return hit->yOnTrack( y(hit->z()) - sly * hit->z(), sly ); }

  /// Calculate the distance between a hit and the parametrised track
  float distance( PrHit* hit ) const { 
    float yTra = yOnTrack( hit );
    return hit->x( yTra ) - x( hit->z(yTra) );
  }

  /// Calculate the chi2 contribution of a single hit to the parametrised track
  float chi2( PrHit* hit )     const { const float d = distance( hit ); return d * d * hit->w(); }

  /// Calculate the y distance between a hit and the parametrised track
  float deltaY( PrHit* hit )   const {
    if ( hit->isX() ) return 0.;
    return distance( hit ) / hit->dxDy();
  }

  bool valid()                               const { return m_valid; }
  void setValid( const bool v )                    { m_valid = v; }

  void setChi2( const float chi2, const int nDoF ) { m_chi2 = chi2; m_nDoF = nDoF; }
  float chi2()                               const { return m_chi2; }
  float chi2PerDoF()                         const { return m_chi2 / m_nDoF; }
  int   nDoF()                               const { return m_nDoF; }
  
  
  void setDXCoord( const float dxCoord )           { m_dXCoord = dxCoord; }
  float dXCoord()                            const { return m_dXCoord; }

  void setMeanDy( const float meanDy )             { m_meanDy = meanDy; }
  float meanDy()                             const { return m_meanDy; }

  void setHitsUnused() {
    for ( PrHit* hit : m_hits ) hit->setUsed(false);
  }

  /// Return number of stereo hits
  int nStereoHits() {
    int n = 0;
    for ( PrHit* hit : m_hits ){
      if ( !hit->isX() ) ++n;
    }
    return n;
  }

  void  setQuality( const float q )               { m_quality = q; }
  float quality()                           const { return m_quality; }

  struct LowerByQuality {
    bool operator() (const PrForwardTrack& lhs, const PrForwardTrack& rhs ) const { return lhs.quality() < rhs.quality(); }
  };
  
  void  setChi2AtMagnet( const float chi2 )       { m_chi2AtMagnet = chi2; }
  float chi2AtMagnet()                      const { return m_chi2AtMagnet; }

protected:

private:

  void init() {
    m_valid = true;
    m_hits.reserve( 32 );
    const LHCb::State& state = m_track->closestState( StateParameters::ZEndVelo );
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
    m_cy = 0.;
    m_ay = m_y0 + (m_zRef-m_z0) * m_by;

    m_chi2 = 0.;
    m_nDoF = -1;
    m_zone = 0;
    m_dXCoord = 0.;
    m_meanDy  = 0.;
    m_quality = 0.;
    m_chi2AtMagnet = 0.;
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
  float m_cy;

  float m_chi2;
  int   m_nDoF;
  float m_dXCoord;
  float m_meanDy;
  float m_quality;
  float m_chi2AtMagnet;
};

typedef std::vector<PrForwardTrack> PrForwardTracks;

#endif // PRFORWARDTRACK_H
