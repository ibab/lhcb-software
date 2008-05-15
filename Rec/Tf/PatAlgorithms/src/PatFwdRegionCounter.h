// $Id: PatFwdRegionCounter.h,v 1.4 2008-05-15 17:59:37 mschille Exp $
#ifndef PATFWDREGIONCOUNTER_H
#define PATFWDREGIONCOUNTER_H 1

// Include files

#include "TfKernel/HitExtension.h"
#include "TfKernel/RegionID.h"

#include <algorithm>
#include "boost/array.hpp"

  /** @class PatFwdRegionCounter PatFwdRegionCounter.h
   *  Simple object to count the hits by region.
   *
   *  @author Olivier Callot
   *  @date   2005-07-07 Initial version
   *          2007-08-20 Update for A-Team framework  SHM
   *          2008-05-09 use boost::array instead of std::vector (M. Schiller)
   */
  class PatFwdRegionCounter {
  public:
    virtual ~PatFwdRegionCounter( ) {} ///< Destructor

    /// Standard constructor
    PatFwdRegionCounter( PatFwdHits::const_iterator first,
	PatFwdHits::const_iterator last )
    {
      std::fill(m_regionList.begin(), m_regionList.end(), 0);
      for ( PatFwdHits::const_iterator ith = first; last != ith; ++ith ) {
        if ( !(*ith)->isSelected() ) continue;
        ++m_regionList[region(*ith)];
      }
    }

    /// add a hit to be counted
    void addHit( const PatFwdHit* hit )
    { ++m_regionList[region(hit)]; }

    /// return number of hits in specified region
    int nbInRegion( int region ) const
    { return m_regionList[region]; }

    /// return number of hits counted in IT
    int nbIT() const
    {
      return std::accumulate(m_regionList.begin() +
	  Tf::RegionID::OTIndex::kNRegions, m_regionList.end(), 0);
    }

    /// return number of hits counted in OT
    int nbOT() const
    {
      return std::accumulate(m_regionList.begin(),
	  m_regionList.begin() + Tf::RegionID::OTIndex::kNRegions, 0);
    }

    /// return the region with most hits
    int maxRegion() const
    {
      RegionArray::const_iterator max =
	std::max_element(m_regionList.begin(), m_regionList.end());
      if (0 == *max) return -1;
      return std::distance(m_regionList.begin(), max);
    }

  protected:

  private:
    enum { kNRegions = Tf::RegionID::OTIndex::kNRegions +
      Tf::RegionID::ITIndex::kNRegions };
    typedef boost::array< unsigned char, kNRegions > RegionArray;

    RegionArray m_regionList; /// array of counters

    /// helper to compute the region of a hit
    int region(const PatFwdHit* hit) const
    {
      int region = hit->hit()->region();
      if (hit->hit()->type() != Tf::RegionID::OT)
	region += Tf::RegionID::OTIndex::kNRegions;
      return region;
    }

  };

#endif // PATFWDREGIONCOUNTER_H
