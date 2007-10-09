// $Id: PatSeedTrack.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATSEEDTRACK_H
#define PATSEEDTRACK_H 1

// Include files

#include "PatKernel/PatForwardHit.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/STHit.h"


  /** @class PatSeedTrack PatSeedTrack.h
   *  This is the working track class during the seeding
   *
   *  @author Olivier Callot
   *  @date   2006-10-17 Initial version
   *  @date   2007-08-20 Updated for A-Team framework 
   */

  class PatSeedTrack {
  public:
    /// Standard constructor
    PatSeedTrack( double x0, double x1, double x2, double z0, double z1, double z2, double zRef, double dRatio);

    PatSeedTrack( PatFwdHit* c0, PatFwdHit* c1, PatFwdHit* c2, PatFwdHit* c3,
                  double zRef, double dRatio, double arrow );

    virtual ~PatSeedTrack( ); ///< Destructor

    double z0()                         const { return m_z0; }

    double xAtZ( double z )    const {
      double dz = z - m_z0;
      return m_ax + dz * ( m_bx + dz * ( m_cx + dz * m_dx ));
    }

    double xSlope( double z )  const {
      double dz = z - m_z0;
      return m_bx + dz * ( 2 * m_cx + 3 * dz * m_dx );
    }

    double yAtZ( double z )     const {  return m_ay + m_by * z;  }

    double ySlope( double )     const { return m_by; }

    double curvature()          const { return m_cx; }

    PatFwdHits::const_iterator coordBegin() const { return m_coords.begin(); }
    PatFwdHits::const_iterator coordEnd()   const { return m_coords.end(); }
    PatFwdHits::iterator coordBegin()             { return m_coords.begin(); }
    PatFwdHits::iterator coordEnd()               { return m_coords.end(); }

    int nCoords()                      const { return m_coords.size(); }

    int nPlanes()                      const { return m_nbPlanes; }

    double cosine()                    const { return m_cosine; }

    int nbOnSide() const  {
      int nb = 0;

      for ( PatFwdHits::const_iterator itH = m_coords.begin(); m_coords.end() != itH; ++itH ) {
        PatFwdHit* hit = *itH;
        if ( hit->hasNext()     ) ++nb;
        if ( hit->hasPrevious() ) ++nb;
      }
      return nb;
    }

    int nbHighThreshold() const {
      int nb = 0;
      for ( PatFwdHits::const_iterator itH = m_coords.begin(); m_coords.end() != itH; ++itH ) {
        PatFwdHit* hit = *itH;
        const Tf::STHit* itHit = hit->hit()->sthit();
        if(itHit && itHit->cluster().highThreshold()) ++ nb;
      }
      return nb;
    }

    double chi2() const { return m_chi2; }

    void setChi2( double chi2 ) { m_chi2 = chi2; }

    void addCoord( PatFwdHit* hit ) {
      m_coords.push_back( hit );
      if ( 0 == m_planeList[hit->planeCode()] ) m_nbPlanes++;
      m_planeList[hit->planeCode()]++;
    }

    void removeCoord( PatFwdHits::iterator& worst ) {
      m_planeList[(*worst)->planeCode()]--;
      if ( 0 == m_planeList[(*worst)->planeCode()] ) m_nbPlanes--;
      worst = m_coords.erase( worst ) - 1;
    }

    double distance( PatFwdHit* hit ) {
      double dist = hit->x() - xAtZ( hit->z() );
      if ( 1 < hit->hit()->region() ) return dist;
      dist *= m_cosine;
      double dx = hit->driftDistance();
      if ( fabs( dist-dx ) < fabs( dist+dx ) ) return dist-dx;
      return dist+dx;
    }

    double distanceWithRL( PatFwdHit* hit ) {
      if ( 0 == hit->rlAmb() ) return distance( hit );
      double dist = ( hit->x() - xAtZ( hit->z() ) ) * m_cosine;
      double dx = hit->driftDistance();
      if ( +1 == hit->rlAmb() ) return dist+dx;
      return dist-dx;
    }

    double distanceForFit( PatFwdHit* hit ) {
      return distance( hit ) / m_cosine;
    }

    double chi2Hit( PatFwdHit* hit) {
      double dist = distance( hit );
      return dist * dist * hit->hit()->weight();
    }

    void setYParams( double y0, double sl ) {
      m_ay = y0;
      m_by = sl;
      updateHits( );
    }

    void updateHits ( ) {
      for ( PatFwdHits::const_iterator itH = m_coords.begin(); m_coords.end() != itH; ++itH ) {
        updateHitForTrack((*itH), m_ay, m_by);
      }
    }

    void sort() {
      std::sort( m_coords.begin(),
                 m_coords.end(),
                 Tf::increasingByZ<PatForwardHit>() );
    }

    double worstChi2() { return m_worstChi2;  }

    void setValid( bool flag )    { m_valid = flag; }
    bool valid()           const  { return m_valid; }
    void updateYParameters( double day, double dby  ) {
      m_ay += day;
      m_by += dby;
      if ( 0 < m_coords.size() ) m_z0 += day * (*m_coords.begin())->hit()->dzDy();
      updateHits();
    }

    void updateParameters( double dax, double dbx, double dcx ) {
      m_ax += dax;
      m_bx += dbx;
      m_cx += dcx;
      m_cosine =  1. / sqrt( 1. +  m_bx * m_bx  );
    }

    class decreasingQuality {
    public:
      bool operator() (const PatSeedTrack* first, PatSeedTrack* second ) const {
        if ( first->nCoords() > second->nCoords() ) return true;
        if ( first->nCoords() < second->nCoords() ) return false;
        return first->chi2() < second->chi2();
      };
    };

  protected:

  private:
    bool   m_valid;
    double m_z0;
    double m_ax;
    double m_bx;
    double m_cx;
    double m_dx;

    double m_ay;
    double m_by;

    double m_cosine;
    double m_chi2;
    double m_wireVelo;
    double m_driftVelo;

    PatFwdHits m_coords;
    std::vector<int>  m_planeList;
    int    m_nbPlanes;
    double m_worstChi2;
  };

#endif // PATSEEDTRACK_H
