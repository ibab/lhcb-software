// $Id: $
#ifndef PATPIXELTRACK_H 
#define PATPIXELTRACK_H 1

// Include files
#include "PatPixelHit.h"
#include "PatPixelSensor.h"
#include "Event/StateVector.h"

namespace LHCb {
  class State ;
}

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

  void set( PatPixelHit* h1, PatPixelHit* h2 );                        // start a seed track from the two given hits

  void setBackward( bool flag )           { m_backward = flag; }       // set(clear) "a-backward-track" flag
  inline bool backward()            const { return m_backward; }

  inline PatPixelHits& hits()             { return m_hits; }           // return the list of hits
  void addHit   ( PatPixelHit* hit );                                  // add a new hit to this track
  void removeHit( PatPixelHit* hit );                                  // remove given hit
  void solve();                                                        // (re)calculate the fit from the x/y-slope sums

  void add( PatPixelHits& hits )                                       // add a vector of hits to this track
  { for ( PatPixelHits::const_iterator itH = hits.begin(); hits.end() != itH; ++itH )
    {  addHit( *itH ); }
  }

  void tagUsedHits()                                                   // mark hits of this track as being associated
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

  int firstSensor() const { return m_hits.front()->sensor(); }
  int lastSensor()  const { return m_hits.back()->sensor(); }

  int extrapolate(double &x, double &y, double &dist, PatPixelHit *hit) const // extrapolate to a given hit
  { return extrapolate(x,y,dist, hit->z()); }

  int extrapolate(double &x, double &y, double &dist, double z) const   // extrapolate to given Z
  { int size = m_hits.size(); if(size<2) return 0;
    if(z > m_hits[0     ]->z() ) return frontExtrapolate(x,y,dist, z);
    if(z < m_hits[size-1]->z() ) return  backExtrapolate(x,y,dist, z);
    return 0; }

  // extrapolate using the two last hits (with lowest Z)
  int backExtrapolate(double &x, double &y, double &dist, double z) const
  { int size = m_hits.size(); if(size<2) return 0;                 // check if at least two hits are there
    PatPixelHit *hit1 = m_hits[size-1];                            // last hit
    PatPixelHit *hit2 = m_hits[size-2];                            // one before last hit
    double x1=hit1->x(); double y1=hit1->y(); double z1=hit1->z(); // last hit: x1,y1,z1
    double x2=hit2->x(); double y2=hit2->y(); double z2=hit2->z(); // one before last hit: x2,y2,z2
    dist = z1-z; double dz = z2-z1;                                // Z distance from last hit to extrapolation Z
    x = x1 + dist*(x1-x2)/dz;                                      // X at extrapolation Z
    y = y1 + dist*(y1-y2)/dz;                                      // Y at extrapolation Z
    return 1; }                                                    // Z distance to the extrapolation Z including half the hit separation

  // extrapolate using the two first hits (with largest Z)
  int frontExtrapolate(double &x, double &y, double &dist, double z) const
  { int size = m_hits.size(); if(size<2) return 0;                 // check if at least two hits are there
    PatPixelHit *hit0 = m_hits[0];                                 // first hit
    PatPixelHit *hit1 = m_hits[1];                                 // second hit
    double x0=hit0->x(); double y0=hit0->y(); double z0=hit0->z(); // first hit x0,y0,z0
    double x1=hit1->x(); double y1=hit1->y(); double z1=hit1->z(); // second hit x0,y0,z0
    dist = z-z0; double dz = z0-z1;
    x = x0 + dist*(x0-x1)/dz;
    y = y0 + dist*(y0-y1)/dz;
    return 1; }

  LHCb::StateVector state( double z )  // create a track state vector: position and the slopes: dx/dz and dy/dz
  { LHCb::StateVector temp;
    temp.setX ( m_x0 + z * m_tx );
    temp.setY ( m_y0 + z * m_ty );
    temp.setZ ( z  );
    temp.setTx( m_tx );
    temp.setTy( m_ty );
    temp.setQOverP( 0. );          // Q/P is Charge/Momentum ratio
    return temp; }

  double zBeam() const            // z-pos. where the track passes closest to the beam
  { return -( m_x0 * m_tx + m_y0 * m_ty ) / ( m_tx * m_tx + m_ty * m_ty ); }

  Gaudi::TrackSymMatrix covariance( double z );

  /// Fit with a K-filter with scattering. Return the chi2
  double fitKalman( LHCb::State& state, int direction, double noisePerLayer ) const ;
  
  void print(void) const
  { // double z=zBeam();
    printf("PatPixelTrack[%d]:\n", (int)m_hits.size() );
    // printf("PatPixelTrack[%d]: chi2=%3.1f slope=[%+6.3f,%+6.3f] zBeam=%+6.1f [%+5.1f,%+5.1f] %c\n",
    //         (int)m_hits.size(), chi2perDoF(), m_tx, m_ty, z, xAtZ(z), yAtZ(z), m_backward?'B':'F' );
    for ( PatPixelHits::const_iterator itH = m_hits.begin(); m_hits.end() != itH; ++itH )
    { (*itH)->print(); }
    // { double z=(*itH)->z(); (*itH)->print( xAtZ(z), yAtZ(z) ); }
    
  }

  int getMinMaxRadius(double &minR, double &maxR) const // get smaller and largest hit radius
  { int size = m_hits.size();
    if(size<1) return 0;
    maxR = m_hits.front()->r();                  // assume the hits are sorted accord. to sensors and the track is about radial.
    minR = m_hits.back()->r();
    if(minR>maxR) { double tmp=maxR; maxR=minR; minR=tmp; }
    return size; }

  int size(void) const { return m_hits.size(); } // number of hits assigned to the track

protected:

private:
  bool          m_backward;     // backward or forward track
  PatPixelHits  m_hits;         // list of (pointer to) hits

                                // track fit as a straight line
  double m_x0;                  // x at z=0
  double m_tx;                  // dx/dz slope
  double m_y0;                  // y at z=0
  double m_ty;                  // dy/dz slope

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

typedef std::vector<PatPixelTrack> PatPixelTracks; // vector of tracks

#endif // PATPIXELTRACK_H
