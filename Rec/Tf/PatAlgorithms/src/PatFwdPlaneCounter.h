// $Id: PatFwdPlaneCounter.h,v 1.3 2008-05-15 17:59:37 mschille Exp $
#ifndef PATFWDPLANECOUNTER_H
#define PATFWDPLANECOUNTER_H 1

// Include files
#include <algorithm>
#include <array>

#include "TfKernel/RegionID.h"
#include "PatFwdTrackCandidate.h"

/** @class PatFwdPlaneCounter PatFwdPlaneCounter.h
 *  Small class to count how many different planes are in a list.
 *
 *  @author Olivier Callot
 *  @date   2005-04-11 Initial version
 *  @date   2007-08-20 Update for A-Team framework
 */

namespace PatFwdPlaneCounter_Policy
{

struct RequireSelected
{
    template <typename Hit>
    static bool check( const Hit* hit )
    {
        return hit->isSelected();
    }
};
template < typename Pred >
class UsePredicate
{
    Pred m_predicate;
public:
    UsePredicate(Pred pred) : m_predicate{ std::move(pred) } {}
    template <typename Hit>
    bool check( const Hit* hit )
    {
        return m_predicate(hit);
    }
};
struct OmitSelectedCheck
{
    template <typename Hit>
    static bool check( const Hit* )
    {
        return true;
    }
};

}

// Use 'empty base class' optimziation to avoid increasing the size
// when the predicate only contains static items...
template <typename Predicate = PatFwdPlaneCounter_Policy::RequireSelected>
class PatFwdPlaneCounter_ final : private Predicate
{
  public:
    PatFwdPlaneCounter_() = delete;
    /// Standard constructor
    template <typename Iterator, typename ... Args>
    PatFwdPlaneCounter_( Iterator first, Iterator last, Args&&... args  )
        : Predicate{ std::forward<Args>(args)...  },
          m_nbDifferent{0}
    {
        m_planeList.fill( 0 );
        for ( ; first != last; ++first ) addHit( *first );
    }

    template <typename ... Args>
    explicit PatFwdPlaneCounter_( const PatFwdTrackCandidate& cand, Args&&... args)
        : PatFwdPlaneCounter_( std::begin( cand ), std::end( cand ) , std::forward<Args>(args)...  )
    {
    }

    /// add a hit to be counted
    template <typename Hit>
    int addHit( const Hit* hit )
    {
        if ( this->check( hit ) ) {
            auto plane = hit->planeCode();
            assert( m_planeList[plane] != -1 ); // this would cause an overflow...
            if ( 0 == m_planeList[plane]++ ) ++m_nbDifferent;
        }
        return m_nbDifferent;
    }

    template <typename Iterator>
    Iterator addHitsUntilEnough( Iterator current, Iterator end, int nPlanes )
    {
        for ( ; current != end && m_nbDifferent < nPlanes; ++current ) addHit( *current );
        return current;
    }

    template <typename Iterator, typename Pred>
    Iterator addHitsWhile( Iterator current, Iterator end, Pred predicate )
    {
        for ( ; current != end && predicate(*current); ++current )  addHit( *current );
        return current;
    }

    template <typename Iterator>
    Iterator addHits( Iterator current, Iterator end )
    {
        for ( ; current != end ; ++current ) addHit( *current );
        return current;
    }

    /// remove a hit
    template <class Hit>
    int removeHit( const Hit* hit )
    {
        if ( this->check( hit ) ) {
            auto plane = hit->planeCode();
            if ( 0 == --m_planeList[plane] ) --m_nbDifferent;
        }
        return m_nbDifferent;
    }

    /// returns number of different planes
    int nbDifferent() const
    {
        return m_nbDifferent;
    }

    /// returns number of hits in specified plane
    int nbInPlane( int plane ) const
    {
        return m_planeList[plane];
    }

    /// returns number of stereo planes with hits
    int nbStereo() const
    {
        // FIXME: If anyone comes up with something nicer which is just as
        // fast, please go ahead
        int nb = 0;
        if ( m_planeList[1] != 0 ) ++nb;
        if ( m_planeList[2] != 0 ) ++nb;
        if ( m_planeList[5] != 0 ) ++nb;
        if ( m_planeList[6] != 0 ) ++nb;
        if ( m_planeList[9] != 0 ) ++nb;
        if ( m_planeList[10] != 0 ) ++nb;
        return nb;
    }

    /// returns number of plane with most hits
    int maxPlane() const
    {
        auto max = std::max_element( std::begin( m_planeList ), std::end( m_planeList ) );
        return *max != 0 ? std::distance( std::begin( m_planeList ), max ) : -1;
    }

  private:
    // make sure things break at compile time if we ever
    // have a different number of layers and stations in IT and OT
    enum {
        kNPlanes = Tf::RegionID::OTIndex::kNStations * Tf::RegionID::OTIndex::kNLayers
    };
    static_assert( kNPlanes == Tf::RegionID::ITIndex::kNStations *
                                   Tf::RegionID::ITIndex::kNLayers,
                   "Number of IT planes and OT planes is not the same?????" );
    std::array<unsigned char, kNPlanes> m_planeList;
    int m_nbDifferent;
};

using PatFwdPlaneCounter = PatFwdPlaneCounter_<>;

template <typename Iterator, typename Iterator2, typename Predicate>
auto make_predicated_planecounter( Iterator&& begin, Iterator2&& end, Predicate pred ) 
-> PatFwdPlaneCounter_<PatFwdPlaneCounter_Policy::UsePredicate<Predicate>>
{  return { std::forward<Iterator>(begin), std::forward<Iterator2>(end), pred }; }
#endif // PATFWDPLANECOUNTER_H
