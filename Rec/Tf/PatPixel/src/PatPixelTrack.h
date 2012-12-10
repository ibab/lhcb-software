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

  void set( PatPixelHit* h1, PatPixelHit* h2 );                        // set this track to contain only the two given hits

  void setBackward( bool flag )           { m_backward = flag; }       // set(clear) "a-backward-track" flag
  inline bool backward()            const { return m_backward; }

  inline PatPixelHits& hits()             { return m_hits; }
  void addHit( PatPixelHit* hit );                                     // add a new hit to this track
  void removeHit( PatPixelHit* hit );                                  // remove given hit
  void solve();                                                        // (re)calculate the fit from the x/y-slope sums
  void add( PatPixelHits& hits )                                     // add a vector of hits to this track
  { for ( PatPixelHits::const_iterator itH = hits.begin(); hits.end() != itH; ++itH )
    {  addHit( *itH ); }
  }

  double chi2()                                                        // Chi2/degrees-of-freedom for this track
  { double ch = 0.;
    int nDoF  = -4;
    for ( PatPixelHits::const_iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH )
    { ch   += chi2( *itH ); nDoF += 2; }
    return ch/nDoF; }

  void tagUsedHits()                                                   // mark hits of this track at being associated
  { for ( PatPixelHits::iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH )
    {  (*itH)->setUsed( true ); }
  }

  int nbUnused()                                                       // count unassociated hits for this track
  { int nn = 0;
    for ( PatPixelHits::iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH )
    { if ( !(*itH)->isUsed() ) ++nn; }
    return nn; }

  bool all3SensorsAreDifferent() // check that for a 3-hit track, there are actually 3 different sensors used.
  { if ( m_hits[0]->sensor() == m_hits[1]->sensor() ) return false;
    if ( m_hits[0]->sensor() == m_hits[2]->sensor() ) return false;
    if ( m_hits[1]->sensor() == m_hits[2]->sensor() ) return false;
    return true; }

  double distance( PatPixelHit* hit ) { return hit->distance( xAtHit( hit ), yAtHit( hit ) ); } 
  double chi2( PatPixelHit* hit )     { return hit->chi2    ( xAtHit( hit ), yAtHit( hit ) ); }

  double xAtHit( PatPixelHit* hit )   { return m_x0 + m_tx * hit->z(); }  // predict x at a hit (with given z)
  double yAtHit( PatPixelHit* hit )   { return m_y0 + m_ty * hit->z(); }  // predict y at a hit (with given z)
  double xAtZ( double z )             { return m_x0 + m_tx * z; }         // predict x at given z
  double r2AtZ( double z )                                                // predict R-square at given z
  { double xx = m_x0 + z * m_tx;
    double yy = m_y0 + z * m_ty;
    return xx*xx + yy * yy; }

  LHCb::StateVector state( double z ) {      // create a track state vector: position and the slopes: dx/dz and dy/dz
    LHCb::StateVector temp;
    temp.setX ( m_x0 + z * m_tx );
    temp.setY ( m_y0 + z * m_ty );
    temp.setZ ( z  );
    temp.setTx( m_tx );
    temp.setTy( m_ty );
    temp.setQOverP( 0. );                    // Q/P is Charge/Momentum ratio
    return temp;
  }

  double zBeam() const // z-pos. where the track passes closest to the beam
  { return -( m_x0 * m_tx + m_y0 * m_ty ) / ( m_tx * m_tx + m_ty * m_ty ); }

  Gaudi::TrackSymMatrix covariance( double z );

  double meanX() const { return m_sx/m_s0; }
  double meanY() const { return m_uy/m_u0; }
  double meanZ() const { return m_sz/m_s0; }

protected:

private:
  bool          m_backward;  
  PatPixelHits  m_hits;         // vector<PatPixelHit>

                                // track fit as a straight line
  double m_x0;                  // x at z=0
  double m_tx;                  // dx/dz
  double m_y0;                  // y at z=0
  double m_ty;                  // dy/dz

  double m_s0;                  // sums for the x-slope fit
  double m_sx;
  double m_sz;
  double m_sxz;
  double m_sz2;
  
  double m_u0;                  // sums for the y-slope fit
  double m_uy;
  double m_uz;
  double m_uyz;
  double m_uz2;
};

typedef std::vector<PatPixelTrack> PatPixelTracks;

#endif // PATPIXELTRACK_H
