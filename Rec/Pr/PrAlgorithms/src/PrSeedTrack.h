#ifndef PRSEEDTRACK_H 
#define PRSEEDTRACK_H 1

// Include files
#include "Event/Track.h"
#include "PrKernel/PrHit.h"

/** @class PrSeedTrack PrSeedTrack.h
 *  This is the working class inside the T station pattern
 *
 *  @author Olivier Callot
 *  @date   2012-03-22
 */
class PrSeedTrack {
public: 
  /// Constructor with the z reference
  PrSeedTrack( unsigned int zone, float zRef ) 
    : m_zone( zone ), m_zRef( zRef )
  {
    init();
  }; 

  /// Constructor with list of hits
  PrSeedTrack( unsigned int zone, float zRef, PrHits& hits )
    : m_zone( zone ), m_zRef( zRef ), m_hits( hits ) 
  {
    init();
  }; 

  virtual ~PrSeedTrack( ) {}; ///< Destructor

  /// Handling of hits: acceess, insertion
  PrHits& hits()                   { return m_hits; }
  const PrHits& hits()       const { return m_hits; }
  void addHit( PrHit* hit )        { m_hits.push_back( hit );}
  void addHits( PrHits& hits ) {
    m_hits.reserve( m_hits.size() + hits.size() );
    m_hits.insert( m_hits.end(), hits.begin(), hits.end() );
  }
  unsigned int zone() const { return m_zone; }
     
  /// Parameters of the trajectory in the T stations
  void setParameters( float ax, float bx, float cx, float ay, float by ) {
    m_ax = ax;
    m_bx = bx;
    m_cx = cx;
    m_ay = ay;
    m_by = by;
  }
  
  void updateParameters( float dax, float dbx, float dcx,
                         float day=0., float dby= 0.  ) {
    m_ax += dax;
    m_bx += dbx;
    m_cx += dcx;
    m_ay += day;
    m_by += dby;
  }

  float x( float z )         const { float dz = z-m_zRef; return m_ax + dz * ( m_bx + dz* m_cx ); }
  float xSlope( float z )    const { float dz = z-m_zRef; return m_bx + 2. * dz * m_cx; }
  float y( float z )         const { return m_ay + (z-m_zRef) *  m_by; }
  float ySlope( )            const { return m_by; }  

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

    m_ax = 0.;
    m_bx = 0.;
    m_cx = 0.;
    m_by = 0.;
    m_ay = 0.;

    m_chi2 = 0.;
    m_nDoF = -1;
    m_dXCoord = 0.;
    m_meanDy  = 0.;
  }
  
  unsigned int m_zone;
  float  m_zRef;
  PrHits m_hits;
  bool   m_valid;
  
  float m_ax;
  float m_bx;
  float m_cx;
  float m_ay;
  float m_by;

  float m_chi2;
  int   m_nDoF;
  float m_dXCoord;
  float m_meanDy;
};

typedef std::vector<PrSeedTrack> PrSeedTracks;

#endif // PRSEEDTRACK_H
