// $Id: PatFwdRegionCounter.h,v 1.4 2008-05-15 17:59:37 mschille Exp $
#ifndef PATFWDREGIONCOUNTER_H
#define PATFWDREGIONCOUNTER_H 1

// Include files

#include "TfKernel/RegionID.h"
#include <algorithm>
#include <array>

/** @class PatFwdRegionCounter PatFwdRegionCounter.h
 *  Simple object to count the hits by region.
 *
 *  @author Olivier Callot
 *  @date   2005-07-07 Initial version
 *          2007-08-20 Update for A-Team framework  SHM
 */
class PatFwdRegionCounter final {
public:

  /// Standard constructor
  PatFwdRegionCounter( PatFwdHits::const_iterator first, PatFwdHits::const_iterator last )
  {
    m_regionList.fill(0);
    std::for_each( first, last, [&](const PatFwdHit* hit) {
      if ( hit->isSelected() ) ++m_regionList[region(hit)];
    } );
  }

  /// add a hit to be counted //TODO: inconsistent with constructor!!!
  void addHit( const PatFwdHit* hit )
  { ++m_regionList[region(hit)]; }

  /// return number of hits in specified region
  int nbInRegion( int region ) const
  { return m_regionList[region]; }

  /// return number of hits counted in IT
  int nbIT() const
  {
    return std::accumulate( std::next(std::begin(m_regionList), Tf::RegionID::OTIndex::kNRegions), 
                            std::end(m_regionList), 0);
  }

  /// return number of hits counted in OT
  int nbOT() const
  {
    auto first = std::begin(m_regionList);
    return std::accumulate( first, std::next(first, Tf::RegionID::OTIndex::kNRegions), 0);
  }

  /// return the region with most hits
  int maxRegion() const
  {
    auto max = std::max_element(std::begin(m_regionList), std::end(m_regionList));
    return *max!=0 ?  std::distance(std::begin(m_regionList), max) : -1;
  }


private:
  enum { kNRegions = Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kNRegions };
  std::array< unsigned char, kNRegions > m_regionList; /// array of counters

  /// helper to compute the region of a hit
  int region(const PatFwdHit* hit) const
  {
    int region = hit->hit()->region();
    return (hit->hit()->type() == Tf::RegionID::OT) ? 
                  region : 
                  region + Tf::RegionID::OTIndex::kNRegions;
  }

};

#endif // PATFWDREGIONCOUNTER_H
