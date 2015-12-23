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
#include <unordered_map>

// TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/KalmanFitResult.h"

#include "Event/RecSummary.h"
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
#include "TrackInterfaces/IGhostProbability.h"

#include "TrackKernel/TrackCloneData.h"

#include "remove_if_partitioned.h"

namespace  {
  /// structure to save some data for each track
  class TrackData : public LHCb::TrackCloneData<>
  {
  private:
    double m_qOverP;
    enum { Clone = 1 };
  public:
    /// constructor
    TrackData(std::unique_ptr<LHCb::Track>&& tr) : 
      LHCb::TrackCloneData<>(std::move(tr)), 
      m_qOverP(track().firstState().qOverP())
    { }
    /// return q/p (or what it was at construction time)
    double qOverP() const { return m_qOverP; }
    bool cloneFlag() const { return userFlags() & Clone; }
    void setCloneFlag() { setUserFlags(userFlags() | Clone); }
  };
}

/** @brief kill clones among tracks in input container and Kalman-fit survivors
 *
 * @author Wouter Hulsbergen
 * - initial release
 *
 * @author Manuel Schiller
 * @date 2014-11-18
 * - simplify, C++11 cleanups, use BloomFilter based TrackCloneData
 * @date 2014-12-09
 * - add code to use ancestor information on tracks to deal with obvious clones
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
  ToolHandle<IGhostProbability> m_ghostTool;
  
  void fillRecTrackSummary();
  

  // job options
  // -----------
  // input Track container paths
  std::vector<std::string> m_tracksInContainers;
  // output Track container path
  std::string m_tracksOutContainer;
  double m_maxOverlapFracVelo;
  double m_maxOverlapFracT;
  double m_maxOverlapFracTT;
  double m_minLongLongDeltaQoP;
  double m_minLongDownstreamDeltaQoP;
  double m_maxChi2DoF;
  double m_maxChi2DoFVelo;
  double m_maxChi2DoFT;
  double m_maxChi2DoFMatchAndTT;
  double m_maxGhostProb;
  /// fit the tracks using the Kalman filter?
  bool m_fitTracks;
  /// initialise track states using m_stateinittool?
  bool m_initTrackStates;
  /// Add the Ghost Probability to a track
  bool m_addGhostProb;
  /// use ancestor information to identify obvious clones
  bool m_useAncestorInfo;
  /// Do not refit already fitted tracks
  bool m_doNotRefit;
  /// Put final tracks into separate containers according to track type
  bool m_splitByType;
  /// Fill the RecSummary object for information about number of rejected 'ghost' tracks
  bool m_fillRecTrackSummary;
  std::map<std::string,std::string> m_tracksOutContainers;
  std::vector<int> m_supportedTrackTypes;
  
  bool m_debugLevel;
  mutable int m_nGhosts;
  
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
  
  /// mapping between original track and the index of its copy
  typedef std::pair<const LHCb::Track*, size_t> CopyMapEntry;
  typedef std::vector<CopyMapEntry> CopyMap;
  
  /** @brief "generator" for TrackData objects
   *
   * @author Manuel Schiller
   * @date 2014-11-18
   *
   * goes through track containers, clones the tracks, pre-initialises the
   * Kalman filter fit with the TrackStateInitTool, creates a TrackData object
   * from it, and returns a the newly created object
   * 
   * if m_useAncestorInfo is set in the parent, it uses the ancestor field in
   * the track class to flag a large class of obvious clones (e.g. the Velo
   * and T station tracks that got promoted to long); this relies on
   * algorithms properly recording ancestor information, but is highly
   * effective in speeding things up when available
   */
  class TrackDataGen {
  private:
    TrackBestTrackCreator& m_parent; ///< parent TrackBestTrackCreator instance
    decltype(m_tracksInContainers.begin()) m_curcont; ///< next track container
    LHCb::Track::Range m_currange; ///< tracks in current track container
    decltype(m_currange.begin()) m_currangeit; ///< next track
    
    /// keep track of number of tracks returned
    size_t m_nrtracks;
    /// keep record between original track and copies
    CopyMap& m_copymap;
    
  public:
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
    TrackDataGen(TrackBestTrackCreator& parent, CopyMap& copymap);
    
    /** @brief destructo
     *
     * flags ancestors as clones if m_useAncestorInfo is set in parent
     */
    ~TrackDataGen();
    
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

TrackBestTrackCreator::TrackDataGen::TrackDataGen(TrackBestTrackCreator& parent, CopyMap& copymap) :
m_parent(parent), m_curcont(m_parent.m_tracksInContainers.begin()),
  m_currange(), m_currangeit(m_currange.end()),
  m_nrtracks(0), m_copymap(copymap)
{ }

TrackBestTrackCreator::TrackDataGen::~TrackDataGen()
{
  // sort m_copymap for quick lookup
  std::sort(std::begin(m_copymap), std::end(m_copymap),
            [] (const CopyMapEntry& a, const CopyMapEntry& b)
            { return a.first < b.first; });
}

TrackData TrackBestTrackCreator::TrackDataGen::operator()()
{
  while (true) {
    if (m_currangeit != m_currange.end()) {
      // not at end of current container, so get next track
      const LHCb::Track* oldtr = *m_currangeit++;
      // clone track
      std::unique_ptr<LHCb::Track> tr(oldtr->clone());
      // pre-initialise (if required)
      const bool fitted = m_parent.m_doNotRefit && ( tr->fitStatus() == LHCb::Track::Fitted ||
                                                     tr->fitStatus() == LHCb::Track::FitFailed );
      if (fitted && tr->fitStatus() == LHCb::Track::FitFailed )
        continue;
      StatusCode sc(m_parent.m_initTrackStates  && !fitted ?
                    m_parent.m_stateinittool->fit(*tr, true) : StatusCode::SUCCESS);
      // if successful, return new TrackData object
      if (sc.isSuccess()) {
        if (m_parent.m_useAncestorInfo) {
          // save mapping between original track and its copy
          m_copymap.emplace_back(oldtr, m_nrtracks);
        }
        ++m_nrtracks;
        // keep a record where this track came from
        tr->addToAncestors(oldtr);
        return TrackData(std::move(tr));
      }
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
  m_fitter("TrackMasterFitter",this),
  m_ghostTool("Run2GhostId",this),
  m_nGhosts(0)
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
  declareProperty( "MaxGhostProb", m_maxGhostProb = 99999); // The default value is 999
  declareProperty( "UseAncestorInfo", m_useAncestorInfo = true);
  declareProperty( "DoNotRefit", m_doNotRefit = false);
  declareProperty( "AddGhostProb", m_addGhostProb = false);
  declareProperty( "GhostIdTool", m_ghostTool );
  declareProperty( "SplitByType", m_splitByType = false);
  declareProperty( "FillRecTrackSummary", m_fillRecTrackSummary = false);
  declareProperty( "TracksOutContainers", m_tracksOutContainers );
  m_supportedTrackTypes = { 1, 3, 4, 5, 6}; //Velo, Long, Upstream, Downstream, TTrack
  for ( auto type : m_supportedTrackTypes ){
    m_tracksOutContainers[LHCb::Track::TypesToString(type)] = "";
  }
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
  if (m_addGhostProb){
    sc =  m_ghostTool.retrieve();
    if ( sc.isFailure() ) return sc; 
  }
  
  if (m_splitByType){
    for (auto it :  m_tracksOutContainers){
      if( m_debugLevel ){
        debug()<<"Splitting types up into:"<<endmsg;
        debug()<<"  "<<it.first<<" -> "<<it.second<<endmsg;
      }
      if (std::none_of(m_supportedTrackTypes.begin(),m_supportedTrackTypes.end(),
                       [it](int i){return it.first==LHCb::Track::TypesToString(i);})){
        warning()<<it.first << " does not correspond to a supported track type."<<endmsg;
      }
    }
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
  if (m_addGhostProb){
    m_ghostTool->beginEvent().ignore();
  }
  m_nGhosts = 0;
  
  // create pool for TrackData objects for all input tracks 
  std::vector<TrackData> trackdatapool;
  // keep a record of which track goes where
  CopyMap copymap;
  // get total number of tracks
  size_t nTracks = std::accumulate(std::begin(m_tracksInContainers), std::end(m_tracksInContainers),
                                   size_t(0), [this] (size_t sz, const std::string& loc)
                                   { return sz + get<LHCb::Track::Range>(loc).size(); });
  // reserve enough space so we don't have to reallocate
  trackdatapool.reserve(nTracks);
  copymap.reserve(nTracks);
  try {
    // generate the TrackData objects for the input tracks, initialising the
    // States for use in the Kalman filter on the way
    TrackDataGen gen(*this, copymap);
    std::generate_n(std::back_inserter(trackdatapool), nTracks, gen);
  } catch (const TrackDataGen::DataExhaustedException&) {
    // nothing to do - occasionally running out of tracks to convert is
    // expected
  }
  
  // take a vector of "references" which is much easier to sort (because less
  // data is moved around)
  std::vector<std::reference_wrapper<TrackData> > alltracks(std::begin(trackdatapool), std::end(trackdatapool));
  
  // sort them by quality
  std::stable_sort(alltracks.begin(), alltracks.end(),
                   [] (const TrackData& t1, const TrackData& t2) { return t1 < t2; });
  
  // remove and erase clones and unfittable tracks
  auto selend = remove_if_partitioned(std::begin(alltracks), std::end(alltracks),
                                      // lambda function to find clones and unfittable tracks
                                      [this, &alltracks, &copymap, &trackdatapool] (TrackData& tr1,
                                                                                    const decltype(std::begin(alltracks))& selend)
                                      { 
                                        // check ancestor based clone flagging
                                        if (tr1.cloneFlag()) return true;
                                        // selected tracks will be in range from std::begin(alltracks) to
                                        // selend find first track tr2 in that range that is a clone of the
                                        // track we're considering (tr1)
                                        const auto firstclone = std::find_if(std::begin(alltracks), selend,
                                                                             [this, &tr1] (const TrackData& tr2)
		{ return areClones(tr1, tr2); });
                                        // if tr1 is a clone of something we already selected, say so
                                        if (selend != firstclone) return true;
                                        // else try to fit, and see what happens
                                        const bool okay = fitAndSelect(tr1.track());
                                        if (okay && m_useAncestorInfo) {
                                          // track survives, so no Clone, proceed to mark ancestors
                                          for (const auto& anc: tr1.track().ancestors()) {
                                            const auto it = std::lower_bound(
                                                                             copymap.begin(), copymap.end(), anc,
                                                                             [] (const CopyMapEntry& cme, const LHCb::Track* tr)
                                                                             { return cme.first < tr; });
                                            if (copymap.end() == it || it->first != anc) continue;
                                            trackdatapool[it->second].setCloneFlag();
                                          }
                                        }
                                        return !okay;
                                      });
  alltracks.erase(selend, std::end(alltracks));
  if (!m_splitByType){
    // create output container, and put selected tracks there
    LHCb::Tracks* tracksOutCont = new LHCb::Tracks();
    put(tracksOutCont, m_tracksOutContainer);
    // insert selected tracks
    tracksOutCont->reserve(alltracks.size());
    for (TrackData& tr: alltracks) {
      // make tr release ownership of track
      tracksOutCont->add(tr.trackptr().release());
    }
  }else{
    /// create mapping, LHCb::Track::Ttrack has the enum 7
    std::unordered_map<int,LHCb::Tracks*> tracksOutContsMap;
    for (auto it : m_tracksOutContainers ){
      auto type = LHCb::Track::TypesToType(it.first);
      if (it.second!="" && type!=LHCb::Track::TypeUnknown){
        tracksOutContsMap[type] = new LHCb::Tracks();
        put( tracksOutContsMap[type], it.second);
      }
    }
    for (TrackData& tr: alltracks) {
      // make tr release ownership of track
      auto container = tracksOutContsMap.find(tr.track().type());
      if ( container != tracksOutContsMap.end())
        container->second->add(tr.trackptr().release());
      else{
        if (m_debugLevel) {
          debug() << "No outputlocation for track type "<< tr.track().type()<<" was specified!" << endmsg;
        }
      }
    }
  }
  
  if( m_fillRecTrackSummary) fillRecTrackSummary();
  

  if (m_debugLevel) {
    debug() << "Selected " << alltracks.size() << " out of "
            << nTracks << " tracks. Rejected " << m_nGhosts << endmsg;
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
        //always()<< track.ghostProbability ()<<"  "<< m_maxGhostProb<<endmsg;
        
        accept = 
          chi2.chi2PerDoF() < m_maxChi2DoF &&
          chi2T.chi2PerDoF() < m_maxChi2DoFT &&
          chi2Velo.chi2PerDoF() < m_maxChi2DoFVelo &&
          chi2MatchAndTT.chi2PerDoF() < m_maxChi2DoFMatchAndTT &&
          ( !m_addGhostProb || track.ghostProbability () < m_maxGhostProb);
        if( !accept ) ++m_nGhosts;
        
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
  /// for counters
  std::string prefix = Gaudi::Utils::toString(track.type());
  if( track.checkFlag( LHCb::Track::Backward ) ) prefix += "Backward";
  prefix += '.';
  
  if (m_doNotRefit && (track.fitStatus() == LHCb::Track::Fitted
                       || track.fitStatus() == LHCb::Track::FitFailed) ){
    if (track.fitStatus() == LHCb::Track::Fitted){
      counter("FittedBefore") += 1;
      sc = StatusCode::SUCCESS;
      if (m_addGhostProb) m_ghostTool->execute(track).ignore();
    }
    else {
      /// fit failed before
      /// This should always be 0 as this type is filtered out when initializing the tracks
      counter("FitFailedBefore") += 1;
      track.setFlag( LHCb::Track::Invalid, true );
      sc = StatusCode::FAILURE;
    }
  }else{
    if( track.nStates()==0 ||
  	    track.checkFlag( LHCb::Track::Invalid ) )  {
  	  track.setFlag( LHCb::Track::Invalid, true );
  	  badinput = true;
  	} else {
  	  double qopBefore = track.firstState().qOverP();
  	  /// Fit the track 
  	  sc =  m_fitter -> fit( track );

  	  if ( sc.isSuccess()){
        if (m_addGhostProb) m_ghostTool->execute(track).ignore();
        // Update counters
        if( track.nDoF()>0) {
  	      double chisqprob = track.probChi2();
  	      counter(prefix + "chisqprobSum") += chisqprob;
  	      counter(prefix + "badChisq") += bool(chisqprob<0.01);
  	    }
  	    counter(prefix + "flipCharge") += bool( qopBefore * track.firstState().qOverP() <0);
  	    counter(prefix + "numOutliers") += track.nMeasurementsRemoved();
      } else {
  	    track.setFlag( LHCb::Track::Invalid, true );
  	    fitfailed = true;
        counter(prefix + "FitFailed") += int(fitfailed);
      }
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

//=============================================================================
// Put number of ghosts (aka tracks rejected by chi2 and ghost prob cuts) on the TES
//=============================================================================
void TrackBestTrackCreator::fillRecTrackSummary(){
  
  // Create a new summary object and save to the TES
  LHCb::RecSummary* summary = new LHCb::RecSummary();
  put( summary, LHCb::RecSummaryLocation::Track );

  summary->addInfo( LHCb::RecSummary::nGhosts, m_nGhosts );
  
}
