// Include files
#include <cmath>
#include <algorithm>

// from Gaudi
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

//-----------------------------------------------------------------------------
// Implementation file for class : PatForwardTool
//
// 2005-10-04 : Olivier Callot
//-----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY( PatForwardTool )

// For convenient trailing-return-types in C++11:
#define AUTO_RETURN(...) noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) {return (__VA_ARGS__);}

constexpr struct proj_distance_t {
  template <typename Range>
  auto operator()(const Range& range) const //TODO: use C++14 auto return deduction instead
      AUTO_RETURN( range.back()->projection() - range.front()->projection() )

      template <typename Iter>
      auto operator()(Iter&& f, Iter&& l) const //TODO: use C++14 auto return deduction instead
      AUTO_RETURN( (*std::prev(std::forward<Iter>(l)))->projection() - (*std::forward<Iter>(f))->projection() )
} proj_distance {};

constexpr struct sortQuality_t {
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
} sortQuality{};

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
template <typename Container>
int nbDifferent( const Container& c) {
  return nbDifferent( std::begin(c), std::end(c) );
}

template <typename Iter>
class RangeGenerator {
  Iter m_last;
  int m_minPlanes;
  Iter next(Iter current) const { return std::next( std::move(current), m_minPlanes-1 ); }
public:
  RangeGenerator( int minPlanes, Iter last ) :  m_last{ std::move(last) }, m_minPlanes{minPlanes} { }
  template <typename Iterator> bool operator()(Iterator&& first, Iterator&& last) const { return std::distance(std::forward<Iterator>(first),std::forward<Iterator>(last)) > m_minPlanes-1; }
  template <typename Iterator> bool operator()(Iterator&& first ) const { return std::distance(std::forward<Iterator>(first),m_last) > m_minPlanes-1; }

  template <typename Iterator, typename Predicate>
  boost::iterator_range<Iterator> next_range(Iterator first, Predicate predicate)
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
    while ( std::distance(i,mid) != (m_minPlanes-1) && j != m_last ) {
      counter.removeHit(*i);
      ++i;
      j = counter.addHitsWhile( j, m_last, [&](reference k) { return predicate(*i,k); } );
      if ( enough_planes() ) mid = j;
    }
    return { first, mid };
  }
};

template <typename Iterator>
RangeGenerator<typename std::decay<Iterator>::type>
make_RangeGenerator( int nPlanes, Iterator&& last)
{
  return { nPlanes, std::forward<Iterator>(last) };
}

template <typename Range>
struct  BestRangeSelector {
  std::tuple<Range,double,int> operator()(std::tuple<Range,double,int> prev, Range&& r) {
    if (!r.empty()) {
      auto cnt = nbDifferent(r);
      if ( cnt >= std::get<2>(prev) ) {
        auto delta = proj_distance(r);
        if ( cnt > std::get<2>(prev) || delta < std::get<1>(prev) ) {
          return { std::forward<Range>(r), delta, cnt };
        }
      }
    }
    return prev;
  }
};


template <typename Range>
class  BestRangeFinder {
  Range m_range;
  double m_size;
  int m_nbDifferent;
public:
  BestRangeFinder(Range&& r, int n, double s) :
    m_range( std::forward<Range>(r) ),
    m_size(s),
    m_nbDifferent(n) {}

  const Range& range() const { return m_range; }

  void update(const Range& r) {
    if (r.empty()) return;
    auto cnt = nbDifferent(r);
    if ( cnt >= m_nbDifferent ) {
      auto delta = proj_distance(r);
      if ( cnt > m_nbDifferent || delta < m_size ) {
        m_nbDifferent = cnt;
        m_size = delta;
        m_range = r;
      }
    }
  }
};

template <typename Range>
BestRangeFinder<Range> make_BestRangeFinder(Range&& r, int n, double s) {
  return { std::forward<Range>(r), n, s};
}

constexpr unsigned int nReg() { return  Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kNRegions; }

template <bool stereo>
struct regions {
  class iterator {
    friend class regions;
    unsigned m_index;
    iterator( unsigned i ) : m_index{i} {}
  public:
    const iterator& operator*() const { return *this; }
    iterator& operator++() { ++m_index; return *this; }

    unsigned region() const { return m_index%nReg(); }
    static_assert(Tf::RegionID::ITIndex::kNLayers == 4, "expecting exactly four IT layers" );
    static_assert(Tf::RegionID::OTIndex::kNLayers == 4, "expecting exactly four OT layers" );
    unsigned layer() const { return stereo ? ( 1+ (m_index/nReg())%2  )  // {1,2} -- stereo only...
                                           : ( 3*((m_index/nReg())%2) ); // X layers only: {0,3} // base 0, stride 3, extent between 3 and 5
                           }
    unsigned station() const { return m_index/(2*nReg()); }
    bool isOT() const { return region() < Tf::RegionID::OTIndex::kNRegions; }

    friend bool operator!=(const iterator& lhs, const iterator& rhs) {
      return lhs.m_index != rhs.m_index;
    }
  };
  constexpr unsigned size()  const { return Tf::RegionID::OTIndex::kNStations*Tf::RegionID::OTIndex::kNLayers*nReg()/2; }
  constexpr iterator begin() const { return { 0 }; }
  constexpr iterator end()   const { return { size() }; }
};

static const auto stereo_regions = regions<true>{};
static const auto      x_regions = regions<false>{};

static const  auto not_selected = [](const PatForwardHit* hit) { return !hit->isSelected(); };

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatForwardTool::PatForwardTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : base_class( type, name , parent )
  , m_trackSelector(nullptr)

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

  declareProperty("AddTTClusterName", m_addTtToolName = "PatAddTTCoord" );

  declareProperty( "WithoutBField"         , m_withoutBField         = false);

  declareProperty("Preselection" , m_Preselection = false);
  declareProperty("PreselectionPT",m_PreselectionPT = 400.*Gaudi::Units::MeV);
  declareProperty("UseWrongSignWindow",m_UseWrongSignWindow = false);
  declareProperty("WrongSignPT",m_WrongSignPT = 2000.*Gaudi::Units::MeV);
  //declareProperty("FlagUsedSeeds",m_FlagUsedSeeds = false);

  //declareProperty("SkipUsedSeeds",m_skipUsedSeeds = false);
  //declareProperty("SkipUsedHits",m_skipUsedHits = false);
  declareProperty( "VeloVetoTracksNames", m_veloVetoTracksNames = {});
  declareProperty( "UsedLHCbIDToolName",m_LHCbIDToolName = "");
  //declareProperty( "UnusedVeloSeeds", m_unusedVeloSeeds = false);
  declareProperty( "TrackSelectorName",  m_trackSelectorName   = "None");

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

  m_trackSelector = (m_trackSelectorName != "None") ? tool<ITrackSelector>(m_trackSelectorName, this)
                                                    : nullptr;

  incSvc()->addListener(this, IncidentType::BeginEvent);
  m_newEvent = true;

  if ( "" != m_addTtToolName  ) {
    m_addTTClusterTool = tool<IAddTTClusterTool>( m_addTtToolName, this );
  } else {
    m_addTTClusterTool = nullptr;
  }

  if (!m_LHCbIDToolName.empty()){
    m_skipUsedHits = true;
    m_usedLHCbIDTool =  tool<IUsedLHCbID>(m_LHCbIDToolName, this);
  }else{
    m_skipUsedHits = false;
    m_usedLHCbIDTool = nullptr;
  }

  m_skipUsedSeeds = !m_veloVetoTracksNames.empty();

  return StatusCode::SUCCESS;
}

//=============================================================================
// -- Check if new event has occurred. If yes, set flag
// -- Note: The actions of initEvent cannot be executed here,
// -- as this handle runs before decoding the clusters
//=============================================================================
void PatForwardTool::handle ( const Incident& incident ) {

  if ( IncidentType::BeginEvent == incident.type() ) m_newEvent = true;

}

//=========================================================================
//  Prepare hits
//=========================================================================
void PatForwardTool::prepareHits()
{
  m_tHitManager->prepareHits();
  //== if any hits to be flagged as used, do so now...
  if ( m_usedLHCbIDTool ) {
    auto hits =   m_tHitManager->hits();
    std::for_each( std::begin(hits), std::end(hits), [&](PatFwdHit* hit) {
      //make sure we set all hits to false
      hit->setIsUsed(false);
      if (m_usedLHCbIDTool->used(hit->hit()->lhcbID())) hit->setIsUsed(true);
    } );
  }
  m_newEvent = false;
}


//=========================================================================
//  Check if a track should be processed. Default flags, and selector if defined
//=========================================================================
bool PatForwardTool::acceptTrack(const LHCb::Track& track) const
{
  if ( track.checkFlag( LHCb::Track::Invalid) ) return false;
  if ( track.checkFlag( LHCb::Track::Backward) ) return false;
  if ( m_trackSelector &&  !m_trackSelector->accept(track)) return false;
  // S.Stahl: Move this external. The track selector above could do the job.
  if ( m_skipUsedSeeds ) {
    for (auto itNames : m_veloVetoTracksNames ){
      if( !exist<LHCb::Track::Range>(itNames) ) {
        // Continue here in case the location does not exist.
        if( UNLIKELY( msgLevel(MSG::ERROR) ) ) error()<<"Location "<<itNames<<" does not exist!"<<endmsg;
        continue;
      }
      LHCb::Track::Range vetoTracks = get<LHCb::Track::Range>( itNames );
      for (auto it : vetoTracks ) {
        if (it == &track ) return false;
        auto ancestors = it->ancestors();
        if (!std::none_of( std::begin( ancestors ), std::end( ancestors ),
                           [&](const LHCb::Track* t) { return t == &track; } )){
          counter("#SkippedUsedSeeds") += 1;
          return false;
        }
      }
    }
  }
  return true;
}


//=========================================================================
//  Main execution: Extend a track.
//=========================================================================
void PatForwardTool::forwardTrack( const LHCb::Track* tr, LHCb::Tracks* output ) {

  std::vector<LHCb::Track*> outvec;
  tracksFromTrack(*tr,outvec).ignore();

  output->reserve(output->size()+outvec.size());
  for ( const auto& i : outvec ) output->insert(i);
}

StatusCode PatForwardTool::tracksFromTrack( const LHCb::Track& seed,
                                            std::vector<LHCb::Track*>& output ){
  if ( !acceptTrack(seed) ) return StatusCode::SUCCESS;

  counter("#Seeds") += 1;

  PatFwdTrackCandidate track( &seed );
  // S.Stahl: Move this external. The track selector can do the job.
  if(m_Preselection && seed.pt() < m_PreselectionPT && 0 != track.qOverP()) return StatusCode::SUCCESS;

  bool isDebug = msgLevel( MSG::DEBUG );
  if ( UNLIKELY(isDebug) ) {
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
  if(m_newEvent) prepareHits();
  //m_tHitManager->prepareHits();
  boost::iterator_range<typename PatFwdHits::const_iterator> rng = fillXList( track ) ;

  bool processingSecondLoop = false;
  bool processedSecondLoop = false;

  while (!processedSecondLoop) {

    if(processingSecondLoop){
      auto ty = track.slY();
      auto y0 = track.yStraight( 0. );
      for(auto it = m_xHitsAtReference.begin(); it != m_xHitsAtReference.end(); ++it){
        auto hit = *it;
        if(hit->isOT()){
          approxUpdateOTHitForTrack( hit, y0, ty);
          hit->setIgnored( false );
          hit->setRlAmb( 0 );
          hit->setSelected( this->driftInRange(*hit) );
          if(not_selected(hit))m_xHitsAtReference.erase(it);
        }
        else{
          updateNonOTHitForTrack( hit, y0, ty);
          hit->setIgnored( false );
          hit->setRlAmb( 0 );
          hit->setSelected( true );
        }
      }
    }

    //== Build the initial list of X candidates
    buildXCandidatesList( track, rng );

    int minPlanes = m_minPlanes;  //== Initial value, can be updated later...

    std::vector<PatFwdTrackCandidate> xCandidates; xCandidates.reserve( m_candidates.size() );

    int minOTX = int( 1.5 * m_minXPlanes );
    auto accept = [=](const PatFwdTrackCandidate& cnd) {
      //== Check the chi2 with magnet center constraint.
      //== Count how many weighted hits
      //== reject if below threshold
      return cnd.chi2PerDoF() < m_maxChi2Track &&
          ( !m_withoutBField || fabs(cnd.slX()-cnd.xSlope(0.))<0.005) &&
          ( nT(cnd) >= minOTX || inCenter(cnd) ) &&
          passMomentum( cnd, track.sinTrack() );
    };

    for( auto& icand : m_candidates ) {
      //== Fit the candidate, and store them (fitXCandidate also updates the hits
      while ( m_fwdTool->fitXCandidate( icand, m_maxChi2, m_minXPlanes ) ) {

        // Copy the whole stuff, keep only used ones and store it.
        PatFwdTrackCandidate temp = icand;
        temp.cleanCoords();
        m_fwdTool->chi2PerDoF( temp );  // Compute and store chi2/dof
        if ( UNLIKELY(isDebug) ) {
          debug() << "Chi2/nDof = " << temp.chi2PerDoF() << " nDoF " << temp.nDoF()
                  << " dist at center " << m_fwdTool->distAtMagnetCenter( temp )
                  << endmsg;
          debugFwdHits( temp );
        }
        if ( accept(temp) ) xCandidates.push_back( std::move(temp) );

        //== tag these coordinates in the original, so that we don't find the same track again
        std::for_each( std::begin(icand), std::end(icand), [](PatFwdHit* hit) {
          if ( hit->isSelected() ) hit->setIgnored( true );
        } );
      }
    }

    if ( UNLIKELY(isDebug) ) {
      debug() << "************ List of X candidates , N = " << xCandidates.size() << endmsg;
      unsigned nCand=0;
      for ( const auto& itL : xCandidates ) {
        debug() << "Candidate " << nCand++
                << " Chi2/nDof = " << itL.chi2PerDoF() << endmsg;
        debugFwdHits( itL );
      }
      if ( !xCandidates.empty() ) debug() << "---- Now get the stereo hits on these ---" << endmsg;
      nCand=0;
      for ( PatFwdTrackCandidate& temp :  xCandidates ) {
        debug() << "--- Candidate " << ++nCand
                << "  X cord size " << temp.coords().size()
                << endmsg;
      }
    }

    //== Now try to get space track from these X track.
    std::vector<PatFwdTrackCandidate> goodCandidates;
    int maxPlanes = 0;

    for ( PatFwdTrackCandidate& temp :  xCandidates ) { //TODO: make this one big 'remove_if'?

      std::for_each( std::begin( temp.coords() ), std::end( temp.coords() ),
                     [](PatForwardHit* h) { h->setIgnored(false); } );// restore normal flag.
      temp.setSelectedCoords( );

      auto qOverP = computeQOverP(temp);  // in 1/GeV
      if ( !fillStereoList( temp, computeStereoTol(qOverP) ) ) continue; // Get the stereo coordinates

      if ( UNLIKELY(isDebug) )  debugFwdHits( temp );
      temp.setSelectedCoords( );

      //== Fit and reject if not good enough
      if ( !m_fwdTool->fitStereoCandidate( temp, m_maxChi2, minPlanes ) ) continue;

      temp.cleanCoords();
      m_fwdTool->chi2PerDoF( temp );  //== compute and store Chi2PerDoF
      if ( UNLIKELY(isDebug) ) {
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
        if( UNLIKELY( isDebug ) ) debug() << "Not enough Y planes : " << nbY << endmsg;
        continue;
      }

      if ( m_maxDeltaY + m_maxDeltaYSlope * qOverP *qOverP < fabs(  m_fwdTool->changeInY( temp ) ))  {
        if( UNLIKELY( isDebug ) ) debug() << "  --- Too big change in Y : " <<  m_fwdTool->changeInY( temp ) << endmsg;
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
      std::stable_sort( std::begin(goodCandidates), std::end(goodCandidates), sortQuality);
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
      auto bestQuality = std::accumulate( std::begin(goodCandidates), last, 1000.,
                                          [](double m, const PatFwdTrackCandidate& t) { return std::min(m,t.quality()); } );
      bestQuality += 1.0;
      last = std::remove_if( std::begin(goodCandidates), last,
                             [=](const PatFwdTrackCandidate& t) { return t.quality()  >= bestQuality; }  );
      // remove if sensibly less OT
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

    if( UNLIKELY( isDebug ) )
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
      auto qOverP = m_fwdTool->qOverP( cand );
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
      auto errQOverP = m_stateErrorP*qOverP;
      cov(4,4) = errQOverP * errQOverP;

      for ( const auto& z : m_fwdTool->zOutputs() ) {
        auto  dz = z - m_fwdTool->zReference();
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
        //S.Stahl: This flag was nowhere used. Now used for hit flagging.
        //myHit->setIsUsed(true);
      } );
      fwTra -> setPatRecStatus( LHCb::Track::PatRecIDs );
      ///== Add TT hits. Do not do this if the seed is an Upstream track.
      if ( m_addTTClusterTool && !seed.checkType(LHCb::Track::Upstream) ) {
        StatusCode sc = m_addTTClusterTool->addTTClusters( *fwTra );
        if ( UNLIKELY(sc.isFailure()) && UNLIKELY( isDebug ) )
          debug()<<" Failure in adding TT clusters to track"<<endmsg;
      }
    }
    // S.Stahl: At this point you don't know if you actually want to flag the seed.
    // mark the input tracks and all ancestors as used
    //if(m_FlagUsedSeeds && !output.empty()){
    //  const_cast<LHCb::Track&>(seed).setFlag(LHCb::Track::Used,true);
    // std::for_each( std::begin(seed.ancestors()), std::end(seed.ancestors()),
    //                 [](const LHCb::Track* anc) { const_cast<LHCb::Track*>(anc)->setFlag(LHCb::Track::Used,true); } );
    //}

    if(processingSecondLoop){
      m_minXPlanes += 1;
      m_minPlanes += 1;
      processedSecondLoop = true;
    }
    if(output.empty() && m_secondLoop && !processingSecondLoop){
      m_minXPlanes -= 1;
      m_minPlanes -= 1;
      processingSecondLoop = true;
    }
    else break;
  }

  counter("#FinishedTracks") += output.size();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Finished track" << endmsg;
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Fill the vector of hit pointer, sorted by projection.
//=========================================================================

boost::iterator_range<typename PatFwdHits::const_iterator>
PatForwardTool::fillXList ( PatFwdTrackCandidate& track ) const
{
  m_xHitsAtReference.clear();
  auto interval = make_XInterval(track);
  if ( UNLIKELY( msgLevel( MSG::DEBUG ) ) ) {
    debug() << "Search X coordinates, xMin " << interval.xMin()
            << " xMax " << interval.xMax() << endmsg;
  }

  auto yCompat = m_yCompatibleTol + 50 * fabs(track.slY());
  auto not_ignored_and_y_compatible = [yCompat,this](double yRegion) {
    return [yRegion,yCompat,this](const PatForwardHit* hit) {
      return !hit->hit()->ignore() && !(m_skipUsedHits && hit->isUsed()) &&
          hit->hit()->isYCompatible( yRegion, yCompat );
    };
  };

  //TODO: use C++14 generalized capture...
  auto ty = track.slY();
  auto y0 = track.yStraight( 0. );
  auto updateOTHitsForTrack = [&](PatFwdHits::iterator first, PatFwdHits::iterator last) {
    std::for_each( first, last, [=](PatForwardHit* hit) {
      approxUpdateOTHitForTrack( hit, y0, ty);
      hit->setIgnored( false );
      hit->setRlAmb( 0 );
      hit->setSelected( this->driftInRange(*hit) );
    } );
    return std::remove_if( first, last, not_selected );
  };
  auto updateITHitsForTrack = [&](PatFwdHits::iterator first, PatFwdHits::iterator last) {
    std::for_each( first, last, [=](PatForwardHit* hit) {
      updateNonOTHitForTrack( hit, y0, ty);
      hit->setIgnored( false );
      hit->setRlAmb( 0 );
      hit->setSelected( true );
    } );
    return last;
  };

  for ( const auto& region : x_regions ) {
    const auto* regionB = m_tHitManager->region(region.station(),region.layer(),region.region());
    auto z = regionB->z();
    auto yRegion = track.yStraight( z );
    if (!regionB->isYCompatible( yRegion, yCompat )) continue;
    // TODO: yRegion * regionB->sinT() should contribute to the central value, not the width...
    auto tol = ( fabs( yRegion * regionB->sinT() ) + 20.);
    auto hitRange = m_tHitManager->hitsInXRange(interval.xMinAtZ(z) - tol,  interval.xMaxAtZ(z) + tol,
                                                region.station(), region.layer(), region.region());

    // break loop here, store (hitRange,yRegion,region.isOT()) ???

    // grow capacity so that things don't move around afterwards...
    m_xHitsAtReference.reserve( m_xHitsAtReference.size() + hitRange.size() );
    auto first = std::end(m_xHitsAtReference);
    std::copy_if( std::begin(hitRange), std::end(hitRange),
                  std::back_inserter(m_xHitsAtReference),
                  not_ignored_and_y_compatible( yRegion ) );
    auto last = region.isOT() ? updateOTHitsForTrack( first, std::end(m_xHitsAtReference) )
                              : updateITHitsForTrack( first, std::end(m_xHitsAtReference) );
    m_xHitsAtReference.erase( last, std::end(m_xHitsAtReference) );
    m_fwdTool->setXAtReferencePlane( track, first, last );
    // make sure we are ordered by the right criterium -- until now, things
    // are ordered by xAtYEq0, which isn't quite the same as by xAtReferencePlane...
    // so this sort is actually needed (not always though...), for both IT and OT...
    std::sort( first, last, Tf::increasingByProjection<PatForwardHit>() );
    std::inplace_merge( std::begin(m_xHitsAtReference),first,last,
                        Tf::increasingByProjection<PatForwardHit>() );
  }
  // select the subrange of m_xHitsAtReference which is within the interval...
  return interval.inside( boost::make_iterator_range( std::begin(m_xHitsAtReference), std::end(m_xHitsAtReference) ),
                          [](const PatForwardHit* h) { return h->projection(); } );
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
  auto updateITHit = [=](PatFwdHit* hit ) {
    updateNonOTHitForTrack( hit, y0, ty );
    hit->setSelected( true );
    return true;
  };
  auto updateOTHit = [=](PatFwdHit* hit ) {
    approxUpdateOTHitForTrack( hit, y0, ty );
    auto ok = this->driftInRange(*hit);
    hit->setSelected( ok );
    return ok;
  };

  for ( const auto& region : stereo_regions ) {
    const auto* regionB = m_tHitManager->region(region.station(),region.layer(),region.region());
    auto dz = regionB->z() - m_fwdTool->zReference();
    auto yRegion = track.y( dz );
    if (!regionB->isYCompatible( yRegion, m_yCompatibleTol ) ) continue;

    auto xPred   = track.x( dz );
    //== Correct for stereo -- TODO/FIXME: this is very loose if the stereo angle * yRegion is positive...
    auto xHitMin = xPred - ( fabs( yRegion * regionB->sinT() ) + 40. + tol );

    auto ot = region.isOT();
    //== Project in Y, in fact in X but oriented, such that a displacement in Y is a
    //== displacement also in this projectio.
    bool flipSign = std::signbit( regionB->sinT() );
    auto minProj = tol + int(ot) * 1.5;

    for ( auto* hit : m_tHitManager->hitsWithMinX(xHitMin, region.station(), region.layer(), region.region()) ) {
      if (hit->hit()->ignore()
          || (m_skipUsedHits && hit->isUsed())
          || !hit->hit()->isYCompatible( yRegion, m_yCompatibleTol ) ) continue;
      hit->setIgnored( false );
      bool ok = ot ? updateOTHit(hit) : updateITHit(hit);
      if (!ok) continue;

      auto xRef = hit->x() - xPred;
      hit->setProjection( flipSign ? -xRef : xRef );

      if ( xRef > minProj ) break;
      if ( xRef >= -minProj ) temp.push_back( hit );
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

  auto bestRangeFinder = make_BestRangeFinder(boost::make_iterator_range(std::end(temp),std::end(temp)), 0, 1000. );

  int minYPlanes = 4;

  auto make_predicate = [=]( const PatFwdHit* hit ) -> MaxSpread { return { this->allowedStereoSpread(hit) }; };
  auto iter = std::begin(temp);
  auto sentinel = make_RangeGenerator(minYPlanes, std::end(temp));

  while ( sentinel( iter ) ) {
    auto range = sentinel.next_range( iter, make_predicate(*iter) );
    iter = std::end(range);
    bestRangeFinder.update( std::move(range) );
  }
  auto bestList = bestRangeFinder.range();

  // assert( bestList.size() >= minYPlanes );
  if ( bestList.size() < minYPlanes ) {
    // error() << " this should never happen! " << endmsg;
    return false;
  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "...Selected " << bestList.size() << " hits from "
            << bestList.front()->projection()
            << " to " << bestList.back()->projection() << endmsg;

  track.addCoords( std::begin(bestList), std::end(bestList) );

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
void PatForwardTool::buildXCandidatesList ( PatFwdTrackCandidate& track,
                                            boost::iterator_range<typename PatFwdHits::const_iterator> rng ) const{

  m_candidates.clear();

  auto iter = std::begin(rng);
  auto sentinel = make_RangeGenerator( m_minXPlanes, std::end(rng) );
  auto xExtrap = track.xStraight( m_fwdTool->zReference() );
  auto make_predicate = [=]( const PatFwdHit* hit ) -> MaxSpread { return { allowedXSpread(hit,xExtrap) }; };
  while ( sentinel( iter ) ) {
    auto predicate = make_predicate(*iter);
    auto range = sentinel.next_range( iter, std::cref(predicate) );
    if (!range.empty() && range.size()  <  m_maxXCandidateSize ) {
      //== Protect against too dirty area.
      //== Merge the lists, if the first new point is close to the last one...
      if (!m_candidates.empty() && predicate(m_candidates.back().coords().back(), range.front() )) {
        m_candidates.back().addCoords( std::begin(range), std::end(range));
      } else {
        m_candidates.emplace_back( track.track(), std::begin(range), std::end(range) );
      }
    }
    iter = std::end(range);
  }
}

//=============================================================================
