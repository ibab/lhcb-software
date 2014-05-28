// Include files
#include <type_traits>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
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
template < typename  Iterator, typename BinaryFunction > 
BinaryFunction for_each_adjacent_pair(Iterator first, Iterator last, BinaryFunction fun) {
    if ( first==last ) return fun;
    auto prev = first;
    while ( ++first != last ) { 
        fun(*prev,*first);
        prev = first; 
    } 
    return fun;
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
std::pair<Iterator,Iterator> find_first_and_last(Iterator begin, Iterator end, Predicate&& pred) {
    Iterator first = std::find_if( begin, end, pred );
    if ( first == end ) return { first, end };
    auto rbegin = std::reverse_iterator<Iterator>(std::next(first));
    auto second = std::find_if( std::reverse_iterator<Iterator>(end), rbegin, pred );
    return { first, second == rbegin ? end : std::prev(second.base()) };
}

template < typename Iterator, 
           typename Projection, 
           typename Predicate, 
           typename Result = std::pair<typename Iterator::value_type, typename std::result_of<Projection(typename Iterator::value_type)>::type> >
Result
max_projected_element( Iterator first, Iterator last, Projection proj, Result init, Predicate pred ) {
    // TODO: can we write this using std::accumulate???
    for (; first != last; ++first) {
        if ( pred(*first) ) {
            auto x = proj(*first);
            if ( !( x < init.second ) ) {
                init.first  = *first;
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
#if 1
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
#else
      // original version...
      for (; first!=last; ++first ) { 
          auto& value = *first;
          bool pass{  pred(value)  };
          if ( pass && ( first < start || stop < first+1 || !value->isSelected() ) ) {
              start = std::min( start, first   );
              stop  = std::max( stop,  first+1 );
              extended = true;
          }
          value->setSelected( pass );
      }
#endif
      return extended;
}


class RLAmbiguityResolver {
    MsgStream* const m_msg;
    const PatFwdTrackCandidate* const m_track;
    double m_zRef;
    double m_distM = 10.;
    double m_distP = 10.;
public:
    RLAmbiguityResolver( const PatFwdTrackCandidate& track, double zRef,  MsgStream* msg = nullptr ) : 
        m_msg{ msg }, 
        m_track{ &track } ,
        m_zRef(zRef)
    {
    }

    void operator()(PatFwdHit *prev, PatFwdHit *hit) {
      double dz   = hit->z() - m_zRef;
      //OT : distance to a circle, drift time
      double dist = (hit->x() - m_track->x( dz ))*m_track->cosAfter();
      double dx = hit->driftDistance();
      double distM = dist-dx;
      double distP = dist+dx;

      double minDist = 0.3;
      int    vP = 0;
      int    vC = 0;

      if (         fabs( distM - m_distP ) < minDist ) {
        minDist =  fabs( distM - m_distP );  vP = +1;  vC = -1;
      }
      if (         fabs( distP - m_distM ) < minDist ) {
        minDist =  fabs( distP - m_distM );  vP = -1;  vC = +1;
      }
      prev->setRlAmb( vP );
      hit->setRlAmb( vC );

      m_distP = distP;
      m_distM = distM;

      if( UNLIKELY( m_msg!=nullptr ) ) {
        (*m_msg) << format( "  z%10.2f x%10.2f region%2d P%2d N%2d distM%7.3f distP%7.3f minDist%7.3f vC%3d vP%3d",
                           hit->z(), hit->x(), hit->hit()->region(),
                           hit->hasPrevious(), hit->hasNext(), distM, distP, minDist, vC, vP ) << endmsg;
      }
    }
};


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
}
//=============================================================================
// Destructor
//=============================================================================
PatFwdTool::~PatFwdTool() {}

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
  return StatusCode::SUCCESS;
}


double PatFwdTool::xAtReferencePlane ( PatFwdTrackCandidate& track, PatFwdHit* hit,
    bool store ) {

  double x;

  double zHit       = hit->z();
  double xHit       = hit->x();
   
  const Tf::OTHit* othit = hit->hit()->othit();
  
  if ( othit ) {
    if ( hit->hasNext()     ) xHit += hit->driftDistance();
    if ( hit->hasPrevious() ) xHit -= hit->driftDistance();
  }

  if (!m_withoutBField){

    double zMagnet    = ( m_zMagnetParams[0] +
			  m_zMagnetParams[2] * track.slX2() +
			  m_zMagnetParams[3] * hit->x() * hit->x() +
			  m_zMagnetParams[4] * track.slY2() );
    
    double xMagnet    = track.xStraight( zMagnet);
    
    double slopeAfter = ( xHit - xMagnet ) / ( zHit - zMagnet );
    double dSlope     = slopeAfter - track.slX();
    double dSl2       = dSlope * dSlope;
    zMagnet          += m_zMagnetParams[1] * dSl2;
    double dz         = 1.e-3 * ( zHit - m_zReference );
    double dyCoef     = dSl2 * track.slY();
    double dy         = dyCoef * ( m_yParams[0] + dz * m_yParams[1] );
    double dxCoef     = dz * dz * ( m_xParams[0] + m_xParams[1] * dz );
    xHit             += dy * hit->hit()->dxDy() - dxCoef * dSlope ;
    xMagnet           = track.xStraight( zMagnet );
    slopeAfter        = ( xHit - xMagnet ) / ( zHit - zMagnet );
    x                 = xMagnet + ( m_zReference - zMagnet ) * slopeAfter;
    
    if ( store ) {
      track.setParameters( x,
			   ( x - xMagnet ) / ( m_zReference - zMagnet ),
			   1.e-6 * m_xParams[0] * dSlope,
			   1.e-9 * m_xParams[1] * dSlope,
			   track.yStraight( m_zReference ) + dyCoef * m_yParams[0],
			   track.slY() + dyCoef * m_yParams[1] );
      m_zMagnet = zMagnet;
    }
  } else {
    
    double zMagnet    = 0.0;
    double xMagnet    = track.xStraight( zMagnet);
    double slopeAfter = ( xHit - xMagnet ) / ( zHit - zMagnet );
    double dSlope     = 0.0;
    double dyCoef     = 0.0;
    
    x                 = xMagnet + ( m_zReference - zMagnet ) * slopeAfter;
    if ( store ) {
      track.setParameters( x, 
			   ( x - xMagnet ) / ( m_zReference - zMagnet ),
			   1.e-6 * m_xParams[0] * dSlope,
			   1.e-9 * m_xParams[1] * dSlope,
			   track.yStraight( m_zReference ) + dyCoef * m_yParams[0],
			   track.slY() + dyCoef * m_yParams[1] );
      m_zMagnet = zMagnet;
    }
  }
    
  return x;
}


//=========================================================================
//  Fit the X projection of a PatFwdTrackCandidate. Iterative.
//=========================================================================

bool PatFwdTool::fitXCandidate ( PatFwdTrackCandidate& track,
                                 double maxChi2, int minPlanes ) {

  if ( track.setSelectedCoords() < minPlanes ) return false;
  PatFwdPlaneCounter maxPlanes( track.coordBegin(), track.coordEnd() );
  int bestPlanes = maxPlanes.nbDifferent();
  if ( maxPlanes.nbDifferent() < minPlanes ) return false;

  //=== Is there a region with 6 planes ?

  int bestRegion = -1;
  double spread = 1000.;

  auto in_region = [](unsigned int r) {
    return [r](const PatForwardHit* hit) {
      unsigned int region = hit->hit()->region();
      if ( region != Tf::RegionID::OT) region+=2;
      return region == r ;
    };
  };

  PatFwdRegionCounter regions( track.coordBegin(), track.coordEnd() );
  for( unsigned int maxRegion = 0; maxRegion < 6 ; ++maxRegion ) {
    if ( regions.nbInRegion( maxRegion ) < 6 ) continue;  // count by plane
   
    auto predicate = in_region(maxRegion);
    auto hits = find_first_and_last( std::begin(track.coords()), std::end(track.coords()), predicate );
    if ( hits.second == std::end(track.coords()) ) continue;

    double mySpread = (*hits.second)->projection() - (*hits.first)->projection();

    if ( mySpread < spread ) {
        auto planes = make_predicated_planecounter( hits.first, std::next(hits.second), [&](const PatFwdHit* hit) { 
            return hit->isSelected() && predicate(hit); 
        } );
        if ( planes.nbDifferent() == 6 ) {
            spread = mySpread;
            bestRegion = maxRegion;
        }
    }
  }
  bool isDebug = msgLevel( MSG::DEBUG );
  if ( 0 <= bestRegion ) {
    // remove other regions !
    if( UNLIKELY( isDebug ) ) 
      debug() << "========= Keep only hits of region " << bestRegion << endmsg;
    std::for_each( std::begin(track.coords()), std::end(track.coords()), [=](PatForwardHit *hit) {
      int region = hit->hit()->region();
      if ( region != Tf::RegionID::OT) region+=2;
      if ( region != bestRegion ) hit->setSelected( false );
    } );
  }

  auto itBeg = std::begin(track.coords());
  auto itEnd = std::next(itBeg, bestPlanes);
  //== get enough planes fired
  PatFwdPlaneCounter planeCount1( itBeg, itEnd );
  itEnd = planeCount1.addHitsUntilEnough( itEnd, std::end(track.coords()), bestPlanes );
  if ( planeCount1.nbDifferent() < bestPlanes ) return false;


  double minDist = (*std::prev(itEnd))->projection() - (*itBeg)->projection();
  if ( UNLIKELY(isDebug) ) {
    debug() << format( "        range minDist %7.2f from %8.3f to %8.3f bestPlanes %2d",
                       minDist, (*itBeg)->projection(), (*std::prev(itEnd))->projection(), bestPlanes )
            << endmsg;
  }
  //== Better range ? Remove first, try to complete, measure spread...
  auto itBest = itBeg;
  auto itLast = itEnd;
  auto last = std::end(track.coords());
  while ( itEnd != last &&  planeCount1.nbDifferent() >= bestPlanes ) {
    planeCount1.removeHit( *itBeg );
    ++itBeg;
    itEnd = planeCount1.addHitsUntilEnough( itEnd, last, bestPlanes );
    if ( planeCount1.nbDifferent() >= bestPlanes ) {
      auto dist = (*std::prev(itEnd))->projection() - (*itBeg)->projection() ;
      if ( dist < minDist ) {
        minDist = dist;
        itBest = itBeg;
        itLast = itEnd;
        bestPlanes = std::max( bestPlanes, planeCount1.nbDifferent() );
        if ( isDebug ) {
          debug() << format( " better range minDist %7.2f from %8.3f to %8.3f bestPlanes %2d",
                             minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(), bestPlanes )
                  << endmsg;
        }
      }
    }
  }
  //== OK, itBest is the start...

  itBeg = itBest;
  itEnd = itLast;

  //== Add hits before/after
  regions = PatFwdRegionCounter{ itBeg, itEnd };
  double tolSide = ( regions.maxRegion() < 2 ) ? 2.0 : 0.2 ;

  // note that PatForwardTool::buildXCandidatesList has sorted the hits by projection
  // so we can use this ordering here if advantageous... (depends on how far we typically
  // traverse before finding our target

  double minProj = (*itBeg)->projection() - tolSide;
  // choice between std::partition_point and std::find_if depends on the 2log(size_of_range) vs distance
  // from the start to the target...
  itBeg = reverse_find_if( itBeg, std::begin(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() < minProj; });
  // itBeg = reverse_partition_point( itBeg, std::begin(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() >= minProj; });
  double maxProj = (*std::prev(itEnd))->projection() + tolSide;
  itEnd = std::find_if(itEnd, std::end(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() > maxProj; });
  // itEnd = std::partition_point(itEnd, std::end(track.coords()), [=](const PatFwdHit *hit) { return hit->projection() <= maxProj; });

  PatFwdPlaneCounter planeCount( itBeg, itEnd );
  if ( isDebug ) debug() << "... X fit, planeCount " << planeCount.nbDifferent()
                         << " size " << itEnd - itBeg << endmsg;


  // initial value;
  int minHit = std::distance(itBeg,itEnd) / 2;
  xAtReferencePlane( track, track.coords()[minHit], true );

  updateHitsForTrack( track, itBeg, itEnd );
  setRlDefault( track, itBeg, itEnd );

  bool first = true;
  double highestChi2 = 1.e10;

  while ( maxChi2 < highestChi2 && minPlanes <= planeCount.nbDifferent() ) {

    if (!fitXProjection( track, itBeg, itEnd, true )) {
      if( UNLIKELY( isDebug ) ) debug() << "Abandon: Matrix not positive definite." << endmsg;
      return false;
    }

    PatFwdHit* worst = nullptr;
    std::tie( worst, highestChi2 ) = max_projected_element( itBeg, itEnd
                                                          , [&](const PatFwdHit* hit) { return chi2Hit(track,hit); }
                                                          , std::make_pair( worst, 0. )
                                                          , [] (const PatFwdHit* hit) { return hit->isSelected(); });
    if ( highestChi2 > maxChi2 && worst ) {
      planeCount.removeHit( worst );
      worst->setSelected( false );
      if( UNLIKELY( isDebug ) ) 
        verbose() << "--- Remove worst and retry, plane count = " << planeCount.nbDifferent() << endmsg;
    }
    if ( first && highestChi2 <  20 * maxChi2 ) {  // Add possibly removed hits 
      first = false;
      double minChi2 = maxChi2;
      if ( highestChi2 > minChi2 ) minChi2 = highestChi2 - 0.0001;  // down't find again the worst...

      if( UNLIKELY( isDebug ) ) debug() << "Collect all hits with chi2 below " << minChi2 << endmsg;
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

  updateHitsForTrack( track, track.coordBegin(), track.coordEnd() );
  setRlDefault( track, track.coordBegin(), track.coordEnd() );

  double highestChi2 = 10*maxChi2;
  bool   ignoreX = true;

  while ( highestChi2 > maxChi2 ) {
    //== Improve X parameterisation
    if (!fitXProjection( track, track.coordBegin(), track.coordEnd(), false )) {
      if( UNLIKELY( isDebug ) ) debug() << "Abandon: Matrix not positive definite." << endmsg;
	  return false;
    }

    for ( unsigned int kk = 0; kk < 10; ++kk ) {
      PatFwdFitLine line;
      for ( PatFwdHit *hit : track.coords() ) {
        if ( !hit->isSelected()  || hit->hit()->layer() == 0 || hit->hit()->layer() == 3) continue;
        line.addPoint( hit->z() - m_zReference
                     , - distanceForFit( track, hit ) / hit->hit()->dxDy()
                     , hit->hit()->weight() );
      }
      if (!line.solve()) {
        if( UNLIKELY( isDebug ) ) 
          debug() << "Abandon: Matrix not positive definite." << endmsg;
        return false;
      }
      double day = line.ax();
      double dby = line.bx();
      if( UNLIKELY( isDebug ) ) 
        verbose() << "    day " << day << " dby " << dby << endmsg;

      track.updateParameters( 0., 0., 0., 0., day, dby );
      updateHitsForTrack( track, track.coordBegin(), track.coordEnd() );
      if ( fabs( day ) < 0.05 && fabs( dby ) < 0.00005 ) break;
    }

    highestChi2 = 0;
    PatFwdHit* worst = nullptr;
    auto predicate = [=](const PatFwdHit* hit) { return hit->isSelected() && ! ( ignoreX && (hit->hit()->layer()==0 || hit->hit()->layer()==3 ) ) ; };
    auto  chi2hit  = [&](const PatFwdHit* hit) { return chi2Hit(track,hit); };
    std::tie(worst,highestChi2) = max_projected_element( std::begin(track.coords()), std::end(track.coords()),
                                                         chi2hit, std::make_pair( worst, 0. ), predicate);
    if ( highestChi2 > maxChi2 && worst) {
      planeCount.removeHit( worst );
      worst->setSelected( false );
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
template <typename Curve>
bool PatFwdTool::fitXProjection_( PatFwdTrackCandidate& track,
                                  PatFwdHits::const_iterator itBeg,
                                  PatFwdHits::const_iterator itEnd,
                                  bool onlyXPlanes  ) const {

  //= Fit the straight line, forcing the magnet centre. Use only position and slope.

  bool isDebug = msgLevel( MSG::DEBUG );
  double errCenter = m_xMagnetTol + track.dSlope() * track.dSlope() * m_xMagnetTolSlope;
  double weightCenter = 1./errCenter;
  auto dz = m_zMagnet - m_zReference;
  auto make_curve = [=](const PatFwdTrackCandidate& trk) -> Curve { 
      return { dz, distAtMagnetCenter( trk ), weightCenter }; 
  };
  auto accept = [=](const PatFwdHit* hit) {
      if ( !hit->isSelected() ) return false;
      if ( onlyXPlanes && !(hit->hit()->layer()==0 || hit->hit()->layer()==3) ) return false;
      return true;
  };

  for ( unsigned int kk = 0 ; kk < 10  ; ++kk ) {
    auto curve = make_curve(track);
    std::for_each( itBeg, itEnd, [&](const PatFwdHit* hit) {
      if (accept(hit)) curve.addPoint( hit->z() - m_zReference, 
                                       distanceForFit( track, hit ),
                                       hit->hit()->weight() );

    } );
    if (!curve.solve()) return false;
    double dax = curve.ax();
    double dbx = curve.bx();
    double dcx = curve.cx();   

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
template bool PatFwdTool::fitXProjection_<PatFwdFitLine> ( PatFwdTrackCandidate& track,
                          PatFwdHits::const_iterator itBeg,
                          PatFwdHits::const_iterator itEnd,
                          bool onlyXPlanes  ) const;

template bool PatFwdTool::fitXProjection_<PatFwdFitParabola> ( PatFwdTrackCandidate& track,
                          PatFwdHits::const_iterator itBeg,
                          PatFwdHits::const_iterator itEnd,
                          bool onlyXPlanes  ) const;



//=========================================================================
//  Compute the chi2 per DoF of the track
//=========================================================================

double PatFwdTool::chi2PerDoF ( PatFwdTrackCandidate& track ) const {

  //== Error component due to the contraint of the magnet centre
  double totChi2 = chi2Magnet( track );
  if( UNLIKELY( msgLevel( MSG::DEBUG )) ) 
    debug() << "   chi2 magnet center " << totChi2 << endmsg;

  int nDof = -2; // Fitted parabola in X, constraint to magnet center
  bool hasStereo = false;
  for ( PatFwdHit * hit : track.coords() ) {
    if ( !hit->isSelected() ) continue;
    totChi2 += chi2Hit( track, hit );
    nDof    += 1;
    if ( !(hit->hit()->layer()==0 || hit->hit()->layer()==3) ) hasStereo = true;
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
  double magscalefactor = m_magFieldSvc->signedRelativeCurrent() ;
  if( std::abs(magscalefactor) > 1e-6 ) {
    double bx = track.bx();
    double bx2 = bx * bx;
    double slY2 = track.slY2();
    double slX2 = track.slX2();
    double coef = std::inner_product( std::begin(m_momentumParams), std::end(m_momentumParams),
                                      std::begin({ 1.0, bx2, bx2*bx2, bx*track.slX(), slY2, slY2*slY2 }), 0. );
    double proj = sqrt( ( 1. + slX2 + slY2 ) / ( 1. + slX2 ) );
    qop = track.dSlope() / ( coef * Gaudi::Units::GeV * proj * magscalefactor*(-1)) ;
  }
  return qop ;
}

//=========================================================================
//  Returns center of magnet for velo track
//=========================================================================
double PatFwdTool::zMagnet( const PatFwdTrackCandidate& track ) const
{
  //== correction behind magnet neglected
  return  ( m_zMagnetParams[0] +
            m_zMagnetParams[2] * track.slX2() +
            m_zMagnetParams[4] * track.slY2() );
}


//=========================================================================
//  Set the RL flag for OT candidates, if obvious.
//=========================================================================

void PatFwdTool::setRlDefault( PatFwdTrackCandidate& track,
                               PatFwdHits::const_iterator ibegin,
                               PatFwdHits::const_iterator iend  ) const 
{
  bool isDebug = msgLevel( MSG::DEBUG );
  PatFwdHits temp; temp.reserve( std::distance(ibegin,iend) );
  // only OT has ambiguity
  std::copy_if( ibegin, iend, std::back_inserter(temp), [](const PatFwdHit *hit) { 
      return /* hit->isSelected() && */ hit->hit()->type() == Tf::RegionID::OT ;
  } );

  auto first = std::begin(temp);
  auto end   = std::end(temp);

  for ( int planeCode = 0; planeCode < 12 ; ++planeCode ) {
    auto part = std::partition( first, end, [planeCode](const PatFwdHit *hit) { 
        return hit->planeCode() == planeCode ; 
    } );
    std::sort( first, part, Tf::increasingByX<PatForwardHit>() ); 

    if ( UNLIKELY( isDebug ) ) 
      debug() << "-- Hit of plane " << planeCode << endmsg;

    if ( std::distance(first,part) > 1 ) {
      RLAmbiguityResolver  resolve{track, zReference(),  UNLIKELY( isDebug ) ? &debug() : nullptr };
      resolve( *first, *first ); //FIXME: required to retain identical results...
      for_each_adjacent_pair( first, part, std::move(resolve) );
    } else if ( first!=part ) {   // no RL solved if only one hit
      (*first)->setRlAmb(0); 
    }
    first = part;
  }
  assert(first==end);
}
//=============================================================================
