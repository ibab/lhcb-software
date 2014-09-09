
// Include files
#include <cmath>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"

#include "OTDet/DeOTDetector.h"

// local
#include "PatForwardTool.h"
#include "PatFwdPlaneCounter.h"
#include "PatFwdRegionCounter.h"

constexpr struct compByX_t {
  template <typename T> bool operator() (const T* hit, double testVal) const
  {
    return hit->hit()->xAtYEq0() < testVal;
  }
  template <typename T> bool operator() (double testVal, const T* hit)  const
  {
    return testVal < hit->hit()->xAtYEq0();
  }
} compByX{};

//-----------------------------------------------------------------------------
// Implementation file for class : PatForwardTool
//
// 2005-10-04 : Olivier Callot
//-----------------------------------------------------------------------------

static const unsigned int nSta = Tf::RegionID::OTIndex::kNStations;
static const unsigned int nLay = Tf::RegionID::OTIndex::kNLayers;
static const unsigned int nReg = Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kNRegions;
static const unsigned int nOTReg = Tf::RegionID::OTIndex::kNRegions;
static const unsigned int nITReg = Tf::RegionID::ITIndex::kNRegions;


DECLARE_TOOL_FACTORY( PatForwardTool )

constexpr struct sortQuality {
  bool operator()( const PatFwdTrackCandidate& first,
                   const PatFwdTrackCandidate& second )
  {
    bool sortDecision = ( first.quality() < second.quality());
    if(first.quality() == second.quality()){
      sortDecision = ( first.chi2PerDoF() < second.chi2PerDoF());
      if(first.chi2PerDoF() == second.chi2PerDoF()){
        sortDecision = ( first.qOverP() < second.qOverP());
      }
    }
    return sortDecision;
  }
} sortQuality_{};

struct MaxSpread {
    double spread;
    MaxSpread(double spread_) : spread { spread_ } {}
    bool operator()(const PatForwardHit* first, const PatForwardHit *last) const {
            return last->projection() < first->projection() + spread;
    }
};

//== Count how many weighted hits

template <typename C>
int nbT(const C& c) { return 2*c.nbIT()+c.nbOT(); }

int nT(const PatFwdTrackCandidate& c) {
    return nbT( PatFwdRegionCounter{  std::begin(c), std::end(c) } );
}

template <typename Iterator>
int nbDifferent( Iterator first, Iterator last)  {
    return PatFwdPlaneCounter{ first, last }.nbDifferent();
}
int nbDifferent( const PatFwdTrackCandidate &c) {
    return nbDifferent( std::begin(c), std::end(c) );
}

template <typename Iter>
class RangeFinder {
    Iter m_last;
    int m_minPlanes;
    Iter next(Iter current) const { return std::next( std::move(current), m_minPlanes-1 ); }
public:
    RangeFinder( int minPlanes, Iter last ) :  m_last{ std::move(last) }, m_minPlanes{minPlanes} { }
    template <typename Iterator> bool operator()(Iterator&& first, Iterator&& last) const { return std::distance(std::forward<Iterator>(first),std::forward<Iterator>(last)) > m_minPlanes-1; }
    template <typename Iterator> bool operator()(Iterator&& first ) const { return std::distance(std::forward<Iterator>(first),m_last) > m_minPlanes-1; }

    template <typename  Iterator,typename Predicate>
    std::pair<Iterator,Iterator> next_range(Iterator first, Predicate predicate)
    {
        auto mid = this->next(first);
        using reference = typename std::iterator_traits<Iterator>::reference;
        //== If not enough hits in the maximum spread, skip
        if ( !predicate(*first,*mid)  ) {
          // first increment first, as we KNOW the current first fails predicate..
          first = std::find_if(++first, mid, [&](reference i) { return predicate(i,*mid);});
          return { first, first };
        }
        //== Add all hits inside the maximum spread.
        // first increment mid, as we KNOW the current mid passes predicate..
        // typically, we find something after 2 steps -- which is smaller than the 2log(size_of_range)
        //  hence, the linear std::find_if is expected to outperform logarithmic  std::partition_point...
        // (typically in 5% of the cases, 2^distance_to_first_match will be larger than range_size)
        mid = std::find_if( ++mid, m_last, [&](reference i) { return !predicate(*first,i); } );
        // mid = std::partition_point( ++mid, m_last, [&](reference i) { return predicate(*first,i); } );

        //== Enough different planes?
        // Note: at this point, only 'selected' hits are used, so we can omit
        //       the check inside the plane counter....
        PatFwdPlaneCounter_<PatFwdPlaneCounter_Policy::OmitSelectedCheck> counter{ first, mid };
        auto enough_planes = [&]() { return counter.nbDifferent() >= m_minPlanes; } ;
        if ( !enough_planes() ) {
          ++first;
          return { first, first };
        }
        //== Try to make a single zone, by removing the first and adding other as
        //   long as the spread and minPlanes conditions are met.
        auto j = mid;
        auto i = first;
        while ( std::distance(i,mid) != (m_minPlanes-1)  && j != m_last ) {
          counter.removeHit(*i);
          ++i;
          for (; j!=m_last && predicate(*i,*j) ; ++j ) counter.addHit(*j);
          if ( enough_planes() ) mid = j;
        }
        return { first, mid };
    }
};

template <typename Iterator>
RangeFinder<typename std::decay<Iterator>::type>
make_RangeFinder( int nPlanes, Iterator&& last)
{
    return { nPlanes, std::forward<Iterator>(last) };
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatForwardTool::PatForwardTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : base_class( type, name , parent ),
      m_nnSwitch(false)
{
  declareInterface<IPatForwardTool>(this);
  declareInterface<ITracksFromTrack>(this);

  declareProperty( "SecondLoop"            , m_secondLoop            = true );
  declareProperty( "ZAfterVelo"            , m_zAfterVelo            = 1640. * Gaudi::Units::mm );
  declareProperty( "YCompatibleTol"        , m_yCompatibleTol        =   10. * Gaudi::Units::mm );
  declareProperty( "YCompatibleTolFinal"   , m_yCompatibleTolFinal   =    1. * Gaudi::Units::mm );
  declareProperty( "MinXPlanes"            , m_minXPlanes            =   5   );
  declareProperty( "MinPlanes"             , m_minPlanes             =   9   );
  declareProperty( "MinOTDrift"            , m_minOTDrift            = -0.3 * Gaudi::Units::mm  );
  declareProperty( "MaxOTDrift"            , m_maxOTDrift            =  2.5 * Gaudi::Units::mm  );
  declareProperty( "MaxSpreadX"            , m_maxSpreadX            =  0.6  );
  declareProperty( "MaxSpreadSlopeX"       , m_maxSpreadSlopeX       = .011  );
  declareProperty( "MaxSpreadY"            , m_maxSpreadY            =  1.5  );
  declareProperty( "MaxSpreadSlopeY"       , m_maxSpreadSlopeY       = 70.   );
  declareProperty( "MinPt"                 , m_minPt                 = 80. * Gaudi::Units::MeV );
  declareProperty( "MinMomentum"           , m_minMomentum           =  1. * Gaudi::Units::GeV );
  declareProperty( "MaxChi2"               , m_maxChi2               =  40       );
  declareProperty( "MaxChi2Track"          , m_maxChi2Track          =  40       );
  declareProperty( "MinHits"               , m_minHits               =  12       );
  declareProperty( "MinOTHits"             , m_minOTHits             =  14       );
  declareProperty( "CenterOTYSize"         , m_centerOTYSize         =  100 * Gaudi::Units::mm );
  declareProperty( "MaxDeltaY"             , m_maxDeltaY             =  30.      );
  declareProperty( "MaxDeltaYSlope"        , m_maxDeltaYSlope        =  300.      );
  declareProperty( "MaxXCandidateSize"     , m_maxXCandidateSize     =  50       );

  declareProperty( "MagnetKickParams"      , m_magnetKickParams      =  { 1255 * Gaudi::Units::MeV, 175 * Gaudi::Units::MeV } );

  declareProperty( "UseMomentumEstimate", m_useMomentumEstimate = false );
  declareProperty( "MomentumEstimateError"    , m_momentumEstimateError    =  0.5 );
  declareProperty( "MinRange"              , m_minRange              =   300. * Gaudi::Units::mm  );

  declareProperty("StateErrorX2",  m_stateErrorX2  =   4.0);
  declareProperty("StateErrorY2",  m_stateErrorY2  = 400.);
  declareProperty("StateErrorTX2", m_stateErrorTX2 =  6.e-5);
  declareProperty("StateErrorTY2", m_stateErrorTY2 =  1.e-4);
  declareProperty("StateErrorP",   m_stateErrorP   =  0.15);

  declareProperty("AddTTClusterName", m_addTtToolName = "" );

  declareProperty( "WithoutBField"         , m_withoutBField         = false);

  declareProperty("Preselection" , m_Preselection = false);
  declareProperty("PreselectionPT",m_PreselectionPT = 400.*Gaudi::Units::MeV);
  declareProperty("UseWrongSignWindow",m_UseWrongSignWindow = false);
  declareProperty("WrongSignPT",m_WrongSignPT = 2000.*Gaudi::Units::MeV);
  declareProperty("FlagUsedSeeds",m_FlagUsedSeeds = false);
  declareProperty("SkipUsedSeeds",m_SkipUsedSeeds = false);

}

//=========================================================================
//  Initialization, get the tools
//=========================================================================
StatusCode PatForwardTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_tHitManager  = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
  m_fwdTool      = tool<PatFwdTool>( "PatFwdTool", this);

  if ( "" != m_addTtToolName  ) {
    m_addTTClusterTool = tool<IAddTTClusterTool>( m_addTtToolName, this );
  } else {
    m_addTTClusterTool = nullptr;
  }


  return StatusCode::SUCCESS;
}

//=========================================================================
//  Main execution: Extend a track.
//=========================================================================
void PatForwardTool::forwardTrack( const LHCb::Track* tr, LHCb::Tracks* output ) {

  std::vector<LHCb::Track*> outvec;
  tracksFromTrack(*tr,outvec).ignore();

  if (outvec.empty() && m_secondLoop) {

      int minXPlanes = m_minXPlanes;
      m_minXPlanes = minXPlanes - 1;

      int minPlanes = m_minPlanes;
      m_minPlanes =  minPlanes - 1;

      tracksFromTrack(*tr,outvec).ignore();

      m_minXPlanes = minXPlanes;
      m_minPlanes = minPlanes;

  }
  output->reserve(output->size()+outvec.size());
  for ( const auto& i : outvec ) output->insert(i);
}

StatusCode PatForwardTool::tracksFromTrack( const LHCb::Track& seed,
                                            std::vector<LHCb::Track*>& output ){

  if ( seed.checkFlag( LHCb::Track::Invalid  ) ) return StatusCode::SUCCESS;
  if ( seed.checkFlag( LHCb::Track::Backward ) ) return StatusCode::SUCCESS;
  if ( m_SkipUsedSeeds && seed.checkFlag( LHCb::Track::Used ) ) {  // check if seed has already successfully been upgraded
    counter("#SkippedUsedSeeds") += 1;
    return StatusCode::SUCCESS;
  }

  counter("#Seeds") += 1;

  PatFwdTrackCandidate track( &seed );
  if(m_Preselection && seed.pt() < m_PreselectionPT && 0 != track.qOverP()) return StatusCode::SUCCESS;

  bool isDebug = msgLevel( MSG::DEBUG );
  if ( isDebug ) {
    debug() << format( "**** Velo track %3d  x%8.2f  y%8.2f  tx%9.5f  ty%9.5f q/p = %8.6f",
                       seed.key(), track.xStraight( m_zAfterVelo ),
                       track.yStraight( m_zAfterVelo ),
                       track.slX(), track.slY(), 1000. * track.qOverP() ) << endmsg;
  }
  // Insure hits are properly prepared. Without this call (which can be made
  // multiple times without penalty) the 'next' and 'previous' information,
  // used in the x @ zRefenence computation of OT hits, will be missing.
  // Also, the order of IT hits in the overlap regions is not guaranteed to be
  // correct.
  m_tHitManager->prepareHits();

  //== Build the initial list of X candidates
  buildXCandidatesList( track );

  int minPlanes = m_minPlanes;  //== Initial value, can be updated later...

  std::vector<PatFwdTrackCandidate> xCandidates;

  int minOTX = int( 1.5 * m_minXPlanes );
  auto accept = [=](const PatFwdTrackCandidate& temp) {
      //== Check the chi2 with magnet center constraint.
      //== Count how many weighted hits
      //== reject if below threshold
      return temp.chi2PerDoF() < m_maxChi2Track &&
             ( !m_withoutBField || fabs(temp.slX()-temp.xSlope(0.))<0.005) &&
             ( nT(temp) >= minOTX || inCenter(temp) ) &&
             passMomentum( temp, track.sinTrack() );
  };

  for( auto& icand : m_candidates ) {

    //== Fit the candidate, and store them
    while ( m_fwdTool->fitXCandidate( icand, m_maxChi2, m_minXPlanes ) ) {

      // Copy the whole stuff, keep only used ones and store it.
      PatFwdTrackCandidate temp = icand;
      temp.cleanCoords();
      m_fwdTool->chi2PerDoF( temp );  // Compute and store chi2/dof
      if ( isDebug ) {
        debug() << "Chi2/nDof = " << temp.chi2PerDoF() << " nDoF " << temp.nDoF()
                << " dist at center " << m_fwdTool->distAtMagnetCenter( temp )
                << endmsg;
        debugFwdHits( temp );
      }
      if ( accept(temp) ) xCandidates.push_back( std::move(temp) );

      //== tag these coordinates in the original, so that we don't find the same track again
      for ( auto* hit :  icand ) {
        if ( hit->isSelected() ) hit->setIgnored( true );
      }
    }
  }

  if ( isDebug ) {
    debug() << "************ List of X candidates , N = " << xCandidates.size() << endmsg;
    unsigned nCand=0;
    for ( const auto& itL : xCandidates ) {
      debug() << "Candidate " << nCand++
              << " Chi2/nDof = " << itL.chi2PerDoF() << endmsg;
      debugFwdHits( itL );
    }
    if ( !xCandidates.empty() ) debug() << "---- Now get the stereo hits on these ---" << endmsg;
  }

  //== Now try to get space track from these X track.
  std::vector<PatFwdTrackCandidate> goodCandidates;
  int maxPlanes = 0;

  unsigned nCand = 0;
  for ( PatFwdTrackCandidate& temp :  xCandidates ) { //TODO: make this one big 'remove_if'?
    if( UNLIKELY( isDebug ) )
      debug() << "--- Candidate " << ++nCand
              << "  X cord size " << temp.coords().size()
              << endmsg;

    for ( auto *hit : temp.coords() ) hit->setIgnored( false);  // restore normal flag.
    temp.setSelectedCoords( );

    double qOverP = computeQOverP(temp);  // in 1/GeV
    if ( !fillStereoList( temp, computeStereoTol(qOverP) ) ) continue; // Get the stereo coordinates

    if ( isDebug )  debugFwdHits( temp );
    temp.setSelectedCoords( );

    //== Fit and reject if not good enough
    if ( !m_fwdTool->fitStereoCandidate( temp, m_maxChi2, minPlanes ) ) continue;

    temp.cleanCoords();
    m_fwdTool->chi2PerDoF( temp );  //== compute and store Chi2PerDoF
    if ( isDebug ) {
      debug() << "  ... fit OK  chi2 " << temp.chi2PerDoF() << " nDoF " << temp.nDoF()
              << " dist at center " << m_fwdTool->distAtMagnetCenter( temp )
              << endmsg;
      debugFwdHits( temp );
    }

    //== Remove stereo coordinates incompatible in Y
    if ( !m_fwdTool->removeYIncompatible( temp, m_yCompatibleTolFinal, minPlanes ) ) continue;
    temp.cleanCoords();
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "  ... Y is compatible" << endmsg;

     // Enough stereo planes
    PatFwdPlaneCounter fullCount( std::begin(temp), std::end(temp) );
    int nbY = fullCount.nbStereo();
    if ( nbY < 4 ) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Not enough Y planes : " << nbY << endmsg;
      continue;
    }

    if ( m_maxDeltaY + m_maxDeltaYSlope * qOverP *qOverP < fabs(  m_fwdTool->changeInY( temp ) ))  {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "  --- Too big change in Y : " <<  m_fwdTool->changeInY( temp ) << endmsg;
      continue;
    }

    temp.setQuality( computeQuality(temp, qOverP) );

    //== Verify if enough OT measurements, counting IT for 2/plane
    //== Ignore the y central region, OT inefficient there.

    PatFwdRegionCounter regions( std::begin(temp), std::end(temp) );
    temp.setNbIT( regions.nbIT() );
    temp.setNbOT( regions.nbOT() );

    if ( !inCenter(temp) ) {
      if ( m_minHits > nbT(temp) ){
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "  --- Not enough hits : " << nbT(temp) << endmsg;
        continue;
      }
      if ( temp.nbIT() == 0 && temp.nbOT() < m_minOTHits ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << " Too few OT for OT only track : " << temp.nbOT() << endmsg;
        continue;
      }
    }
    maxPlanes = std::max(maxPlanes, fullCount.nbDifferent());

    //== reject if below threshold
    if (!passMomentum( temp, track.sinTrack())) continue;

    //== Update requirement according to already found good solutions...
    minPlanes = std::max( minPlanes,   nbDifferent(temp)-1 );
    goodCandidates.push_back( std::move(temp) );
  }


  // added for Tr/NNTools -- sort all candidates with respect to PatQuality
  if( this->nnSwitch()){
    std::stable_sort( std::begin(goodCandidates), std::end(goodCandidates), sortQuality_);
    // loop over all candidates
    for( auto& all : goodCandidates ) {
	  all.setCand1stQuality(all.quality());
      if(goodCandidates.size() == 1) {
	    all.setCand2ndQuality(0.);
      } else {
        auto i2 = std::find_if( std::begin(goodCandidates), std::end(goodCandidates),
                                [&](const PatFwdTrackCandidate& c) { return c.quality() != all.quality(); } );
        if (i2!=std::end(goodCandidates)) { all.setCand2ndQuality( i2->quality() ); }
      }
    }
  }
  // end of NNTools loop
  //================================================================================
  //  Now some filtering of tracks, in case of multiple candidates.
  //================================================================================

  if ( goodCandidates.size() > 1  ) {

    // remove track with sensibly lower number of planes
    int minPlanes = maxPlanes - 1;
    auto last = std::remove_if( std::begin(goodCandidates), std::end(goodCandidates),
                                [=](const PatFwdTrackCandidate& t) { return nbDifferent(t) < minPlanes; } );
    // remove worst quality
    // TODO: could fold this accumulate into the lambda above by capturing bestQuality by reference
    auto bestQuality = std::accumulate( std::begin(goodCandidates), last, 1000.,
                                  [](double m, const PatFwdTrackCandidate& t) { return std::min(m,t.quality()); } );
    bestQuality += 1.0;
    last = std::remove_if( std::begin(goodCandidates), last,
                           [=](const PatFwdTrackCandidate& t) { return t.quality()  >= bestQuality; }  );
    // remove if sensibly less OT
    // TODO: could fold this accumulate into the lambda above by capturing maxOT by reference
    auto maxOT = std::accumulate( std::begin(goodCandidates), last, 0,
                                  [](int m, const PatFwdTrackCandidate& t) { return std::max(m,nbT(t)); } );
    maxOT = std::min(24,maxOT)-3;
    last = std::remove_if( std::begin(goodCandidates), last,
                           [=](const PatFwdTrackCandidate& t) { return nbT(t)  <= maxOT; }  );
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
        debug() << "Ignoring " << std::distance(last,std::end(goodCandidates))
                << " candidates -- either not enough planes, or bad quality, or not enough OT" << endmsg;
    }
    goodCandidates.erase(last,std::end(goodCandidates));
  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Storing " << goodCandidates.size() << " good tracks " << endmsg;
  //=== Store tracks...
  for (const auto&  cand : goodCandidates ) {
    output.push_back( seed.clone() );
    LHCb::Track* fwTra = output.back();
    //fwTra->clearAncestors();
    fwTra->addToAncestors( seed );  // Set the Velo track as only ancestor of the Forward track
    fwTra->setType( LHCb::Track::Long );
    fwTra->setHistory( LHCb::Track::PatForward );

    //== Add a new state in the T stations ...

    double qOverP = m_fwdTool->qOverP( cand );
    // set q/p in all of the existing states
    std::for_each( std::begin( fwTra->states() ), std::end( fwTra->states() ),
                   [qOverP]( LHCb::State* state ) {
      state->setQOverP(qOverP);
      state->setErrQOverP2(qOverP*qOverP*0.012*0.012);
    } );

    Gaudi::TrackSymMatrix cov;
    cov(0,0) = m_stateErrorX2;
    cov(1,1) = m_stateErrorY2;
    cov(2,2) = m_stateErrorTX2;
    cov(3,3) = m_stateErrorTY2;
    double errQOverP = m_stateErrorP*qOverP;
    cov(4,4) = errQOverP * errQOverP;


    for ( const auto& z : m_fwdTool->zOutputs() ) {
      double dz = z - m_fwdTool->zReference();
      LHCb::State temp;
      temp.setLocation( LHCb::State::AtT );
      temp.setState( cand.x( dz ), cand.y( dz ), z,
		             cand.xSlope( dz ), cand.ySlope( dz ), qOverP );
      //== overestimated covariance matrix, as input to the Kalman fit
      temp.setCovariance( cov );
      fwTra->addToStates( std::move(temp) );
    }
    //== New information, on the track fit.
    fwTra->setChi2PerDoF( cand.chi2PerDoF() );
    fwTra->setNDoF(       cand.nDoF() );
    fwTra->addInfo(LHCb::Track::PatQuality, cand.quality());
    // added for NNTools
    fwTra->addInfo(LHCb::Track::Cand1stQPat, cand.cand1stquality());
    fwTra->addInfo(LHCb::Track::Cand2ndQPat, cand.cand2ndquality());

    //== Add reference to the used clusters/, T stations
    std::for_each( std::begin(cand), std::end(cand),
                   [&fwTra](PatForwardHit *myHit) {
           fwTra->addToLhcbIDs( myHit->hit()->lhcbID() );
           myHit->hit()->setStatus(Tf::HitBase::UsedByPatForward);
           myHit->setIsUsed(true);
    } );
    fwTra -> setPatRecStatus( LHCb::Track::PatRecIDs );

    if ( m_addTTClusterTool ) {
      StatusCode sc = m_addTTClusterTool->addTTClusters( *fwTra );
      if (sc.isFailure())
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug()<<" Failure in adding TT clusters to track"<<endmsg;
    }
  }
  // mark the input tracks and all ancestors as used
  if(m_FlagUsedSeeds && !output.empty()){
    const_cast<LHCb::Track&>(seed).setFlag(LHCb::Track::Used,true);
    std::for_each( std::begin(seed.ancestors()), std::end(seed.ancestors()),
                   [](const LHCb::Track* anc) { const_cast<LHCb::Track*>(anc)->setFlag(LHCb::Track::Used,true); } );
  }

  counter("#FinishedTracks") += output.size();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Finished track" << endmsg;
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Fill the vector of hit pointer, sorted by projection.
//=========================================================================

std::pair<PatFwdHits::const_iterator,PatFwdHits::const_iterator>
PatForwardTool::fillXList ( PatFwdTrackCandidate& track ) const
{
  m_xHitsAtReference.clear();
  auto interval = make_XInterval(track);
  if ( UNLIKELY( msgLevel( MSG::DEBUG ) ) ) {
    debug() << "Search X coordinates, xMin " << interval.xMin()
            << " xMax " << interval.xMax() << endmsg;
  }
  double yCompat = m_yCompatibleTol + 50 * fabs(track.slY());

  //TODO: use C++14 generalized capture...
  double ty = track.slY();
  double y0 = track.yStraight( 0. );
  auto updateHitsForTrack = [&](PatFwdHits::iterator first, PatFwdHits::iterator last) {
    std::for_each( first, last, [=](PatForwardHit* hit) { 
                       updateHitForTrack( hit, y0, ty);
                       hit->setIgnored( false );
                       hit->setRlAmb( 0 );
                       hit->setSelected( this->driftOK(*hit) );
                   } );
  };

  for (unsigned int sta = 0; sta < nSta; sta ++) {
    for (unsigned int lay = 0; lay< nLay; lay++) {
      if (lay == 1 || lay == 2) continue;  // X layers
      for (unsigned int region = 0; region <nReg; region ++) {
        const Tf::EnvelopeBase* regionB = m_tHitManager->region(sta,lay,region);
        auto z = regionB->z();
        double yRegion = track.yStraight( z );
        if (!regionB->isYCompatible( yRegion, yCompat )) continue;

        auto xRange = symmetricRange( track.xStraight(z),
                                      interval.xKick( z ) + fabs( yRegion * regionB->sinT() ) + 20. );

        auto hitRange =  m_tHitManager->hitsWithMinX(xRange.min(), sta, lay, region);
        auto last = std::upper_bound( std::begin(hitRange), std::end(hitRange), xRange.max(), compByX );

        // grow capacity so that things don't move around afterwards...
        m_xHitsAtReference.reserve( m_xHitsAtReference.size() + std::distance( std::begin(hitRange), last) );
        auto current = std::end(m_xHitsAtReference);

        std::copy_if( std::begin(hitRange), last, 
                      std::back_inserter(m_xHitsAtReference), 
                      [=](PatForwardHit* hit){
                              return !hit->hit()->ignore() &&  
                                     hit->hit()->isYCompatible( yRegion, yCompat );
                      } );
        updateHitsForTrack( current, std::end(m_xHitsAtReference) );
        m_xHitsAtReference.erase( std::remove_if( current, std::end(m_xHitsAtReference), 
                                      [](const PatForwardHit* hit) {
                                           return !hit->isSelected();
                                      } ),
                                  std::end(m_xHitsAtReference) );
        m_fwdTool->setXAtReferencePlane(track, current, std::end(m_xHitsAtReference) );
        m_xHitsAtReference.erase( std::remove_if( current, std::end(m_xHitsAtReference), 
                                      [&](const PatForwardHit* hit) {
                                           return interval.outside(hit->projection());
                                      } ),
                                  std::end(m_xHitsAtReference) );
        // make sure we are ordered by the right criterium -- until now, things
        // are ordered by xAtYEq0, which isn't quite the same as by xAtReferencePlane...
        // so this sort is actually needed (not always though...), for both IT and OT...
        std::sort( current, std::end(m_xHitsAtReference), Tf::increasingByProjection<PatForwardHit>() );
        std::inplace_merge(std::begin(m_xHitsAtReference),current,std::end(m_xHitsAtReference), 
                           Tf::increasingByProjection<PatForwardHit>() );
      }
    }
  }
  return { std::begin(m_xHitsAtReference), std::end(m_xHitsAtReference) };
}

//=========================================================================
//  Fill the vector of hit pointer, sorted by projection.
//=========================================================================
bool PatForwardTool::fillStereoList ( PatFwdTrackCandidate& track, double tol ) const {
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Adding stereo coordinates, tol = " << tol << endmsg;

  PatFwdHits temp; // no need actually for PatFwdHits -- a container with objects which have 'projection', 'planeCode','selected' (and pointer to hit) suffices

  //== get position and slope at z=0 from track at zReference (0 for y/ySlope functions)
  auto ty = track.ySlope( 0. );
  auto y0 = track.y( 0. - m_fwdTool->zReference() );  // Extrapolate from back...
  auto update = [=](PatFwdHit* hit, double yRegion) {
    if (hit->hit()->ignore() || !hit->hit()->isYCompatible( yRegion, m_yCompatibleTol ) ) return false;;
    updateHitForTrack( hit, y0, ty );
    hit->setIgnored( false );
    auto ok = this->driftOK(*hit);
    hit->setSelected( ok );
    return ok;
  };

  for (unsigned int sta=0; sta<nSta; ++sta) {
    for (unsigned int lay=1; lay<nLay-1; ++lay) {
      for (unsigned int region=0; region<nReg; ++region) {

        const Tf::EnvelopeBase* regionB = m_tHitManager->region(sta,lay,region);
        double dz = regionB->z() - m_fwdTool->zReference();
        double yRegion = track.y( dz );
        if (!regionB->isYCompatible( yRegion, m_yCompatibleTol ) ) continue;

        double xPred   = track.x( dz );
        //== Correct for stereo
        double xHitMin = xPred - fabs( yRegion * regionB->sinT() ) - 40. - tol;

        //== Project in Y, in fact in X but oriented, such that a displacement in Y is a
        //== displacement also in this projectio.
        bool  flipSign = std::signbit( regionB->sinT() );
        double minProj = tol;
        if ( region< nOTReg ) minProj += 1.5;

        for ( auto* hit : m_tHitManager->hitsWithMinX(xHitMin, sta, lay, region) ) {
          if (!update(hit,yRegion)) continue;

          double xRef = ( hit->x() - xPred );
          hit->setProjection( flipSign ? -xRef : xRef );

          if (  xRef > minProj ) break;
          if (  xRef >= -minProj ) temp.push_back( hit );
        }
      }
    }
  }

  //== Sort by projection
  std::sort( std::begin(temp), std::end( temp), Tf::increasingByProjection<PatForwardHit>() );

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << "List size = " << temp.size() << endmsg;
    for ( const PatFwdHit *hit : temp ) {
      debug() << format( " Selected:  Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d typ%2d Prev%2d Next%2d",
                         hit->z(),
                         hit->projection(),
                         hit->x(),
                         hit->hit()->station(),
                         hit->hit()->layer(),
                         hit->hit()->region(),
                         hit->hasPrevious(),
                         hit->hasNext() ) << endmsg;
    }
  }

  //== Select a coherent group
  auto bestList = std::make_pair(temp.end(),temp.end());
  int inbDifferent = 0;
  double size = 1000.;
  int minYPlanes = 4;

  auto itP = std::begin(temp);
  auto sentinel = make_RangeFinder(minYPlanes, std::end(temp));
  auto make_predicate = [=]( const PatFwdHit* hit ) -> MaxSpread { return { this->allowedStereoSpread(hit) }; };

  while ( sentinel( itP ) ) {
    auto range = sentinel.next_range( itP, make_predicate(*itP) );
    if ( range.first != range.second ) {

        if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
          verbose() << format( "Found Y group from %9.2f to %9.2f with %2d entries and %2d planes, spread %9.2f",
                               (*range.first)->projection(), (*std::prev(range.second))->projection(),
                               std::distance(range.first,range.second), nbDifferent(range.first,range.second),
                               allowedStereoSpread(*itP) )
                    << endmsg;
        }
        //== We have the first list. The best one ????
        auto cnt = nbDifferent( range.first, range.second );
        if ( cnt >= inbDifferent ) {
          auto delta = (*std::prev(range.second))->projection() - (*range.first)->projection();
          if ( cnt > inbDifferent || delta < size ) {
            inbDifferent =  cnt;
            size = delta;
            bestList = range;
            //break; /// Keep first one !
          }
        }
    }
    itP = range.second;
  }

  if ( std::distance(bestList.first,bestList.second) < minYPlanes ) return false;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "...Selected " << std::distance(bestList.first,bestList.second) << " hits from "
            << (*bestList.first)->projection()
            << " to " << (*(bestList.second-1))->projection() << endmsg;

  track.addCoords( bestList.first, bestList.second );

  //== Sort by Z
  std::sort( std::begin(track), std::end(track), Tf::increasingByZ<PatForwardHit>() );
  return true;
}

//=========================================================================
//  Debug one vector of of Hits
//=========================================================================
void PatForwardTool::debugFwdHits ( const PatFwdTrackCandidate& track ) const {
  debugFwdHits( track, debug() );
}

void PatForwardTool::debugFwdHits ( const PatFwdTrackCandidate& track, MsgStream& msg ) const {
  for ( const auto *hit : track ) {
    msg << format( " Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d typ%2d Prev%2d Next%2d Drift %7.3f",
                   hit->z(),
                   hit->projection(),
                   hit->x(),
                   hit->hit()->station(),
                   hit->hit()->layer(),
                   hit->hit()->region(),
                   hit->hasPrevious(),
                   hit->hasNext(),
                   hit->driftDistance() );
    if ( track.fitted() ) msg << format( " chi2%7.2f", m_fwdTool->chi2Hit( track, hit) );
    /* for ( const auto& tM : hit->MCKeys() ) {
       msg << " " << tM;
       if ( tM == m_MCKey ) msg << " <=*** ";
       }
    */
    msg << endmsg;
  }
}

//=========================================================================
//  Build the list of vector of X hit candidates.
//=========================================================================
void PatForwardTool::buildXCandidatesList ( PatFwdTrackCandidate& track ) const{

  m_candidates.clear();

  auto range = fillXList( track ) ;

  double xExtrap = track.xStraight( m_fwdTool->zReference() );

  auto itP = range.first;
  auto sentinel = make_RangeFinder( m_minXPlanes, range.second );
  auto make_predicate = [=]( const PatFwdHit* hit ) -> MaxSpread { return { allowedXSpread(hit,xExtrap) }; };
  while ( sentinel( itP+1 ) ) { //TODO: why the +1 here??? (other than historical reasons)
    auto  predicate = make_predicate(*itP) ;
    auto range = sentinel.next_range( itP, predicate);
    if (range.first != range.second) {
        if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
          verbose() << format( "Found X group from %9.2f to %9.2f with %2d entries and %2d planes, spread %9.2f",
                               (*range.first)->projection(),(*std::prev(range.second))->projection(),
                               std::distance(range.second,range.first), nbDifferent(range.first,range.second),
                               predicate.spread)
                    << endmsg;
        }
        //== Protect against too dirty area.
        if ( std::distance(range.first,range.second ) <  m_maxXCandidateSize ) {
          //== Merge the lists, if the first new point is close to the last one...
          if (!m_candidates.empty() && predicate(m_candidates.back().coords().back(), *range.first) ) {
            m_candidates.back().addCoords( range.first, range.second );
          } else {
            m_candidates.emplace_back( track.track(), range.first, range.second );
          }
        }
    }
    itP = range.second;
  }
}

//=============================================================================
