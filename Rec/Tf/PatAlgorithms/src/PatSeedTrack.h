// $Id: PatSeedTrack.h,v 1.3 2008-07-13 22:05:48 mschille Exp $
#ifndef PATSEEDTRACK_H
#define PATSEEDTRACK_H 1

// Include files

#include <cmath>
#include <algorithm>
#include <functional>

#include <boost/array.hpp>
#include <boost/foreach.hpp>

#include "PatKernel/PatForwardHit.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"
#include "TfKernel/STHit.h"

/** @class PatSeedTrack PatSeedTrack.h
 *  This is the working track class during the seeding
 *
 *  @author Olivier Callot
 *  @date   2006-10-17 Initial version
 *  @date   2007-08-20 Updated for A-Team framework 
 *  @date   2008-04-13 Added assignment operator
 *  @date   2008-04-15 Removed unused members
 */

class PatSeedTrack {
  private:
    enum { kNPlanes = Tf::RegionID::OTIndex::kNLayers *
      Tf::RegionID::OTIndex::kNStations };
    typedef boost::array<unsigned char, kNPlanes> PlaneArray;

  public:
    /// Standard constructor
    PatSeedTrack( double x0, double x1, double x2, 
	double z0, double z1, double z2, double zRef, double dRatio);

    PatSeedTrack( PatFwdHit* c0, PatFwdHit* c1, PatFwdHit* c2, PatFwdHit* c3,
	double zRef, double dRatio, double arrow );

    PatSeedTrack( const PatSeedTrack& other) { *this = other; }

    virtual ~PatSeedTrack( ); ///< Destructor

    double z0() const { return m_z0; } ///< return reference z
    double xAtZEqZ0() const { return m_ax; } ///< return x at reference z

    double xAtZ( double z ) const ///< return x at given z
    {
      const double dz = z - m_z0;
      return m_ax + dz * ( m_bx + dz * ( m_cx + dz * m_dx ));
    }

    double xSlope( double z ) const ///< return slope in x at given z
    {
      const double dz = z - m_z0;
      return m_bx + dz * ( 2. * m_cx + 3. * dz * m_dx );
    }

    double yAtZ( double z ) const ///< return y at given z
    { return m_ay + m_by * z; }

    double ySlope( double ) const ///< return slope in y at given z
    { return m_by; }

    double curvature() const ///< return curvature
    { return m_cx; }

    PatFwdHits::const_iterator coordBegin() const { return m_coords.begin(); }
    PatFwdHits::const_iterator coordEnd()   const { return m_coords.end(); }
    PatFwdHits::iterator coordBegin()             { return m_coords.begin(); }
    PatFwdHits::iterator coordEnd()               { return m_coords.end(); }
    const PatFwdHits& coords() const		  { return m_coords; }

    int nCoords() const ///< return number of hits on the track
    { return m_coords.size(); }

    int nPlanes() const ///< return number of planes on the track
    { return m_nbPlanes; }

    double cosine() const ///< return cosine of track angle in xz projection
    { return m_cosine; }

    int nbOnSide() const ///< weighted number of hits (hits in clusters count twice)
    { 
      int nb = 0;
      BOOST_FOREACH(const PatFwdHit* hit, m_coords) {
	if (hit->hasNext()) ++nb;
	if (hit->hasPrevious()) ++nb;
      }
      return nb;
    }

    int nbHighThreshold() const ///< number of hits above ST high threshold
    { return std::count_if( m_coords.begin(), m_coords.end(),
	countIfHighThreshold() ); }

    double chi2() const ///< track chi^2/ndf
    { return m_chi2; }

    void setChi2( double chi2 ) ///< set track chi^2/ndf
    { m_chi2 = chi2; }

    void addCoord( PatFwdHit* hit ) ///< add a hit
    {
      m_coords.push_back( hit );
      if ( 0 == m_planeList[hit->planeCode()]++ ) ++m_nbPlanes;
    }

    void removeCoord( PatFwdHits::iterator& worst ) ///< remove a hit
    {
      if ( 0 == --m_planeList[(*worst)->planeCode()] ) --m_nbPlanes;
      worst = m_coords.erase( worst ) - 1;
    }

    double distance( const PatFwdHit* hit ) const ///< distance track to hit
    {
      double dist = hit->x() - xAtZ( hit->z() );
      if ( 1 < hit->hit()->region() ) return dist;
      dist *= m_cosine;
      const double dx = hit->driftDistance();
      if ( fabs( dist - dx ) < fabs( dist + dx ) )
	return dist - dx;
      else
	return dist + dx;
    }

    /// distance track to hit with ambiguity fixed
    double distanceWithRL( const PatFwdHit* hit ) const
    {
      if ( 0 == hit->rlAmb() ) return distance( hit );
      const double dist = ( hit->x() - xAtZ( hit->z() ) ) * m_cosine;
      const double dx = hit->driftDistance();
      if ( 0 < hit->rlAmb() )
	return dist + dx;
      else
	return dist - dx;
    }

    /// perpendicular distance for fit
    double distanceForFit( const PatFwdHit* hit ) const
    { return distance( hit ) / m_cosine; }

    /// chi^2 contribution of a hit
    double chi2Hit( const PatFwdHit* hit) const
    {
      const double dist = distance( hit );
      return dist * dist * hit->hit()->weight();
    }

    void setYParams( double y0, double sl ) ///< set track parameters in y
    { m_ay = y0; m_by = sl; updateHits( ); }

    void updateHits() ///< update hit positions
    {
      BOOST_FOREACH( PatFwdHit* hit, m_coords )
	updateHitForTrack(hit, m_ay, m_by);
    }

    void sort() ///< sort hits on track by increasing z
    {
      std::sort( m_coords.begin(),
	  m_coords.end(),
	  Tf::increasingByZ<PatForwardHit>() );
    }

    void setValid( bool flag ) ///< set if a track is valid
    { m_valid = flag; }
    
    bool valid() const ///< return if a track is valid
    { return m_valid; }

    /// update track parameters in y
    void updateYParameters( double day, double dby  )
    {
      m_ay += day;
      m_by += dby;
      if ( 0 < m_coords.size() )
	m_z0 += day * (*m_coords.begin())->hit()->dzDy();
      updateHits();
    }

    /// update track parameters in x
    void updateParameters( double dax, double dbx, double dcx )
    {
      m_ax += dax;
      m_bx += dbx;
      m_cx += dcx;
      m_cosine =  1. / std::sqrt( 1. +  m_bx * m_bx  );
    }

    /** predicate to sort tracks by decreasing quality
     * longer tracks (i.e. more hits) are better that shorter ones
     * for tracks with same number of hits, lower chi^2 wins
     */
    struct decreasingQuality {
      public:
	bool operator() (const PatSeedTrack* first, const PatSeedTrack* second ) const {
	  if ( first->nCoords() > second->nCoords() ) return true;
	  if ( first->nCoords() < second->nCoords() ) return false;
	  return first->chi2() < second->chi2();
	};
	bool operator() (const PatSeedTrack& first, const PatSeedTrack& second ) const {
	  if ( first.nCoords() > second.nCoords() ) return true;
	  if ( first.nCoords() < second.nCoords() ) return false;
	  return first.chi2() < second.chi2();
	};
    };

    /// assignment operator
    const PatSeedTrack& operator=(const PatSeedTrack& other)
    {
      m_valid = other.m_valid;
      m_nbPlanes = other.m_nbPlanes;
      m_z0 = other.m_z0;
      m_bx = other.m_bx;
      m_ax = other.m_ax;
      m_cx = other.m_cx;
      m_dx = other.m_dx;
      m_ay = other.m_ay;
      m_by = other.m_by;
      m_cosine = other.m_cosine;
      m_chi2 = other.m_chi2;
      m_planeList = other.m_planeList;
      m_coords = other.m_coords;
      return *this;
    }

  protected:

  private:
    bool   m_valid;
    int    m_nbPlanes;

    double m_z0;
    double m_bx;
    double m_ax;
    double m_cx;
    double m_dx;

    double m_ay;
    double m_by;

    double m_cosine;
    double m_chi2;

    PlaneArray m_planeList;
    PatFwdHits m_coords;

    struct countIfHighThreshold : /// helper predicate for nbHighThreshold
      public std::unary_function<const PatFwdHit*,bool>
  {
    bool operator() (const PatFwdHit* hit) const
    { const Tf::STHit* itHit = hit->hit()->sthit();
      return itHit && itHit->cluster().highThreshold(); }
  };

};

#endif // PATSEEDTRACK_H
// vim:shiftwidth=2:tw=78
