// $Id: PatFwdPlaneCounter.h,v 1.3 2008-05-15 17:59:37 mschille Exp $
#ifndef PATFWDPLANECOUNTER_H
#define PATFWDPLANECOUNTER_H 1

// Include files
#include <algorithm>
#include <array>

#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/RegionID.h"

#include "PatFwdTrackCandidate.h"


  /** @class PatFwdPlaneCounter PatFwdPlaneCounter.h
   *  Small class to count how many different planes are in a list.
   *
   *  @author Olivier Callot
   *  @date   2005-04-11 Initial version
   *  @date   2007-08-20 Update for A-Team framework
   */

  class PatFwdPlaneCounter final {
  public:

    PatFwdPlaneCounter() = delete;
    /// Standard constructor
    template < class Iterator >
    PatFwdPlaneCounter( Iterator first, Iterator last )
      : m_nbDifferent{0}
    {
      m_planeList.fill(0);
      while (first!=last) {
        if ( (*first)->isSelected() ) {
            auto plane = (*first)->planeCode();
            if ( 0 == m_planeList[plane]++ ) ++m_nbDifferent;
        }
        ++first;
      }
    }

    explicit PatFwdPlaneCounter( const PatFwdTrackCandidate& cand) : PatFwdPlaneCounter( std::begin(cand), std::end(cand) ) {}

    /// add a hit to be counted
    template < class Hit >
    int addHit( const Hit* hit )
    {
      if ( !hit->isSelected() ) return m_nbDifferent;
      const int plane = hit->planeCode();
      if ( 0 == m_planeList[plane]++ ) ++m_nbDifferent;
      return m_nbDifferent;
    }

    /// remove a hit
    template < class Hit >
    int removeHit( const Hit* hit )
    {
      if ( !hit->isSelected() ) return m_nbDifferent;
      auto plane = hit->planeCode();
      if ( 0 == --m_planeList[plane] ) --m_nbDifferent;
      return m_nbDifferent;
    }

    /// returns number of different planes
    int nbDifferent() const { return m_nbDifferent; }

    /// returns number of hits in specified plane
    int nbInPlane( int plane ) const { return m_planeList[plane]; }

    /// returns number of stereo planes with hits
    int nbStereo() const
    {
      // FIXME: If anyone comes up with something nicer which is just as
      // fast, please go ahead
      int nb = 0;
      if ( m_planeList[ 1] !=0 ) ++nb;
      if ( m_planeList[ 2] !=0 ) ++nb;
      if ( m_planeList[ 5] !=0 ) ++nb;
      if ( m_planeList[ 6] !=0 ) ++nb;
      if ( m_planeList[ 9] !=0 ) ++nb;
      if ( m_planeList[10] !=0 ) ++nb;
      return nb;
    }

    /// returns number of plane with most hits
    int maxPlane () const
    {
      auto max = std::max_element(std::begin(m_planeList),std::end(m_planeList));
      if (0 == *max) return -1;
      return std::distance( std::begin(m_planeList), max);
    }

  private:
    // make sure things break at compile time if we ever
    // have a different number of layers and stations in IT and OT
    enum { kNPlanes = Tf::RegionID::OTIndex::kNStations * Tf::RegionID::OTIndex::kNLayers };
    static_assert(kNPlanes == Tf::RegionID::ITIndex::kNStations *
                                  Tf::RegionID::ITIndex::kNLayers,
                  "Number of IT planes and OT planes is not the same?????");
    std::array< unsigned char, kNPlanes >  m_planeList;
    int m_nbDifferent;

  };

#endif // PATFWDPLANECOUNTER_H
