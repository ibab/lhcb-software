/** @file TrackBestTrackCreator.cpp
 *
 * @autor Wouter Hulsbergen
 *
 * @author Manuel Schiller
 * @date 2014-11-18
 */

// Include files 
// -------------
// from STD
#include <algorithm>
#include <exception>
#include <functional>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/KalmanFitResult.h"

// LHCbKernel
#include "Kernel/HitPattern.h"

// local

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#if DEBUGHISTOGRAMS
#include "GaudiAlg/GaudiHistoAlg.h"
#endif
#include "GaudiKernel/ToolHandle.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackCloneFinder.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackStateInit.h"
#include "TrackKernel/TrackCloneData.h"

namespace  {
  /// structure to save some data for each track
  class TrackData : public LHCb::TrackCloneData<>
  {
    private:
      double m_qOverP;
    public:
      /// constructor
      TrackData(std::unique_ptr<LHCb::Track>&& tr) : 
	LHCb::TrackCloneData<>(std::move(tr)), 
	m_qOverP(track().firstState().qOverP())
      { }
      /// return q/p (or what it was at construction time)
      double qOverP() const { return m_qOverP; }
  };

  /** remove elements if binary predicate is satisfied
   *
   * @author Manuel Schiller
   * @date 2014-11-19
   *
   * @param begin	start of range of elements to be processed
   * @param end		end of range of elements to be processed
   * @param p		binary predicate to decide which elements to remove
   *
   * The binary predicate p takes the current element being tested as first
   * argument, and an iterator pointing to the end of the range of elements
   * already processed as second argument. The second argument thus partitions
   * the range into the range of elements from begin to the second argument
   * which is known to contain elements that will not be removed, and the rest.
   *
   * This facilitates removal of equivalent elements from a range, for example:
   * @code
   * class Foo;
   * std::vector<Foo> v;
   * // assume v contains elements sorted according to some quality criterion,
   * // and Foo::equiv(const Foo&) allows to check for equivalence of some
   * // sort; then the following will keep the highest quality unique elements
   * auto newend = remove_if_partitioned(std::begin(v), std::end(v),
   *     [&v] (const Foo& el, decltype(std::begin(v)) doneend)
   *     { return doneend != std::find_if(std::begin(v), doneend,
   *           [&el] (const Foo& el2) { return el.equiv(el2); });
   *     });
   * v.erase(newend, std::end(v));
   * @endcode
   */
  template <class ForwardIterator, class BinaryPredicate>
  ForwardIterator remove_if_partitioned(
      ForwardIterator begin, ForwardIterator end,
      BinaryPredicate p)
  {
    for (auto it = begin; end != it; ++it) {
      if (p(*it, begin)) continue;
      if (begin != it) *begin = std::move(*it);
      ++begin;
    }
    return begin;
  }
}
  
/** @brief kill clones among tracks in input container and Kalman-fit survivors
 *
 * @author Wouter Hulsbergen
 * - initial release
 *
 * @author Manuel Schiller
 * @date 2014-11-18
 * - simplify, C++11 cleanups, use BloomFilter based TrackCloneData
 */
class TrackBestTrackCreator : 
#if DEBUGHISTOGRAMS
  public GaudiHistoAlg
#else
  public GaudiAlgorithm
#endif
{
  public: 
    /// Standard constructor
    TrackBestTrackCreator(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~TrackBestTrackCreator( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private:
    ToolHandle<ITrackStateInit> m_stateinittool;
    ToolHandle<ITrackFitter> m_fitter;
    bool m_debugLevel;

    // job options
    // -----------
    // input Track container paths
    std::vector<std::string> m_tracksInContainers;
    // output Track container path
    std::string m_tracksOutContainer;
    bool m_fitTracks;
    bool m_initTrackStates;
    double m_maxOverlapFracVelo;
    double m_maxOverlapFracT;
    double m_maxOverlapFracTT;
    double m_minLongLongDeltaQoP;
    double m_minLongDownstreamDeltaQoP;
    double m_maxChi2DoF;
    double m_maxChi2DoFVelo;
    double m_maxChi2DoFT;
    double m_maxChi2DoFMatchAndTT;

  protected:
    /// are tracks clones in VeloR and VeloPhi
    bool veloClones(const TrackData&, const TrackData&) const;
    /// are tracks clones in VeloR or VeloPhi
    bool veloOrClones(const TrackData&, const TrackData&) const;
    /// are tracks clones in T
    bool TClones(const TrackData&, const TrackData&) const;
    /// are tracks clones in TT
    bool TTClones(const TrackData&, const TrackData&) const;
    /// fit a track
    StatusCode fit(LHCb::Track& track) const;
    /// fit a track, then check if it passes selection requirements
    bool fitAndSelect(LHCb::Track&) const;

    /// check if tracks pointed to by their TrackData objects are clones
    bool areClones(const TrackData& it, const TrackData& jt);

    /** @brief "generator" for TrackData objects
     *
     * @author Manuel Schiller
     * @date 2014-11-18
     *
     * goes through track containers, clones the tracks, pre-initialises the
     * Kalman filter fit with the TrackStateInitTool, creates a TrackData object
     * from it, and returns a the newly created object
     */
    struct TrackDataGen {
      TrackBestTrackCreator& m_parent; ///< parent TrackBestTrackCreator instance
      decltype(m_tracksInContainers.begin()) m_curcont; ///< next track container
      LHCb::Track::Range m_currange; ///< tracks in current track container
      decltype(m_currange.begin()) m_currangeit; ///< next track

      /// exception to throw when all tracks have been consumed
      class DataExhaustedException : public std::exception
      {
	private:
	  const char* m_what;
	public:
	  DataExhaustedException(const char* what) :
	    m_what(what) { }
	  virtual const char* what() const noexcept { return m_what; }
      };

      /// constructor
      TrackDataGen(TrackBestTrackCreator& parent) :
	m_parent(parent), m_curcont(m_parent.m_tracksInContainers.begin()),
	m_currange(), m_currangeit(m_currange.end())
      { }

      /// call to get the next TrackData object
      TrackData operator()();
    };
};

//-----------------------------------------------------------------------------
// Implementation file for class : TrackBestTrackCreator
//
// 2011-12-01 Wouter Hulsbergen
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackBestTrackCreator )

TrackData TrackBestTrackCreator::TrackDataGen::operator()()
{
  while (true) {
    if (m_currangeit != m_currange.end()) {
      // not at end of current container, so get next track, and clone it
      std::unique_ptr<LHCb::Track> tr((*m_currangeit++)->clone());
      // pre-initialise (if required)
      StatusCode sc(m_parent.m_initTrackStates ?
	  m_parent.m_stateinittool->fit(*tr, true) : StatusCode::SUCCESS);
      // if successful, return new TrackData object
      if (sc.isSuccess()) return TrackData(std::move(tr));
      // report error in log file
      m_parent.Warning("TrackStateInitTool fit failed", sc, 0 ).ignore();
    } else {
      // need to start with new container
      if (m_parent.m_tracksInContainers.end() == m_curcont) {
	// all tracks read from all containers, nothing to do any more
	throw DataExhaustedException("No more tracks.");
      }
      m_currange = m_parent.get<LHCb::Track::Range>(*m_curcont++);
      m_currangeit = m_currange.begin();
    }
    // move on to next track
  }
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackBestTrackCreator::TrackBestTrackCreator( const std::string& name,
						    ISvcLocator* pSvcLocator) :
#if DEBUGHISTOGRAMS
  GaudiHistoAlg ( name , pSvcLocator ),
#else
  GaudiAlgorithm ( name , pSvcLocator ),
#endif
  m_stateinittool("TrackStateInitTool",this),
  m_fitter("TrackMasterFitter",this)
{
  // default list of input containers
  m_tracksInContainers.push_back( LHCb::TrackLocation::Forward    );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Match      );
  m_tracksInContainers.push_back( LHCb::TrackLocation::VeloTT     );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Downstream );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Tsa        );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Velo       );

  declareProperty( "TracksInContainers",m_tracksInContainers );
  declareProperty( "TracksOutContainer", m_tracksOutContainer = LHCb::TrackLocation::Default );
  declareProperty( "MaxOverlapFracVelo", m_maxOverlapFracVelo = 0.5);
  declareProperty( "MaxOverlapFracT", m_maxOverlapFracT = 0.5);
  declareProperty( "MaxOverlapFracTT", m_maxOverlapFracTT = 0.35) ; // essentially: max 1 common hit
  declareProperty( "InitTrackStates", m_initTrackStates = true );
  declareProperty( "FitTracks", m_fitTracks = true );
  declareProperty( "StateInitTool", m_stateinittool );
  declareProperty( "Fitter", m_fitter );
  declareProperty( "MaxChi2DoF", m_maxChi2DoF = 3 );
  declareProperty( "MaxChi2DoFVelo", m_maxChi2DoFVelo = 999 );
  declareProperty( "MaxChi2DoFT", m_maxChi2DoFT = 999 );
  declareProperty( "MaxChi2DoFMatchTT", m_maxChi2DoFMatchAndTT = 999 );
  declareProperty( "MinLongLongDeltaQoP", m_minLongLongDeltaQoP = -1 );
  declareProperty( "MinLongDownstreamDeltaQoP", m_minLongDownstreamDeltaQoP = 5e-6 );
}

//=============================================================================
// Destructor
//=============================================================================
TrackBestTrackCreator::~TrackBestTrackCreator() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackBestTrackCreator::initialize()
{
#if DEBUGHISTOGRAMS
  StatusCode sc = GaudiHistoAlg::initialize();
#else
  StatusCode sc = GaudiAlgorithm::initialize();
#endif
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_debugLevel = msgLevel( MSG::DEBUG );  
  if( m_debugLevel ) debug() << "==> Initialize" << endmsg;
  
  // ------------------------------
  if( m_initTrackStates ) {
    sc = m_stateinittool.retrieve();
    if ( sc.isFailure() ) return sc; 
  }
  if( m_fitTracks ) {
    sc = m_fitter.retrieve();
    if ( sc.isFailure() ) return sc; 
  }

  // Print out the user-defined settings
  // -----------------------------------
  if( m_debugLevel ) 
    debug() << endmsg
	    << "============ TrackBestTrackCreator Settings ===========" << endmsg
	    << "TracksInContainers : " << getProperty( "TracksInContainers" ).toString()                    << endmsg
	    << "TrackOutContainer  : " << m_tracksOutContainer                                              << endmsg
	    << "=======================================================" << endmsg
	    << endmsg;
  
  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackBestTrackCreator::finalize()
{
  if( m_debugLevel ) debug() << "==> Finalize" << endmsg;

  if( m_fitTracks ) {
    double perf = (1.0 - counter("FitFailed").mean())*100;
    info() << "  Fitting performance   : "
	   << format( " %7.2f %%", perf ) << endmsg;
  }
  
  m_stateinittool.release().ignore();
  m_fitter.release().ignore();
#if DEBUGHISTOGRAMS
  return GaudiHistoAlg::finalize();
#else
  return GaudiAlgorithm::finalize();
#endif
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackBestTrackCreator::execute()
{  
  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;

  // create pool for TrackData objects for all input tracks 
  std::vector<TrackData> trackdatapool;
  // get total number of tracks
  size_t nTracks = std::accumulate(
      std::begin(m_tracksInContainers), std::end(m_tracksInContainers),
      size_t(0), [this] (size_t sz, const std::string& loc)
      { return sz + get<LHCb::Track::Range>(loc).size(); });
  // reserve enough space so we don't have to reallocate
  trackdatapool.reserve(nTracks);
  try {
    // generate the TrackData objects for the input tracks, initialising the
    // States for use in the Kalman filter on the way
    TrackDataGen gen(*this);
    std::generate_n(std::back_inserter(trackdatapool), nTracks, gen);
  } catch (const TrackDataGen::DataExhaustedException&) {
    // nothing to do - occasionally running out of tracks to convert is
    // expected
  }

  // take a vector of "references" which is much easier to sort (because less
  // data is moved around)
  std::vector<std::reference_wrapper<TrackData> > alltracks(
      std::begin(trackdatapool), std::end(trackdatapool));

  // sort them by quality
  std::stable_sort(alltracks.begin(), alltracks.end(),
      [] (const TrackData& t1, const TrackData& t2) { return t1 < t2; });

  // remove and erase clones and unfittable tracks
  auto selend = remove_if_partitioned(
	  std::begin(alltracks), std::end(alltracks),
	  // lambda function to find clones and unfittable tracks
	  [this, &alltracks] (TrackData& tr1,
	      const decltype(std::begin(alltracks))& selend)
	  { 
	    // selected tracks will be in range from std::begin(alltracks) to
	    // selend find first track tr2 in that range that is a clone of the
	    // track we're considering (tr1)
	    const auto firstclone = std::find_if(std::begin(alltracks), selend,
		[this, &tr1] (const TrackData& tr2)
		{ return areClones(tr1, tr2); });
	    // if tr1 is a clone of something we already selected, say so
	    if (selend != firstclone) return true;
	    // else try to fit, and see what happens
	    return !fitAndSelect(tr1.track());
	  });
  alltracks.erase(selend, std::end(alltracks));
  // create output container, and put selected tracks there
  LHCb::Tracks* tracksOutCont = new LHCb::Tracks();
  put(tracksOutCont, m_tracksOutContainer);
  // insert selected tracks
  tracksOutCont->reserve(alltracks.size());
  for (TrackData& tr: alltracks) {
    // make tr release ownership of track
    tracksOutCont->add(tr.trackptr().release());
  }
  
  if (m_debugLevel) {
    debug() << "Selected " << alltracks.size() << " out of "
	    << nTracks << " tracks. " << endmsg;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Clone a track, fit it, and if it passes the selection, return the
// cloned candidate.
// =============================================================================
bool TrackBestTrackCreator::fitAndSelect(LHCb::Track& track ) const
{
  bool accept = true;
  if( m_fitTracks ) {
    StatusCode sc = fit( track );
    const LHCb::KalmanFitResult* kalfit = 
      dynamic_cast<const LHCb::KalmanFitResult*>(track.fitResult());
    if(sc.isSuccess() && kalfit ) {
      // only apply the cut to composite tracks
      if( track.type() == LHCb::Track::Long ||
	  track.type() == LHCb::Track::Upstream ||
	  track.type() == LHCb::Track::Downstream ) {
	LHCb::ChiSquare chi2 = kalfit->chi2();
	LHCb::ChiSquare chi2T = kalfit->chi2Downstream();
	LHCb::ChiSquare chi2Velo = kalfit->chi2Velo();
	// note: this includes TT hit contribution
	LHCb::ChiSquare chi2MatchAndTT = chi2 - chi2T - chi2Velo;
	accept = 
	  chi2.chi2PerDoF() < m_maxChi2DoF &&
	  chi2T.chi2PerDoF() < m_maxChi2DoFT &&
	  chi2Velo.chi2PerDoF() < m_maxChi2DoFVelo &&
	  chi2MatchAndTT.chi2PerDoF() < m_maxChi2DoFMatchAndTT;
      }
      //   //if(!m_selector.accept( *track ) ) sc = StatusCode::FAILURE;
    } else {
      accept = false;
    }
  }
  return accept;
}

//=============================================================================
//  Fit a track, keeping some counters. This is copied from TrackEventFitter
//=============================================================================
StatusCode TrackBestTrackCreator::fit( LHCb::Track& track ) const
{
  // copied from TrackEventFitter
  StatusCode sc = StatusCode::FAILURE;
  bool badinput  = false;
  bool fitfailed = false;

  if( track.nStates()==0 ||
      track.checkFlag( LHCb::Track::Invalid ) )  {
    track.setFlag( LHCb::Track::Invalid, true );
    badinput = true;
  } else {
    double qopBefore = track.firstState().qOverP();
    sc = m_fitter -> fit( track );
    if ( sc.isSuccess() ) {
      // Update counters
      std::string prefix = Gaudi::Utils::toString(track.type());
      if( track.checkFlag( LHCb::Track::Backward ) ) prefix += "Backward";
      prefix += '.';
      if( track.nDoF()>0) {
	double chisqprob = track.probChi2();
	counter(prefix + "chisqprobSum") += chisqprob;
	counter(prefix + "badChisq") += bool(chisqprob<0.01);
      }
      counter(prefix + "flipCharge") += bool( qopBefore * track.firstState().qOverP() <0);
      counter(prefix + "numOutliers") += track.nMeasurementsRemoved();
    }
    else {
      track.setFlag( LHCb::Track::Invalid, true );
      fitfailed = true;
    }
  }
  // stick as close as possible to whatever EventFitter prints
  counter("BadInput")  += int(badinput);
  counter("FitFailed") += int(fitfailed);

  return sc;
}

bool TrackBestTrackCreator::veloClones(const TrackData& lhs,
				       const TrackData& rhs) const
{
  const double fR   = lhs.overlapFraction(rhs, TrackData::VeloR);
  const double fPhi = lhs.overlapFraction(rhs, TrackData::VeloPhi);
#ifdef DEBUGHISTOGRAMS
  if(fR>0) plot1D(fR,"veloROverlapFractionH1",0,1);
  if(fPhi>0) plot1D(fPhi,"veloPhiOverlapFractionH1",0,1);
#endif
  return fR > m_maxOverlapFracVelo && fPhi > m_maxOverlapFracVelo;
}

bool TrackBestTrackCreator::veloOrClones(const TrackData& lhs,
					 const TrackData& rhs) const
{
  const double fR   = lhs.overlapFraction(rhs, TrackData::VeloR);
#ifndef DEBUGHISTOGRAMS
  if (fR > m_maxOverlapFracVelo) return true;
#endif
  const double fPhi = lhs.overlapFraction(rhs, TrackData::VeloPhi);
#ifdef DEBUGHISTOGRAMS
  if(fR>0) plot1D(fR,"veloROverlapFractionH1",0,1);
  if(fPhi>0) plot1D(fPhi,"veloPhiOverlapFractionH1",0,1);
  return (fR > m_maxOverlapFracVelo) || (fPhi > m_maxOverlapFracVelo);
#else
  return fPhi > m_maxOverlapFracVelo;
#endif
}

bool TrackBestTrackCreator::TClones(const TrackData& lhs,
				    const TrackData& rhs) const
{
  const double f = lhs.overlapFraction(rhs,TrackData::T);
#ifdef DEBUGHISTOGRAMS
  if(f>0) plot1D(f,"TOverlapFractionH1",0,1);
#endif
  return f > m_maxOverlapFracT;
}

bool TrackBestTrackCreator::TTClones(const TrackData& lhs,
				     const TrackData& rhs) const
{
  const double f = lhs.overlapFraction(rhs,TrackData::TT);
#ifdef DEBUGHISTOGRAMS
  if(f>0) plot1D(f,"TTOverlapFractionH1",0,1);
#endif
  return f > m_maxOverlapFracTT;
}

bool TrackBestTrackCreator::areClones(const TrackData& it, const TrackData& jt)
{
  const LHCb::Track &itrack(it.track()), &jtrack(jt.track());
  const int itype(itrack.type()), jtype(jtrack.type());
  const double dqop = it.qOverP() - jt.qOverP();
  const int offset = 256;
  switch (itype + offset * jtype) {
    case LHCb::Track::Long + offset * LHCb::Track::Long:
#ifdef DEBUGHISTOGRAMS
      if (TClones(it, jt) && veloOrClones(it, jt)) {
	plot(dqop, "LLDqopClones", -1e-5, 1e-5);
      } else if (TClones(it, jt)) {
	plot(dqop, "LLDqopTClones", -1e-5, 1e-5);
      } else if (veloOrClones(it, jt)) {
	plot(dqop, "LLDqopVeloOrClones", -1e-5, 1e-5);
      }
#endif
      return TClones(it, jt) && 
	(std::abs(dqop) < m_minLongLongDeltaQoP || veloOrClones(it, jt));
    case LHCb::Track::Long + offset*LHCb::Track::Downstream:
    case LHCb::Track::Downstream + offset*LHCb::Track::Long:
#ifdef DEBUGHISTOGRAMS
      if (TClones(it, jt)) {
	plot(dqop, "DLDqop", -2e-5, 2e-5);
	if (TTClones(it, jt)) 
	  plot(dqop, "DLDqopTTClones", -2e-5, 2e-5);
      }
#endif
      return TClones(it, jt) && 
	(std::abs(dqop) < m_minLongDownstreamDeltaQoP || TTClones(it, jt));
    case LHCb::Track::Downstream + offset*LHCb::Track::Downstream:
      // it seems that there are no down stream tracks that share T hits ...
#ifdef DEBUGHISTOGRAMS
      if(TClones(it, jt)) {
	plot(dqop, "DDDqop", -1e-4, 1e-4);
      }
#endif
      return TClones(it, jt) && TTClones(it, jt);
    case LHCb::Track::Long + offset*LHCb::Track::Upstream:
    case LHCb::Track::Upstream + offset*LHCb::Track::Long:
    case LHCb::Track::Upstream + offset*LHCb::Track::Upstream:
      return veloOrClones(it, jt) && TTClones(it, jt);
    case LHCb::Track::Long + offset*LHCb::Track::Velo:
    case LHCb::Track::Velo + offset*LHCb::Track::Long:
    case LHCb::Track::Upstream + offset*LHCb::Track::Velo:
    case LHCb::Track::Velo + offset*LHCb::Track::Upstream:
    case LHCb::Track::Velo + offset*LHCb::Track::Velo:
      return veloOrClones(it, jt);
    case LHCb::Track::Long + offset*LHCb::Track::Ttrack:
    case LHCb::Track::Ttrack + offset*LHCb::Track::Long:
    case LHCb::Track::Downstream + offset*LHCb::Track::Ttrack:
    case LHCb::Track::Ttrack + offset*LHCb::Track::Downstream:
    case LHCb::Track::Ttrack + offset*LHCb::Track::Ttrack:
      return TClones(it, jt);
    case LHCb::Track::Ttrack + offset*LHCb::Track::Upstream:
    case LHCb::Track::Upstream + offset*LHCb::Track::Ttrack:
    case LHCb::Track::Ttrack + offset*LHCb::Track::Velo:
    case LHCb::Track::Velo + offset*LHCb::Track::Ttrack:
    case LHCb::Track::Downstream + offset*LHCb::Track::Velo:
    case LHCb::Track::Velo + offset*LHCb::Track::Downstream:
    case LHCb::Track::Downstream + offset*LHCb::Track::Upstream:
    case LHCb::Track::Upstream + offset*LHCb::Track::Downstream:
      break;
    default:
      error() << "Don't know how to handle combi: " << itype << " " 
	<< jtype << endmsg;
  }
  return false;
}

