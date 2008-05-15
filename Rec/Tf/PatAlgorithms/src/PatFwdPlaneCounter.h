// $Id: PatFwdPlaneCounter.h,v 1.3 2008-05-15 17:59:37 mschille Exp $
#ifndef PATFWDPLANECOUNTER_H
#define PATFWDPLANECOUNTER_H 1

// Include files
#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/RegionID.h"

#include <algorithm>
#include "boost/array.hpp"

  /** @class PatFwdPlaneCounter PatFwdPlaneCounter.h
   *  Small class to count how many different planes are in a list.
   *
   *  @author Olivier Callot
   *  @date   2005-04-11 Initial version
   *  @date   2007-08-20 Update for A-Team framework
   *  @date   2008-05-09 Use boost::array instead of std::vector
   */

  class PatFwdPlaneCounter {
  public:

    /// Standard constructor
    template < class Iterator >
    PatFwdPlaneCounter( Iterator first, Iterator last )
      : m_nbDifferent(0)
    {
      std::fill(m_planeList.begin(), m_planeList.end(), 0);
      for ( Iterator ith = first; last != ith; ++ith ) {
        if ( !(*ith)->isSelected() ) continue;
        const int plane = (*ith)->planeCode();
        if ( 0 == m_planeList[plane]++ )
	  ++m_nbDifferent;
      }
    }

    virtual ~PatFwdPlaneCounter( ) {} ///< Destructor

    /// add a hit to be counted
    template < class Hit >
    int addHit( const Hit* hit )
    {
      if ( !hit->isSelected() ) return m_nbDifferent;
      const int plane = hit->planeCode();
      if ( 0 == m_planeList[plane]++ )
        ++m_nbDifferent;
      return m_nbDifferent;
    }

    /// remove a hit
    template < class Hit >
    int removeHit( const Hit* hit )
    {
      if ( !hit->isSelected() ) return m_nbDifferent;
      const int plane = hit->planeCode();
      if ( 0 == --m_planeList[plane] )
	--m_nbDifferent;
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
      if ( 0 < m_planeList[1] ) ++nb;
      if ( 0 < m_planeList[2] ) ++nb;
      if ( 0 < m_planeList[5] ) ++nb;
      if ( 0 < m_planeList[6] ) ++nb;
      if ( 0 < m_planeList[9] ) ++nb;
      if ( 0 < m_planeList[10] ) ++nb;
      return nb;
    }

    /// returns number of plane with most hits
    int maxPlane () const
    {
      PlaneArray::const_iterator max = std::max_element(m_planeList.begin(),
	  m_planeList.end());
      if (0 == *max) return -1;
      return std::distance(m_planeList.begin(), max);
    }

  private:
    // FIXME: is there a way to break things at compile time if we ever
    // have a different number of layers and stations in IT and OT?
    enum { kNPlanes = Tf::RegionID::OTIndex::kNStations *
	Tf::RegionID::OTIndex::kNLayers };
    typedef boost::array< unsigned char, kNPlanes > PlaneArray;

    PlaneArray m_planeList;
    int m_nbDifferent;

  };

#endif // PATFWDPLANECOUNTER_H
