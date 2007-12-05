// $Id: PatFwdRegionCounter.h,v 1.2 2007-12-05 10:37:21 smenzeme Exp $
#ifndef PATFWDREGIONCOUNTER_H
#define PATFWDREGIONCOUNTER_H 1

// Include files

#include "TfKernel/HitExtension.h"
#include "TfKernel/RegionID.h"

  /** @class PatFwdRegionCounter PatFwdRegionCounter.h
   *  Simple object to count the hits by region.
   *
   *  @author Olivier Callot
   *  @date   2005-07-07 Initial version
   *          2007-08-20 Update for A-Team framework  SHM
   */

  class PatFwdRegionCounter {
  public:
    /// Standard constructor

    ~PatFwdRegionCounter( ) {}; ///< Destructor


    /// Standard constructor
    PatFwdRegionCounter( PatFwdHits::const_iterator first, PatFwdHits::const_iterator last )
      : m_regionList(10,0)
    {
      for ( PatFwdHits::const_iterator ith = first; last != ith; ++ith ) {
        if ( !(*ith)->isSelected() ) continue;
        int region  = (*ith)->hit()->region();
	if ((*ith)->hit()->type() != Tf::RegionID::OT)
	  region+=2;
        m_regionList[region]++;
      }
    };

    void addHit( PatFwdHit* hit ) { 
      int region = hit->hit()->region();
      if (hit->hit()->type() != Tf::RegionID::OT)
	region+=2;
      m_regionList[region]++; }

    int maxRegion() {
      int l = -1;
      int max = 0;
      for ( unsigned int kk = 0; m_regionList.size() > kk; ++kk ) {
        if ( m_regionList[kk] > max ) {
          max = m_regionList[kk];
          l   = kk;
        }
      }
      return l;
    }

    int nbInRegion( int region ) const { return m_regionList[region]; }

    int nbIT() const { return m_regionList[2] +  m_regionList[3] + m_regionList[4] + m_regionList[5] ; }

    int nbOT() const { return m_regionList[0] +  m_regionList[1]; }

  protected:

  private:
    std::vector<int> m_regionList;

  };

#endif // PATFWDREGIONCOUNTER_H
