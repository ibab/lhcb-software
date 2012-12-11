#ifndef TRACKKERNEL_TRACKCLONEDATA_H
#define TRACKKERNEL_TRACKCLONEDATA_H

#include "DetDesc/StaticArray.h"
#include "Kernel/LHCbID.h"

namespace  LHCb
{
  // forward declarations
  class Track ;

  // structure to save some data for each track
  class TrackCloneData
  {
  public:
    enum HitType { T, TT, VeloR, VeloPhi, Velo, MaxType } ;
    // constructor
    TrackCloneData(const LHCb::Track& track) ;

    // sort by number of LHCbIDs
    bool operator<( const TrackCloneData& rhs) const { return m_weight > rhs.m_weight ; }
    //
    double overlapFraction( const TrackCloneData& rhs, HitType type ) const ;
    size_t nCommon( const TrackCloneData& rhs, HitType type ) const ;

    // set the weight for ordering
    void setWeight( double w ) { m_weight = w ; }

    // return reference to track
    const LHCb::Track& track() const { return *m_track ; }

    // return pointer to track
    const LHCb::Track* trackp() const { return m_track ; }

  private:
    // we can probably do this with ranges since the lhcbids are
    // ordered by detector type. for now, let's just create containers
    //typedef Range_<LHCb::Track::LHCbIDContainer> LHCbIDs ;
    typedef StaticArray<LHCb::LHCbID,64> LHCbIDs ;
    LHCbIDs m_ids[MaxType] ;
    const LHCb::Track* m_track ;
    double m_weight ;
  } ;
}
  

#endif
