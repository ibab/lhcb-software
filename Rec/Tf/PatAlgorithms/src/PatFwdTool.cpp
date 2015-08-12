// Include files
#include <type_traits>
#include <iterator>

#include "boost/range/iterator_range.hpp"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from TrackEvent
#include "Event/StateParameters.h"

#include "TfKernel/RecoFuncs.h"

// local
#include "PatFwdTool.h"
#include "PatFwdRegionCounter.h"
#include "PatFwdFitParabola.h"
#include "PatFwdFitLine.h"
//-----------------------------------------------------------------------------
// Implementation file for class : PatFwdTool
//
// 2005-04-01 : Olivier Callot
//
//-----------------------------------------------------------------------------

// For convenient trailing-return-types in C++11:
#define AUTO_RETURN(...) noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) {return (__VA_ARGS__);}

constexpr struct proj_distance_t {
       template <typename Range> 
       auto operator()(const Range& range) const //TODO: use C++14 auto return deduction instead
       AUTO_RETURN( range.back()->projection() - range.front()->projection() )
} proj_distance {};

class inRegion {
    unsigned int m_region;
public:
    inRegion(unsigned int region) : m_region(region) {}
    template <typename Hit>
    bool operator()(const Hit* hit) const {
      auto region = hit->hit()->region();
      if ( region != Tf::RegionID::OT) region+=Tf::RegionID::OTIndex::kNRegions ; // TODO: how does this work??? TfRegionID::OT = 4, which is the 'DetType' of the OT...
                                                  // FIXME: shouldn't this instead do what PatFwdRegionCounter::region does???
                                                  // as-is, this 'just' maps IT region 2 and IT region 4 together, and confuses the 
                                                  // this predicate, as 'r' varies from [0,5], and hence will select 
      //if ( hit->hit()->type() != Tf::RegionID::OT) region+=Tf::RegionID::OTIndex::kNRegions ; // I suspect this is what was intended... matching the code in PatFwdRegionCounter...
      return region == m_region;
    }
};

template < typename  Iterator, typename Init, typename Function > 
Init accumulate_adjacent_pairs(Iterator first, Iterator last, Init init, Function fun) {
    if ( first!=last ) {
        auto prev = first;
        while ( ++first != last ) { 
            init = fun(init,*prev,*first);
            prev = first; 
        }
    }
    return init;
}

template < typename  Iterator, typename Predicate > 
Iterator reverse_partition_point(Iterator begin, Iterator end, Predicate&& pred) {
    return std::partition_point( std::reverse_iterator<Iterator>(begin), 
                                 std::reverse_iterator<Iterator>(end), 
                                 std::forward<Predicate>(pred) ).base();
}

template < typename  Iterator, typename Predicate > 
Iterator reverse_find_if(Iterator begin, Iterator end, Predicate&& pred) {
    return std::find_if( std::reverse_iterator<Iterator>(begin), 
                         std::reverse_iterator<Iterator>(end), 
                         std::forward<Predicate>(pred) ).base();
}

template < typename  Iterator, typename Predicate >
std::pair<Iterator,Iterator> find_first_and_last(Iterator begin, Iterator end, Predicate&& pred) 
{
    Iterator first = std::find_if( begin, end, pred );
    if ( first == end ) return { first, end };
    auto rbegin = std::reverse_iterator<Iterator>(std::next(first));
    auto second = std::find_if( std::reverse_iterator<Iterator>(end), rbegin, pred );
    return { first, second == rbegin ? first : second.base() };
}

template < typename Iterator, 
           typename Projection, 
           typename Result,
           typename Predicate >
Result
max_projected_element( Iterator first, Iterator last, Projection proj, Result init, Predicate pred ) 
{
    for ( ; first!=last ; ++first) {
        if ( pred(*first) ) {
            auto x = proj(*first);
            if ( init.second < x ) {
                init.first = first;
                init.second = x;
            }
        }
    }
    return init;
}

template <typename Iterator, typename Predicate> 
bool extend_range(Iterator& start, Iterator& stop,  Iterator first, Iterator last, Predicate pred ) 
{
      bool extended = false;
      // unrolled version...
      assert( first <= start );
      assert( start <= stop );
      assert( stop <= last );

      for (; first!=start;++first) {
          bool pass { pred(*first) } ;
          (*first)->setSelected(pass);
          if ( pass ) {
              start = first;
              ++first;
              extended = true;
              break;
          }
      }
      for (; first!=stop; ++first ) {
          bool pass { pred(*first) } ;
          if ( pass && !(*first)->isSelected() ) extended = true;
          (*first)->setSelected(pass);
      }
      for (;first!=last;++first) {
          bool pass { pred(*first) };
          (*first)->setSelected(pass);
          if ( pass ) {
              stop = first+1;
              extended = true;
          }
      }
      return extended;
}

class RLAmbiguityResolver {
  protected:
    MsgStream* const m_msg;
    const PatFwdTrackCandidate* const m_track;
    const double m_zRef, m_ca;
    /// little helper class to set ambiguities in a pair of hits
    template <bool sameSign>
    struct HitPairAmbSetter {
      /// set ambiguity of h1 to amb, h2 according to sameSign
      template <class HIT>
      static inline void set(HIT& h1, HIT& h2, int amb)
      { h1->setRlAmb(amb); h2->setRlAmb(sameSign ? amb : -amb); }
      /// set ambiguity of h1 to amb, h2 according to sameSign if setsecond is true
      template <class HIT>
      static inline void set(HIT& h1, HIT& h2, int amb, bool setsecond)
      { h1->setRlAmb(amb); if (setsecond) h2->setRlAmb(sameSign ? amb : -amb); }
    };
  public:
    RLAmbiguityResolver( const PatFwdTrackCandidate& track, double zRef,  MsgStream* msg = nullptr ) : 
        m_msg{ msg }, 
        m_track{ &track } ,
        m_zRef{zRef},
        m_ca{ track.cosAfter() }
    {
    }

    std::pair<double,double> operator()(std::pair<double,double> val, PatFwdHit *prev, PatFwdHit *hit) const {
      // catch the case where we switch from one layer to the next
      if (prev == hit || prev->planeCode() != hit->planeCode())
	return std::make_pair(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
      //OT : distance to a circle
      const auto dist = (hit->x() - m_track->x( hit->z() - m_zRef ))*m_ca;
      const auto dx = hit->driftDistance();

      const auto residual = std::make_pair( dist + dx, dist - dx );

      const auto dm = std::abs( residual.second - val.first );
      const auto dp = std::abs( residual.first - val.second );

      const int rl = std::min(dm, dp) > 0.3 ? 0 : (dp < dm ? +1 : -1); 
      HitPairAmbSetter<false>::set(hit, prev, rl);

#if 0
      if( UNLIKELY( m_msg!=nullptr ) ) {
        (*m_msg) << format( "  z%10.2f x%10.2f region%2d P%2d N%2d distM%7.3f distP%7.3f minDist%7.3f vC%3d vP%3d",
                           hit->z(), hit->x(), hit->hit()->region(),
                           hit->hasPrevious(), hit->hasNext(), distM, distP, std::min(dm,dp),rl, -rl ) << endmsg;
      }
#endif
      return residual;
    }
};

class RLAmbiguityResolverFromPitchRes : public RLAmbiguityResolver {
  public:
    RLAmbiguityResolverFromPitchRes( const PatFwdTrackCandidate& track, double zRef,  MsgStream* msg = nullptr ) : 
      RLAmbiguityResolver(track, zRef, msg)
    { }
    std::pair<double,double> operator()(std::pair<double,double> val, PatFwdHit *prev, PatFwdHit *hit) const
    {
      // catch the case where we switch from one layer to the next
      if (LIKELY(prev != hit && prev->planeCode() == hit->planeCode())) {
	// require hits in same module
	const LHCb::OTChannelID ot1(prev->hit()->lhcbID().otID());
	const LHCb::OTChannelID ot2(hit->hit()->lhcbID().otID());
	if (LIKELY(ot1.uniqueModule() == ot2.uniqueModule())) {
	  // check in detail: distance in z at y = 0 must be compatible with
	  // straws in different monolayers and distance in x must be
	  // compatible with the pitch
	  const auto dzabs = std::abs(prev->hit()->zAtYEq0() - hit->hit()->zAtYEq0());
	  const auto dxabs = std::abs(prev->hit()->xAtYEq0() - hit->hit()->xAtYEq0());
	  typedef decltype(dzabs) F;
	  const F dzmax = F(13) / F(2), dzmin = F(9) / F(2);
	  const F dxmax = F( 7) / F(2), dxmin = F(3) / F(2);
	  if (LIKELY(dzmin <= dzabs) && LIKELY(dzabs <= dzmax) &&
	      LIKELY(dxmin <= dxabs) && LIKELY(dxabs <= dxmax)) {
	    // have two neighbouring hits, calculate pitch residual
	    // work out effective slope in module frame
	    const auto sinT = prev->hit()->sinT(), cosT = prev->hit()->cosT();
	    typedef decltype(sinT) F;
	    const auto z0 = (prev->hit()->zAtYEq0() + hit->hit()->zAtYEq0()) / F(2);
	    const auto txeff = m_track->xSlope(z0 - m_zRef) * cosT +
	      m_track->ySlope(z0 - m_zRef) * sinT;
	    // get 2D wire distance vector in module coordinates
	    const auto dx = (prev->hit()->xAtYEq0() - hit->hit()->xAtYEq0()) * cosT;
	    const auto dzdy = prev->hit()->dzDy();
	    const auto dz = (prev->hit()->zAtYEq0() - hit->hit()->zAtYEq0()) /
	      std::sqrt(F(1) + dzdy * dzdy);
	    // calculate effective pitch (i.e. pitch seen by m_track)
	    const auto scale = (dx * txeff + dz) / (F(1) + txeff * txeff);
	    const auto eprx = dx - scale * txeff, eprz = dz - scale;
	    const auto epr = std::sqrt(eprx * eprx + eprz * eprz);
	    // get radii
	    const auto r1 = prev->driftDistance(), r2 = hit->driftDistance();
	    // calculate pitch residual (convert back from effective pitch)
	    // case 1: track passes between hits
	    // case 2: track passes to one side of both hits
	    const auto pr1 = std::abs(dx / epr) * (epr - r1 - r2);
	    const auto pr2 = std::abs(dx / epr) * (epr - std::abs(r1 - r2));
	    // resolve ambiguities only if smaller of pitch res. is smaller
	    // than about three times the pitch residual resolution (pitch
	    // residual resolution is (OT resolution) * sqrt(2), or about 0.283
	    // mm for a well calibrated OT)
	    if (std::min(std::abs(pr1), std::abs(pr2)) <= 1.) {
	      if (std::abs(pr1) <= std::abs(pr2)) {
		// case 1: set ambiguities accordingly
		HitPairAmbSetter<false>::set(hit, prev, (dx < F(0)) ? -1 : +1,
		    // only set amb. on prev hit if better than last pitch res.
		    val.first >= std::abs(pr1));
		return std::make_pair(std::abs(pr1), std::numeric_limits<double>::max());
	      } else {
		// case 2: try to figure out ambiguity by comparing slope
		// estimate obtained from drift radii (assuming hits do not
		// pass in between the wires) to the (well measured) effective
		// slope
		//
		// as the slopes are usually big, we have to correct for the
		// fact that the radius is no longer in x direction (using the
		// measured txeff)
		const auto corr = std::sqrt(F(1) + txeff * txeff);
		const auto dslplus = (dx + (r1 - r2) * corr) - txeff * dz;
		const auto dslminus = (dx - (r1 - r2) * corr) - txeff * dz;
		HitPairAmbSetter<true>::set(hit, prev,
		    (std::abs(dslplus) < std::abs(dslminus)) ? +1 : -1,
		    // only set amb. on prev hit if better than last pitch res.
		    val.first >= std::abs(pr2));
		return std::make_pair(std::abs(pr2), std::numeric_limits<double>::max());
	      } // case 1/2?
	    } // pitch residual small enough?
	  } // neighbouring straws?
	} // same module?
      } // pit == prev?
      // no clue about ambiguity...
      hit->setRlAmb(0);
      return std::make_pair(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    }
};

// find the consequitive range between begin and end which has at least bestPlanes planes, and has the smallest lenght
template <typename Iter>
boost::iterator_range<Iter> 
find_shortest_range(Iter begin, Iter end, int bestPlanes ) 
{
  // assert( std::distance(begin,end) >= bestPlanes );
  // assert( std::is_sorted( begin, end, []( x,y ) { return x->projection()<y->projection() } ) );

  auto range = boost::make_iterator_range(begin,std::next(begin, bestPlanes));
  //== get enough planes fired
  PatFwdPlaneCounter planeCount1( std::begin(range), std::end(range) );
  range.advance_end( std::distance( std::end(range), planeCount1.addHitsUntilEnough( std::end(range), end, bestPlanes )));
  if ( planeCount1.nbDifferent() < bestPlanes ) return {end,end};

  //== Better range ? Remove first, try to complete, measure spread...
  auto minDist = proj_distance(range);
  auto bestRange = range;
  while ( std::end(range) != end &&  planeCount1.nbDifferent() >= bestPlanes ) {
    planeCount1.removeHit( range.front() ); range.advance_begin(1);
    range.advance_end( std::distance( std::end(range), planeCount1.addHitsUntilEnough( std::end(range), end, bestPlanes ) ) );
    if ( planeCount1.nbDifferent() >= bestPlanes ) {
      auto dist = proj_distance( range ) ;
      if ( dist < minDist ) {
        minDist = dist;
        bestRange  = range;
        bestPlanes = std::max( bestPlanes, planeCount1.nbDifferent() );
      }
    }
  }
  return bestRange;
}

template <typename Iter1, typename Iter2, typename Predicate>
int count_planes(Iter1&& first ,Iter2&& last, Predicate predicate ) 
{
        auto pc = make_predicated_planecounter( std::forward<Iter1>(first), std::forward<Iter2>(last), 
                                                [&](const PatFwdHit* hit) { 
            return hit->isSelected() && predicate(hit); 
        } );
        return pc.nbDifferent();

}

template <typename Iter>
void select_hits_in_best_region_only(Iter begin, Iter end)
{
  auto spread = 1000.0;
  enum { nPlanes = 6 };
  enum { nRegions =  Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kNRegions };
  unsigned bestRegion = nRegions;

  PatFwdRegionCounter regions{ begin, end };
  for( unsigned int maxRegion = 0; maxRegion != nRegions ; ++maxRegion ) {
    if ( regions.nbInRegion( maxRegion ) < nPlanes ) continue;  // count by plane
   
    auto predicate = inRegion{maxRegion};
    auto hits = find_first_and_last( begin, end, predicate );
    auto range = boost::make_iterator_range(hits.first, hits.second);
    if ( range.empty() ) continue; // should never happen, if nbInRegion is at least nPlanes, which is > 1 -- provided the way PatFwdRegionCounter counts regions is the same, which it isn't

    auto mySpread = proj_distance(range);
    if ( mySpread < spread && count_planes( std::begin(range), std::end(range), predicate ) == nPlanes ) {
      spread = mySpread;
      bestRegion = maxRegion;
    }
  }

  if ( bestRegion != nRegions ) {
    // remove other regions !
    // if( UNLIKELY( isDebug ) ) debug() << "========= Keep only hits of region " << bestRegion << endmsg;
    auto predicate = inRegion(bestRegion);
    std::for_each( begin, end, [&](PatForwardHit *hit) {
      if (!predicate(hit)) hit->setSelected( false );
    } );
  }
}

template <typename Iterator1, typename Iterator2>
void updateHitsForTrack ( const PatFwdTrackCandidate& track, Iterator1&& begin, Iterator2&& end, double zRef ) {
    auto z0=zRef;
    auto y0=track.y(-z0);
    std::for_each( std::forward<Iterator1>(begin), std::forward<Iterator2>(end) , 
                   [y0,z0,&track](PatForwardHit *hit) {
          updateHitForTrack( hit, y0, track.ySlope( hit->z()-z0 ) ); // does not change 'selected' 
    } );
}

DECLARE_TOOL_FACTORY( PatFwdTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatFwdTool::PatFwdTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<PatFwdTool>(this);

  declareProperty( "ZReference"      , m_zReference      = StateParameters::ZMidT); 
  declareProperty( "ZOutput"          ,m_zOutputs        = {StateParameters::ZBegT,StateParameters::ZMidT,StateParameters::ZEndT});

  declareProperty( "ZMagnetParams"   , m_zMagnetParams   = {});
  declareProperty( "xParams"         , m_xParams         = {});
  declareProperty( "yParams"         , m_yParams         = {});
  declareProperty( "momentumParams"  , m_momentumParams  = {});

  declareProperty( "xMagnetTol"      , m_xMagnetTol      = 3. );
  declareProperty( "xMagnetTolSlope" , m_xMagnetTolSlope = 40. );
  declareProperty( "withoutBField"   , m_withoutBField   = false);
  declareProperty( "AmbiguitiesFromPitchResiduals", m_ambiguitiesFromPitchResiduals = true);
}

//=========================================================================
//  Initialisation, check parameters
//=========================================================================
StatusCode PatFwdTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;

  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

  if ( m_zMagnetParams.empty() && m_momentumParams.empty() &&
       m_xParams.empty() && m_yParams.empty()){
    if (m_magFieldSvc->useRealMap()){
      m_zMagnetParams   = { 5208.05, 318.502, -1223.87, 9.80117e-06, -304.272},
      m_xParams         = { 17.5815, -5.94803},
      m_yParams         = { -979.0, -0.684947},
      m_momentumParams  = { 1.21174, 0.634127, -0.242116, 0.412728, 2.82916, -20.6599};
    } else {
      m_zMagnetParams   = {5199.31, 334.725, -1283.86, 9.59486e-06, -413.281},
      m_xParams         = {16.8238, -6.35556};
      m_yParams         = {-970.89, -0.686028},
      m_momentumParams  = {1.21909, 0.627841, -0.235216, 0.433811, 2.92798, -21.3909};
    }
  }

  if ( 5 != m_zMagnetParams.size() || 6 != m_momentumParams.size() || 
       2 != m_xParams.size() || 2 != m_yParams.size()){
      error() << "The properties ZMagnetParams, xParams, yParams and momentumParams must either"
                 " all be empty, or contain resp. 5, 6, 2 and 2 entries" << endmsg;
      return StatusCode::FAILURE;
  }
  if (UNLIKELY(msgLevel(MSG::DEBUG))) {
    if (m_ambiguitiesFromPitchResiduals) {
      debug() << "Resolving ambiguities from pitch residuals." << endmsg;
    } else {
      debug() << "Resolving ambiguities based on distance to track." << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

#include "VectorizationSupport.h"
template <bool withoutBField, typename ... Hits>
std::array<double, sizeof...(Hits)> 
PatFwdTool::xAtReferencePlane( const PatFwdTrackCandidate& track, double z_magnet, Hits... hits ) const 
{
  using double_v = typename vector_type<double,sizeof...(Hits)>::type;

  auto zHit    = double_v{ hits->z()... };
  auto xHit    = double_v{ hits->x()... };

  // why not do this after the drift distance correction to xHit??
  auto zMagnet = ( !withoutBField ? m_zMagnetParams[3] : 0. )  * xHit * xHit + z_magnet; 
   
  // only OT hits have a non-zero driftDistance. So this is a NOP for everything else...
  xHit        += double_v{ double(int(hits->hasNext())-int(hits->hasPrevious()))... }
               * double_v{ hits->driftDistance()... };

  if (!withoutBField) {
      auto dSlope  = ( xHit - track.xStraight(zMagnet) ) / ( zHit - zMagnet ) - track.slX();
      auto dSl2    = dSlope * dSlope;
      zMagnet     += m_zMagnetParams[1] * dSl2;
      auto dz      = 1.e-3 * ( zHit - zReference() );
      auto dyCoef  = dSl2 * track.slY();
      auto dxdY    = double_v{ hits->hit()->dxDy()... };
      xHit        +=     dyCoef * ( m_yParams[0] + dz * m_yParams[1] ) * dxdY
                      - dz * dz * ( m_xParams[0] + dz * m_xParams[1] ) * dSlope ;
  }
  auto x = track.xStraight( zMagnet );
  x     += ( xHit - x ) * ( zReference() - zMagnet ) / ( zHit - zMagnet );
  return scatter_array<double,sizeof...(Hits)>(x);
}

// explicitly instantiate the relevant templates...
#ifdef PATFWDTOOL_VEC
template std::array<double, 2ul> PatFwdTool::xAtReferencePlane<false,PatForwardHit*, PatForwardHit*>(const PatFwdTrackCandidate&, double, PatForwardHit*, PatForwardHit*) const;
template std::array<double, 2ul> PatFwdTool::xAtReferencePlane<true,PatForwardHit*, PatForwardHit*>(const PatFwdTrackCandidate&, double, PatForwardHit*, PatForwardHit*) const;
#endif
template std::array<double, 1ul> PatFwdTool::xAtReferencePlane<false,PatForwardHit*>(const PatFwdTrackCandidate&, double, PatForwardHit* ) const;
template std::array<double, 1ul> PatFwdTool::xAtReferencePlane<true,PatForwardHit*>(const PatFwdTrackCandidate&, double, PatForwardHit* ) const;

double PatFwdTool::updateTrackAndComputeZMagnet( PatFwdTrackCandidate& track, const PatFwdHit* hit ) const {

  auto zHit       = hit->z();
  auto xHit       = hit->x();
   
  xHit += (int( hit->hasNext()) - int(hit->hasPrevious()))*hit->driftDistance();

  auto zMagnet = 0.0;
  if (!m_withoutBField){

    zMagnet += ( m_zMagnetParams[0] +
				 m_zMagnetParams[2] * track.slX2() +
				 m_zMagnetParams[3] * hit->x() * hit->x() +
				 m_zMagnetParams[4] * track.slY2() );
    
    auto xMagnet    = track.xStraight( zMagnet);
    
    auto slopeAfter = ( xHit - xMagnet ) / ( zHit - zMagnet );
    auto dSlope     = slopeAfter - track.slX();
    auto dSl2       = dSlope * dSlope;
    zMagnet          += m_zMagnetParams[1] * dSl2;
    auto dz         = 1.e-3 * ( zHit - zReference() );
    auto dyCoef     = dSl2 * track.slY();
    auto dy         = dyCoef * ( m_yParams[0] + dz * m_yParams[1] );
    auto dxCoef     = dz * dz * ( m_xParams[0] + m_xParams[1] * dz );
    xHit             += dy * hit->hit()->dxDy() - dxCoef * dSlope ;
    xMagnet           = track.xStraight( zMagnet );
    slopeAfter        = ( xHit - xMagnet ) / ( zHit - zMagnet );
    auto x          = xMagnet + ( zReference() - zMagnet ) * slopeAfter;
    
    track.setParameters( x,
	  	   ( x - xMagnet ) / ( zReference() - zMagnet ),
	  	   1.e-6 * m_xParams[0] * dSlope,
	  	   1.e-9 * m_xParams[1] * dSlope,
	  	   track.yStraight( zReference() ) + dyCoef * m_yParams[0],
	  	   track.slY() + dyCoef * m_yParams[1] );
  } else {
    
    auto xMagnet    = track.xStraight( zMagnet);
    auto slopeAfter = ( xHit - xMagnet ) / ( zHit - zMagnet );
    auto dSlope     = 0.0;
    auto dyCoef     = 0.0;
    
    auto x          = xMagnet + ( zReference() - zMagnet ) * slopeAfter;
    track.setParameters( x, 
	  	   ( x - xMagnet ) / ( zReference() - zMagnet ),
	  	   1.e-6 * m_xParams[0] * dSlope,
	  	   1.e-9 * m_xParams[1] * dSlope,
	  	   track.yStraight( zReference() ) + dyCoef * m_yParams[0],
	  	   track.slY() + dyCoef * m_yParams[1] );
  }
  return zMagnet;
}


//=========================================================================
//  Fit the X projection of a PatFwdTrackCandidate. Iterative.
//=========================================================================

bool PatFwdTool::fitXCandidate ( PatFwdTrackCandidate& track,
                                 double maxChi2, int minPlanes ) 
{

  if ( track.setSelectedCoords() < minPlanes ) return false;
  PatFwdPlaneCounter maxPlanes( track.coordBegin(), track.coordEnd() );
  int bestPlanes = maxPlanes.nbDifferent();
  if ( bestPlanes < minPlanes ) return false;

  bool isDebug = msgLevel( MSG::DEBUG );

  //=== Is there a region with 6 planes ?
  select_hits_in_best_region_only( std::begin(track.coords()), std::end(track.coords()) );

  //=== find shortest range with enough planes
  auto rng = find_shortest_range( std::begin(track.coords()), std::end(track.coords()), bestPlanes);
  if (rng.empty()) return false;
  auto itBeg = std::begin(rng);
  auto itEnd = std::end(rng);

  //== Add hits before/after
  PatFwdRegionCounter regions{ itBeg, itEnd };
  auto tolSide = ( regions.maxRegion() < Tf::RegionID::OTIndex::kNRegions ) ? 2.0 : 0.2 ;

  // note that PatForwardTool::buildXCandidatesList has sorted the hits by projection
  // so we can use this ordering here if advantageous... (depends on how far we typically
  // traverse before finding our target

  auto minProj = (*itBeg)->projection() - tolSide;
  // choice between std::partition_point and std::find_if depends on the 2log(size_of_range) vs distance
  // from the start to the target...
  itBeg = reverse_find_if( itBeg, std::begin(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() < minProj; });
  // itBeg = reverse_partition_point( itBeg, std::begin(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() >= minProj; });
  auto maxProj = (*std::prev(itEnd))->projection() + tolSide;
  itEnd = std::find_if(itEnd, std::end(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() > maxProj; });
  // itEnd = std::partition_point(itEnd, std::end(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() <= maxProj; });

  // initial value;
  int minHit = std::distance(itBeg,itEnd) / 2;
  m_zMagnet = updateTrackAndComputeZMagnet( track, track.coords()[minHit] ); // how to get this to the fitXProjection inside fitStereoCandidate????? Add it to the the track!!!

  updateHitsForTrack( track, itBeg, itEnd, zReference() );
  setRlDefault( track, itBeg, itEnd );

  bool   first = true;
  auto highestChi2 = 1.e10;
  PatFwdPlaneCounter planeCount( itBeg, itEnd );
  if ( isDebug ) debug() << "... X fit, planeCount " << planeCount.nbDifferent()
                         << " size " << itEnd - itBeg << endmsg;

  while ( highestChi2 > maxChi2 && minPlanes <= planeCount.nbDifferent() ) {

    if (!fitXProjection( track, itBeg, itEnd, true )) { // uses m_zMagnet...
      if( UNLIKELY( isDebug ) ) debug() << "Abandon: Matrix not positive definite." << endmsg;
      return false;
    }

    auto worst = itEnd;
    std::tie( worst, highestChi2 ) = max_projected_element( itBeg, itEnd
                                                          , [&](const PatFwdHit* hit) { return chi2Hit(track,hit); }
                                                          , std::make_pair( worst, 0. )
                                                          , [] (const PatFwdHit* hit) { return hit->isSelected(); });
    if ( highestChi2 > maxChi2 && worst!=itEnd ) {
      planeCount.removeHit( *worst );
      (*worst)->setSelected( false );
      if( UNLIKELY( isDebug ) ) 
        verbose() << "--- Remove worst and retry, plane count = " << planeCount.nbDifferent() << endmsg;
    }
    if ( first && highestChi2 <  20 * maxChi2 ) {  // Add possibly removed hits 
      first = false;
      auto minChi2 = std::max( highestChi2 - 0.0001, maxChi2 );  // don't find again the worst...

      if( UNLIKELY( isDebug ) ) debug() << "Collect all hits with chi2 below " << minChi2 << endmsg;
      // WARNING: itBeg, itEnd must point into the same container as std::begin(track.coords)!!!!
      bool hasNewHits = extend_range( itBeg, itEnd, std::begin(track.coords()), std::end(track.coords()), 
                                    [&](const PatFwdHit* hit) { return !hit->isIgnored() && chi2Hit(track,hit) < minChi2; } );
      if ( hasNewHits ) {
        planeCount = PatFwdPlaneCounter{ itBeg, itEnd }; // update counter...
        if( UNLIKELY( isDebug ) ) debug() << "   new plane count " << planeCount.nbDifferent() << endmsg;
        //setRlDefault( track, itBeg, itEnd );
        highestChi2 = 2*maxChi2; // force an iteration...
      } else if( UNLIKELY( isDebug ) ) {
        debug() << "   no new hit added to the track." << endmsg;
      }
    }
  }
  return minPlanes <= planeCount.nbDifferent();
}


//=========================================================================
//  Fit only Y with the stereo hits.
//=========================================================================
bool PatFwdTool::fitStereoCandidate ( PatFwdTrackCandidate& track,
                                      double maxChi2, int minPlanes ) const {

  bool isDebug = msgLevel( MSG::DEBUG );

  //== get enough planes fired
  PatFwdPlaneCounter planeCount( track.coordBegin(), track.coordEnd() );
  if ( planeCount.nbDifferent() < minPlanes ) return false;

  if ( isDebug ) debug() << "+++ Stereo fit, planeCount " << planeCount.nbDifferent()
                         << " size " << track.coordEnd() - track.coordBegin() << endmsg;

  updateHitsForTrack( track, track.coordBegin(), track.coordEnd(), zReference() );
  setRlDefault( track, track.coordBegin(), track.coordEnd() );

  auto highestChi2 = 10*maxChi2;
  bool   ignoreX = true;

  while ( highestChi2 > maxChi2 ) {
    //== Improve X parameterisation
    if (!fitXProjection( track, track.coordBegin(), track.coordEnd(), false )) { // uses m_zMagnet -- move it into PatFwdTrackCandidate...
      if( UNLIKELY( isDebug ) ) debug() << "Abandon: Matrix x-fit not positive definite." << endmsg;
	  return false;
    }

    if (!fitYProjection(track, track.coordBegin(), track.coordEnd())) {
      if( UNLIKELY( isDebug ) ) debug() << "Abandon: Matrix y-fit not positive definite." << endmsg;
	  return false;
    }

    highestChi2 = 0;
    auto worst = std::end(track.coords());
    auto predicate = [=](const PatFwdHit* hit) { return hit->isSelected() && ! ( ignoreX && hit->isX() ) ; };
    auto  chi2hit  = [&](const PatFwdHit* hit) { return chi2Hit(track,hit); };
    std::tie(worst,highestChi2) = max_projected_element( std::begin(track.coords()), std::end(track.coords()),
                                                         chi2hit, std::make_pair( worst, 0. ), predicate);
    if ( highestChi2 > maxChi2 && worst!=std::end(track.coords())) {
      planeCount.removeHit( *worst );
      (*worst)->setSelected( false );
      if( UNLIKELY( isDebug ) ) debug() << " Remove hit and try again " << endmsg;
      //== Remove in one go all hits with bad contribution...
      if ( highestChi2 > 1000. ) {
        for ( PatFwdHit *hit : track.coords() ) {
          if (predicate(hit) &&  chi2hit( hit ) > 1000. ) {
            planeCount.removeHit( hit );
            hit->setSelected( false );
          }
        }
      }

      if ( planeCount.nbDifferent() < minPlanes ) {
        if( UNLIKELY( isDebug ) ) 
          debug() << " Abandon: Only " << planeCount.nbDifferent() << " planes, min " << minPlanes
                  << " highestChi2 " << highestChi2 << endmsg;
        return false;
      }
    }
    //== If almost there, force one iteration with X fitting, at least...
    if ( highestChi2 < 2 * maxChi2 && ignoreX ) {
        ignoreX = false;
        highestChi2 = 2.* maxChi2;
    }
  }

  if( UNLIKELY( isDebug ) ) 
    debug() << ".. OK with " << planeCount.nbDifferent() << " planes, min " << minPlanes
            << " highestChi2 " << highestChi2 << endmsg;
  return planeCount.nbDifferent() >= minPlanes ;
}


//=========================================================================
//  fit to projection.
//=========================================================================
bool PatFwdTool::fitYProjection( PatFwdTrackCandidate& track,
                                 PatFwdHits::const_iterator itBeg,
                                 PatFwdHits::const_iterator itEnd ) const 
{
    auto accept = [=](const PatFwdHit* hit) {
      return hit->isSelected() && !hit->isX(); 
    };
    bool isDebug = msgLevel( MSG::DEBUG );

    for ( unsigned int kk = 0; kk < 10; ++kk ) {
      PatFwdFitLine line;
      auto ica = 1.0/track.cosAfter();
      std::for_each( itBeg, itEnd, [&](const PatFwdHit* hit) {
        if (accept(hit)) line.addPoint( hit->z() - zReference()
                                      , distanceForYFit( track, hit, ica )
                                      , hit->hit()->weight() );
      } );
      if (!line.solve()) return false;
      auto day = line.ax();
      auto dby = line.bx();
      if( UNLIKELY( isDebug ) ) verbose() << "    day " << day << " dby " << dby << endmsg;

      track.updateParameters( 0., 0., 0., 0., day, dby );
      updateHitsForTrack( track, track.coordBegin(), track.coordEnd(), zReference() );
      if ( fabs( day ) < 0.05 && fabs( dby ) < 0.00005 ) break;
    }
    return true;
}

//TODO: remove the 'onlyXPlanes' option, move selection to caller
template <bool withoutBField>
bool PatFwdTool::fitXProjection_( PatFwdTrackCandidate& track,
                                  PatFwdHits::const_iterator itBeg,
                                  PatFwdHits::const_iterator itEnd,
                                  bool onlyXPlanes  ) const 
{
  //= Fit the straight line, forcing the magnet centre. Use only position and slope.
  bool isDebug = msgLevel( MSG::DEBUG );
  auto errCenter = m_xMagnetTol + track.dSlope() * track.dSlope() * m_xMagnetTolSlope;
  auto weightCenter = 1./errCenter;
  auto dz = m_zMagnet - zReference();

  using Curve = typename std::conditional<withoutBField, PatFwdFitLine, PatFwdFitParabola>::type;
  auto make_curve = [=](const PatFwdTrackCandidate& trk) -> Curve { 
      return { dz, distAtMagnetCenter( trk ), weightCenter }; 
  };

  auto accept = [=](const PatFwdHit* hit) {
      return hit->isSelected() && ( !onlyXPlanes || hit->isX() ) ;
  };


  for ( unsigned int kk = 0 ; kk < 10  ; ++kk ) {
    auto curve = make_curve(track);
    auto ica = 1.0/track.cosAfter();
    std::for_each( itBeg, itEnd, [&](const PatFwdHit* hit) {
      if (accept(hit)) curve.addPoint( hit->z()-zReference(), 
                                       distanceForXFit(track,hit,ica),
                                       hit->hit()->weight() );
    } );
    if (!curve.solve()) return false;
    auto dax = curve.ax();
    auto dbx = curve.bx();
    auto dcx = curve.cx();   

    track.updateParameters( dax, dbx, dcx );

    if( UNLIKELY( isDebug ) ) {
      verbose() << format( " dax %10.4f dbx%10.4f dcx %10.4f distCent %10.2f",
                           dax, dbx*1.e3, dcx*1.e6, distAtMagnetCenter( track ) ) << endmsg;
    }
    if ( fabs( dax ) < 5.e-3 &&
         fabs( dbx ) < 5.e-6 &&
         fabs( dcx ) < 5.e-9    ) break;  // wait until stable, due to OT.
  }
  return true;
}

// explicitly instantiate the two valid versions...
template bool PatFwdTool::fitXProjection_<true>  ( PatFwdTrackCandidate& , PatFwdHits::const_iterator , PatFwdHits::const_iterator , bool onlyXPlanes ) const;
template bool PatFwdTool::fitXProjection_<false> ( PatFwdTrackCandidate& , PatFwdHits::const_iterator , PatFwdHits::const_iterator , bool onlyXPlanes ) const;

//=========================================================================
//  Compute the chi2 per DoF of the track
//=========================================================================

double PatFwdTool::chi2PerDoF ( PatFwdTrackCandidate& track ) const {

  //== Error component due to the contraint of the magnet centre
  auto totChi2 = chi2Magnet( track );
  if( UNLIKELY( msgLevel( MSG::DEBUG )) ) 
    debug() << "   chi2 magnet center " << totChi2 << endmsg;

  int nDof = -2; // Fitted parabola in X, constraint to magnet center
  bool hasStereo = false;
  for ( PatFwdHit * hit : track.coords() ) {
    if ( !hit->isSelected() ) continue;
    totChi2 += chi2Hit( track, hit );
    nDof    += 1;
    if ( !hit->isX() ) hasStereo = true;
  }
  if ( hasStereo ) nDof -= 1;  // Linear fit, magnet center constraint

  totChi2 /= nDof;

  track.setChi2PerDoF( totChi2 , nDof);

  return totChi2;
}

//=========================================================================
//  Returns the q/p of the track
//=========================================================================
double PatFwdTool::qOverP ( const PatFwdTrackCandidate& track ) const {
  double qop(1.0/Gaudi::Units::GeV) ;
  auto magscalefactor = m_magFieldSvc->signedRelativeCurrent() ;
  if ( std::abs(magscalefactor) > 1e-6 ) {
    auto bx = track.bx();
    auto bx2 = bx * bx;
    auto slY2 = track.slY2();
    auto slX2 = track.slX2();
    auto coef = std::inner_product( std::begin(m_momentumParams), std::end(m_momentumParams),
                                    std::begin({ 1.0, bx2, bx2*bx2, bx*track.slX(), slY2, slY2*slY2 }), 0.0 );
    auto proj = sqrt( ( 1. + slX2 + slY2 ) / ( 1. + slX2 ) );
    qop = track.dSlope() / ( coef * Gaudi::Units::GeV * proj * magscalefactor*(-1)) ;
  }
  return qop ;
}



//=========================================================================
// visit pairs of hits in the same layer
//=========================================================================
template <class IT, class FN>
static inline void visitHitPairs(IT begin, const IT end, const FN& fn)
{
  accumulate_adjacent_pairs(begin, end, 
      fn(std::make_pair(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()), *begin, *begin),
      std::cref(fn));
}

//=========================================================================
//  Set the RL flag for OT candidates, if obvious.
//=========================================================================
void PatFwdTool::setRlDefault( PatFwdTrackCandidate& track,
                               PatFwdHits::const_iterator begin,
                               PatFwdHits::const_iterator end  ) const 
{
  // should not have more than 64 hits on a track, but if we do, the code keeps
  // working with minimal performance loss (up to 2 hits in 64 for which the
  // ambiguity is not resolved, but only if we have more than 64 OT hits on a
  // track...)
  std::array<PatFwdHit*, 64> hits;
  auto hbegin = std::begin(hits), hend = std::begin(hits);
  for (auto it = begin; end != it; hend = hbegin) {
    const unsigned howmany = std::min(unsigned(std::distance(it, end)), 64u);
    // only OT has ambiguity
    hend = std::copy_if(it, it + howmany, hend, [](const PatFwdHit *hit) { 
	return /* hit->isSelected() && */ hit->isOT();
	} );
    if (std::distance(hbegin, hend) > 1) {
      // sort such that neighbouring hits end up next to each other
      std::sort(hbegin, hend, [] (const PatFwdHit* h1, const PatFwdHit* h2) {
	  if (h1->planeCode() < h2->planeCode()) return true;
	  else if (h2->planeCode() < h1->planeCode()) return false;
	  else if (h1->hit()->xAtYEq0() < h2->hit()->xAtYEq0()) return true;
	  else if (h2->hit()->xAtYEq0() < h1->hit()->xAtYEq0()) return false;
	  else return h1->hit()->lhcbID() < h2->hit()->lhcbID();
	  });
      // resolve ambiguities
      if (m_ambiguitiesFromPitchResiduals) {
	const RLAmbiguityResolverFromPitchRes resolve{track, zReference(),
	  UNLIKELY( msgLevel( MSG::DEBUG ) ) ? &debug() : nullptr};
	visitHitPairs(hbegin, hend, resolve);
      } else {
	const RLAmbiguityResolver resolve{track, zReference(),
	  UNLIKELY( msgLevel( MSG::DEBUG ) ) ? &debug() : nullptr};
	visitHitPairs(hbegin, hend, resolve);
      }
    } else if (1 == std::distance(hbegin, hend)) {
      (*hbegin)->setRlAmb(0);
    }
    it += howmany;
  }
}
//=============================================================================
