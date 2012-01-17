// $Id: $
#ifndef PATPIXELTRACK_H 
#define PATPIXELTRACK_H 1

// Include files
#include "PatPixelHit.h"
#include "PatPixelSensor.h"
#include "Event/StateVector.h"

/** @class PatPixelTrack PatPixelTrack.h
 *  Working tracks, used inside the PatPixelTracking
 *
 *  @author Olivier Callot
 *  @date   2012-01-06
 */
class PatPixelTrack {
public: 
  /// Standard constructor
  PatPixelTrack(  ); 

  virtual ~PatPixelTrack( ){}; ///< Destructor

  void set( PatPixelHit* h1, PatPixelHit* h2 ); 

  void setBackward( bool flag )           { m_backward = flag; }  
  inline bool backward()            const { return m_backward; }

  inline PatPixelHits& hits()             { return m_hits; }
  void addHit( PatPixelHit* hit );
  void removeHit( PatPixelHit* hit );
  void solve();
  void add( PatPixelHits& hits ) {
    for ( PatPixelHits::const_iterator itH = hits.begin();
          hits.end() != itH; ++itH ) {
      addHit( *itH );
    }
  }

  double chi2() {
    double ch = 0.;
    int nDoF  = -4;
    for ( PatPixelHits::const_iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH ) {
      ch   += chi2( *itH );
      nDoF += 2;
    }
    return ch/nDoF;
  }

  void tagUsedHits() {
    for ( PatPixelHits::iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH ) {
      (*itH)->setUsed( true );
    }
  }

  int nbUnused() {
    int nn = 0;
    for ( PatPixelHits::iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH ) {
      if ( !(*itH)->isUsed() ) ++nn;
    }
    return nn;
  }

  bool all3SensorsAreDifferent() {
    if ( m_hits[0]->sensor() == m_hits[1]->sensor() ) return false;
    if ( m_hits[0]->sensor() == m_hits[2]->sensor() ) return false;
    if ( m_hits[1]->sensor() == m_hits[2]->sensor() ) return false;
    return true;
  }

  double distance( PatPixelHit* hit ) { return hit->distance( xAtHit( hit ), yAtHit( hit ) ); } 
  double chi2( PatPixelHit* hit )     { return hit->chi2    ( xAtHit( hit ), yAtHit( hit ) ); }

  double xAtHit( PatPixelHit* hit )   { return m_x0 + m_tx * hit->z(); }
  double yAtHit( PatPixelHit* hit )   { return m_y0 + m_ty * hit->z(); }
  double xAtZ( double z )             { return m_x0 + m_tx * z; }
  double r2AtZ( double z ) {
    double xx = m_x0 + z * m_tx;
    double yy = m_y0 + z * m_ty;
    return xx*xx + yy * yy;
  }
  

  LHCb::StateVector state( double z ) {
    LHCb::StateVector temp;
    temp.setX ( m_x0 + z * m_tx );
    temp.setY ( m_y0 + z * m_ty );
    temp.setZ ( z  );
    temp.setTx( m_tx );
    temp.setTy( m_ty );
    temp.setQOverP( 0. );
    return temp;
  }

  double zBeam() { return -( m_x0 * m_tx + m_y0 * m_ty ) / ( m_tx * m_tx + m_ty * m_ty ); }

  Gaudi::TrackSymMatrix covariance( double z );

protected:

private:
  bool          m_backward;  
  PatPixelHits  m_hits;

  double m_x0;
  double m_tx;
  double m_y0;
  double m_ty;  

  double m_s0;
  double m_sx;
  double m_sz;
  double m_sxz;
  double m_sz2;
  
  double m_u0;
  double m_uy;
  double m_uz;
  double m_uyz;
  double m_uz2;
};

typedef std::vector<PatPixelTrack> PatPixelTracks;

#endif // PATPIXELTRACK_H
