// Include files 
// -------------
// from STD
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"
#include "Event/KalmanFitResult.h"

// LHCbKernel
#include "Kernel/HitPattern.h"

// from BOOST
#include <boost/foreach.hpp>

// local

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackCloneFinder.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackStateInit.h"
#include "DetDesc/StaticArray.h"

namespace  {
  // structure to save some data for each track
  class TrackData
  {
    // we can probably do this with ranges since the lhcbids are
    // ordered by detector type. for now, let's just create containers
  public:
    //typedef Range_<LHCb::Track::LHCbIDContainer> LHCbIDs ;
    //typedef StaticArray<LHCb::LHCbID,128> LHCbIDs ;
    typedef std::vector<LHCb::LHCbID> LHCbIDs;
    LHCbIDs veloIds, veloRIds,veloPhiIds ;
    LHCbIDs TTIds ;
    LHCbIDs TIds ;
  private:
    LHCb::Track* m_track ;
    unsigned long int m_weight ;
    double m_qOverP ;
    bool m_owntrack ;

    // don't allow copies, because ownership too complicated
    TrackData( const TrackData& ) {}
  public:
    TrackData() : m_track(NULL), m_weight(0), m_qOverP(0.), m_owntrack(false) {}
    TrackData(LHCb::Track* atrack, bool owntrack=true)
      : m_track(atrack),
        m_weight(0), 
        m_qOverP(atrack->firstState().qOverP()),
        m_owntrack(owntrack)
    {
      veloIds.reserve(48); veloRIds.reserve(24); veloPhiIds.reserve(24);
      TTIds.reserve(6);
      TIds.reserve(24);
      const LHCb::Track::LHCbIDContainer& ids = atrack->lhcbIDs() ;
      for( LHCb::Track::LHCbIDContainer::const_iterator id = ids.begin() ;
           id != ids.end(); ++id ) {
        switch( id->detectorType() ) {
        case LHCb::LHCbID::Velo: 
          veloIds.push_back(*id) ; 
          if( id->isVeloR() ) veloRIds.push_back(*id) ;
          else veloPhiIds.push_back(*id) ;
          break ;
        case LHCb::LHCbID::TT: TTIds.push_back(*id) ; break ;
        case LHCb::LHCbID::OT: 
        case LHCb::LHCbID::IT: TIds.push_back(*id) ; break ;
        default: break ;
        }
      }
      // compute a weight for sorting.
      LHCb::HitPattern hp(ids) ;
      // first sort by type
      int weightFromType[10] = {0,11,10,20,15,16,12,2,3} ; 
      m_weight = 100000 * weightFromType[atrack->type()] ;
      // then sort by the number of TT planes
      m_weight += 10000*hp.tt().count() ;
      // then sort by number of T layers / velo _clusters_
      m_weight += 100*hp.numTLayers() ;
      m_weight += 100*( (hp.veloRA()|hp.veloRC()) & (hp.veloPhiA()|hp.veloPhiC()) ).count() ;
      // only finally sort by total number of hits
      m_weight += ids.size() ;      
    }

    // destructor
    ~TrackData() {
      if( m_owntrack ) delete m_track ;
    }

    // return the track
    LHCb::Track* track() const { return m_track ; }
    // release ownership of the track
    LHCb::Track* release() { m_owntrack = false ; return m_track ; }
    // sort by weight
    bool operator<( const TrackData& rhs) const { return m_weight > rhs.m_weight ; }
    // get q/p (set by stateinittool)
    double qOverP() const { return m_qOverP ; }
    // return the weight used for sorting
    unsigned long int weight() const { return m_weight; }
  } ;

  struct TrackDataSorter
  {
    bool operator()(const TrackData* lhs, const TrackData* rhs) {
      return *lhs < *rhs ;
    }
  } ;
}
  
/** @class TrackBestTrackCreator TrackBestTrackCreator.cpp
 *  
 *
 *  @author Wouter Hulsbergen
 *
 */
class TrackBestTrackCreator : public GaudiHistoAlg {
public: 
  /// Standard constructor
  TrackBestTrackCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackBestTrackCreator( ); ///< Destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  bool veloClones( const TrackData&, const TrackData&) const ;
  bool veloOrClones( const TrackData&, const TrackData&) const ;
  bool TClones( const TrackData&, const TrackData&) const ;
  bool TTClones( const TrackData&, const TrackData&) const ;
  StatusCode fit( LHCb::Track& track ) const ;
  bool fitAndSelect(LHCb::Track&) const ;
private:
  ToolHandle<ITrackStateInit> m_stateinittool ;
  ToolHandle<ITrackFitter> m_fitter ;
  bool m_debugLevel;

  // job options
  // -----------
  // input Track container paths
  std::vector<std::string> m_tracksInContainers;
  // output Track container path
  std::string m_tracksOutContainer;
  bool m_fitTracks ;
  bool m_initTrackStates ;
  double m_maxOverlapFracVelo ;
  double m_maxOverlapFracT ;
  double m_maxOverlapFracTT ;
  double m_minLongLongDeltaQoP ;
  double m_minLongDownstreamDeltaQoP ;
  double m_maxChi2DoF ;
  double m_maxChi2DoFVelo ;
  double m_maxChi2DoFT ;
  double m_maxChi2DoFMatchAndTT ;
};

//-----------------------------------------------------------------------------
// Implementation file for class : TrackBestTrackCreator
//
// 2011-12-01 Wouter Hulsbergen
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackBestTrackCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackBestTrackCreator::TrackBestTrackCreator( const std::string& name,
						    ISvcLocator* pSvcLocator)
: GaudiHistoAlg ( name , pSvcLocator ),
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
  declareProperty( "MaxOverlapFracVelo", m_maxOverlapFracVelo = 0.5) ;
  declareProperty( "MaxOverlapFracT", m_maxOverlapFracT = 0.5) ;
  declareProperty( "MaxOverlapFracTT", m_maxOverlapFracTT = 0.35) ; // essentially: max 1 common hit
  declareProperty( "InitTrackStates", m_initTrackStates = true ) ;
  declareProperty( "FitTracks", m_fitTracks = true ) ;
  declareProperty( "StateInitTool", m_stateinittool ) ;
  declareProperty( "Fitter", m_fitter ) ;
  declareProperty( "MaxChi2DoF", m_maxChi2DoF = 3 ) ;
  declareProperty( "MaxChi2DoFVelo", m_maxChi2DoFVelo = 999 ) ;
  declareProperty( "MaxChi2DoFT", m_maxChi2DoFT = 999 ) ;
  declareProperty( "MaxChi2DoFMatchTT", m_maxChi2DoFMatchAndTT = 999 ) ;
  declareProperty( "MinLongLongDeltaQoP", m_minLongLongDeltaQoP = -1 ) ;
  declareProperty( "MinLongDownstreamDeltaQoP", m_minLongDownstreamDeltaQoP = 5e-6 ) ;
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
  //StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  StatusCode sc = GaudiHistoAlg::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_debugLevel = msgLevel( MSG::DEBUG );  
  if( m_debugLevel ) debug() << "==> Initialize" << endmsg;
  
  // ------------------------------
  if( m_initTrackStates ) {
    sc = m_stateinittool.retrieve() ;
    if ( sc.isFailure() ) return sc; 
  }
  if( m_fitTracks ) {
    sc = m_fitter.retrieve() ;
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
  
  return sc ;
}


namespace {
  template <class Container>
  size_t nCommon(const Container& lhs, const Container& rhs) {
    size_t rc(0) ;
    typename Container::const_iterator first1 = lhs.begin() ;
    typename Container::const_iterator last1  = lhs.end() ;
    typename Container::const_iterator first2 = rhs.begin() ;
    typename Container::const_iterator last2  = rhs.end() ;
    while (first1 != last1 && first2 != last2)
      if (*first1 < *first2)
	++first1;
      else if (*first2 < *first1)
	++first2;
      else {
	++first1;
	++first2;	++rc ;
      }
    return rc ;
  }
  
  template <class Container>
  double overlapFraction(const Container& lhs, const Container& rhs) {
    size_t minsize = std::min( lhs.size(), rhs.size()) ;
    return minsize>0 ? nCommon(lhs,rhs) / double(minsize) : 0 ;
  }
}

bool TrackBestTrackCreator::veloClones(const TrackData& lhs,
				       const TrackData& rhs) const
{
  double fR = overlapFraction(lhs.veloRIds, rhs.veloRIds) ;
  double fPhi = overlapFraction(lhs.veloPhiIds,rhs.veloPhiIds) ;
#ifdef DEBUGHISTOGRAMS
  if(fR>0) plot1D(fR,"veloROverlapFractionH1",0,1) ;
  if(fPhi>0) plot1D(fPhi,"veloPhiOverlapFractionH1",0,1) ;
#endif
  return fR>m_maxOverlapFracVelo && fPhi>m_maxOverlapFracVelo ;
}

bool TrackBestTrackCreator::veloOrClones(const TrackData& lhs,
					    const TrackData& rhs) const
{
  double fR = overlapFraction(lhs.veloRIds, rhs.veloRIds) ;
  double fPhi = overlapFraction(lhs.veloPhiIds,rhs.veloPhiIds) ;
  return fR>m_maxOverlapFracVelo || fPhi>m_maxOverlapFracVelo ;
}

bool TrackBestTrackCreator::TClones(const TrackData& lhs,
				       const TrackData& rhs) const
{
  double f = overlapFraction(lhs.TIds,rhs.TIds); 
#ifdef DEBUGHISTOGRAMS
  if(f>0) plot1D(f,"TOverlapFractionH1",0,1) ;
#endif
  return f>m_maxOverlapFracT ;
}

bool TrackBestTrackCreator::TTClones(const TrackData& lhs,
					const TrackData& rhs) const
{
  double f = overlapFraction(lhs.TTIds,rhs.TTIds) ;
#ifdef DEBUGHISTOGRAMS
  if(f>0) plot1D(f,"TTOverlapFractionH1",0,1) ;
#endif
  return f>m_maxOverlapFracTT ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackBestTrackCreator::execute()
{  
  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;

  // create 'TrackData' for all input tracks 
  std::vector<TrackData*> alltracks ;
  BOOST_FOREACH( const std::string& location, m_tracksInContainers) {
    LHCb::Track::Range tracks = get<LHCb::Track::Range>(location);
    alltracks.reserve( alltracks.size() + tracks.size()) ;

    BOOST_FOREACH( const LHCb::Track* itr, tracks) {
      // clone the track
      LHCb::Track* track = itr->clone() ;
      StatusCode sc = StatusCode::SUCCESS ;
      if( m_initTrackStates )
        sc = m_stateinittool->fit(*track,true);
      if( sc.isSuccess() ) 
        alltracks.push_back(new TrackData(track)) ;
      else {
        delete track ;
        Warning("TrackStateInitTool fit failed", sc, 0 ).ignore() ;
      }
    }
  }

  // sort them
  std::stable_sort( alltracks.begin(), alltracks.end(), TrackDataSorter() ) ;

   // loop over all tracks and add them to output container if they
  // pass selector and are not yet there.
  std::vector<TrackData*> selectedtracks ;
  selectedtracks.reserve( alltracks.size() ) ;
  for( std::vector<TrackData*>::const_iterator it = alltracks.begin() ; 
       it != alltracks.end() ; ++it ) {
    // first check that it isn't there yet
    bool found(false) ;
    for( std::vector<TrackData*>::const_iterator jt = selectedtracks.begin() ; 
	 jt != selectedtracks.end() &&!found; ++jt) {
      // rely on the fact that tracks were sorted Long, Downstream, Upstream, Velo, T
      // note: 'clone' can also mean that either of them has no hits in this region

      const LHCb::Track* itrack = (*it)->track() ;
      const LHCb::Track* jtrack = (*jt)->track() ;
      int itype = itrack->type() ;
      int jtype = jtrack->type() ;
      double dqop = (*it)->qOverP()-(*jt)->qOverP() ;
      const int offset = 100 ;
      switch( itype + offset*jtype) {
      case LHCb::Track::Long + offset*LHCb::Track::Long:
#ifdef DEBUGHISTOGRAMS
	if( TClones(**it,**jt) && veloOrClones(**it,**jt) ) {
	  plot( dqop, "LLDqopClones", -1e-5, 1e-5) ;
	} else if( TClones(**it,**jt) ) {
	  plot( dqop, "LLDqopTClones", -1e-5, 1e-5) ;
	} else if( veloOrClones(**it,**jt) ) {
	  plot( dqop, "LLDqopVeloOrClones", -1e-5, 1e-5) ;
	}
#endif
	found = TClones(**it,**jt) && 
	  ( std::abs( dqop ) < m_minLongLongDeltaQoP || veloOrClones(**it,**jt)) ; break;
      case LHCb::Track::Long + offset*LHCb::Track::Downstream:
      case LHCb::Track::Downstream + offset*LHCb::Track::Long:
#ifdef DEBUGHISTOGRAMS
	if( TClones(**it,**jt) ) {
	  plot( dqop, "DLDqop", -2e-5, 2e-5) ;
	  if( TTClones(**it,**jt) ) 
	    plot( dqop, "DLDqopTTClones", -2e-5, 2e-5) ;
	}
#endif
	found = TClones(**it,**jt) && 
	  ( std::abs(dqop) < m_minLongDownstreamDeltaQoP || TTClones(**it,**jt) ); break ;
      case LHCb::Track::Downstream + offset*LHCb::Track::Downstream:
	// it seems that there are no down stream tracks that share T hits ...
#ifdef DEBUGHISTOGRAMS
	if( TClones(**it,**jt) ) {
	  plot( dqop, "DDDqop", -1e-4, 1e-4) ;
	}
#endif
	found = TClones(**it,**jt) && TTClones(**it,**jt) ; break ;
      case LHCb::Track::Long + offset*LHCb::Track::Upstream:
      case LHCb::Track::Upstream + offset*LHCb::Track::Long:
      case LHCb::Track::Upstream + offset*LHCb::Track::Upstream:
	found = veloOrClones(**it,**jt) && TTClones(**it,**jt) ; break ;
      case LHCb::Track::Long + offset*LHCb::Track::Velo:
      case LHCb::Track::Velo + offset*LHCb::Track::Long:
      case LHCb::Track::Upstream + offset*LHCb::Track::Velo:
      case LHCb::Track::Velo + offset*LHCb::Track::Upstream:
      case LHCb::Track::Velo + offset*LHCb::Track::Velo:
	found = veloOrClones(**it,**jt) ; break ;
      case LHCb::Track::Long + offset*LHCb::Track::Ttrack:
      case LHCb::Track::Ttrack + offset*LHCb::Track::Long:
      case LHCb::Track::Downstream + offset*LHCb::Track::Ttrack:
      case LHCb::Track::Ttrack + offset*LHCb::Track::Downstream:
      case LHCb::Track::Ttrack + offset*LHCb::Track::Ttrack:
	found = TClones(**it,**jt) ; break ;
      case LHCb::Track::Ttrack + offset*LHCb::Track::Upstream:
      case LHCb::Track::Upstream + offset*LHCb::Track::Ttrack:
      case LHCb::Track::Ttrack + offset*LHCb::Track::Velo:
      case LHCb::Track::Velo + offset*LHCb::Track::Ttrack:
      case LHCb::Track::Downstream + offset*LHCb::Track::Velo:
      case LHCb::Track::Velo + offset*LHCb::Track::Downstream:
      case LHCb::Track::Downstream + offset*LHCb::Track::Upstream:
      case LHCb::Track::Upstream + offset*LHCb::Track::Downstream:
	found = false; break ;
      default:
	error() << "Don't know how to handle combi: " << itype << " " 
		<< jtype << endmsg ;
      }
    }
    // if it isn't there yet, clone, fit, apply selection
    if(!found) {
      bool success = fitAndSelect(*((*it)->track())) ;
      if( success )
	selectedtracks.push_back( *it ) ;
    }
  }

  // Make the output container
  // -------------------------
  LHCb::Tracks* tracksOutCont = new LHCb::Tracks();
  put( tracksOutCont, m_tracksOutContainer);
  tracksOutCont->reserve( selectedtracks.size());
  for( std::vector<TrackData*>::iterator jt = selectedtracks.begin() ; 
       jt != selectedtracks.end(); ++jt) {
    // transfer ownership of the track
    tracksOutCont->add( (*jt)->release() ) ;
  }
  
  // cleanup
  for( std::vector<TrackData*>::iterator jt = alltracks.begin() ;
       jt != alltracks.end(); ++jt) 
    delete *jt ;

  if(m_debugLevel)
    debug() << "Selected " << tracksOutCont -> size() << " out of "
	    << alltracks.size() << " tracks. " << endmsg ;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Clone a track, fit it, and if it passes the selection, return the
// cloned candidate.
// =============================================================================
bool TrackBestTrackCreator::fitAndSelect(LHCb::Track& track ) const
{
  bool accept = true ;
  if( m_fitTracks ) {
    StatusCode sc = fit( track ) ;
    const LHCb::KalmanFitResult* kalfit = 
      dynamic_cast<const LHCb::KalmanFitResult*>(track.fitResult()) ;
    if(sc.isSuccess() && kalfit ) {
      // only apply the cut to composite tracks
      if( track.type() == LHCb::Track::Long ||
	  track.type() == LHCb::Track::Upstream ||
	  track.type() == LHCb::Track::Downstream ) {
	LHCb::ChiSquare chi2 = kalfit->chi2() ;
	LHCb::ChiSquare chi2T = kalfit->chi2Downstream() ;
	LHCb::ChiSquare chi2Velo = kalfit->chi2Velo() ;
	// note: this includes TT hit contribution
	LHCb::ChiSquare chi2MatchAndTT = chi2 - chi2T - chi2Velo ;
	accept = 
	  chi2.chi2PerDoF() < m_maxChi2DoF &&
	  chi2T.chi2PerDoF() < m_maxChi2DoFT &&
	  chi2Velo.chi2PerDoF() < m_maxChi2DoFVelo &&
	  chi2MatchAndTT.chi2PerDoF() < m_maxChi2DoFMatchAndTT ;
      }
      //   //if(!m_selector.accept( *track ) ) sc = StatusCode::FAILURE ;
    } else {
      accept = false ;
    }
  }
  return accept ;
}

//=============================================================================
//  Fit a track, keeping some counters. This is copied from TrackEventFitter
//=============================================================================
StatusCode TrackBestTrackCreator::fit( LHCb::Track& track ) const
{
  // copied from TrackEventFitter
  StatusCode sc = StatusCode::FAILURE ;
  bool badinput  = false ;
  bool fitfailed = false ;

  if( track.nStates()==0 ||
      track.checkFlag( LHCb::Track::Invalid ) )  {
    track.setFlag( LHCb::Track::Invalid, true );
    badinput = true ;
  } else {
    double qopBefore = track.firstState().qOverP() ;
    sc = m_fitter -> fit( track );
    if ( sc.isSuccess() ) {
      // Update counters
      std::string prefix = Gaudi::Utils::toString(track.type()) ;
      if( track.checkFlag( LHCb::Track::Backward ) ) prefix += "Backward" ;
      prefix += '.' ;
      if( track.nDoF()>0) {
	double chisqprob = track.probChi2() ;
	counter(prefix + "chisqprobSum") += chisqprob ;
	counter(prefix + "badChisq") += bool(chisqprob<0.01) ;
      }
      counter(prefix + "flipCharge") += bool( qopBefore * track.firstState().qOverP() <0) ;
      counter(prefix + "numOutliers") += track.nMeasurementsRemoved() ;
    }
    else {
      track.setFlag( LHCb::Track::Invalid, true );
      fitfailed = true ;
    }
  }
  // stick as close as possible to whatever EventFitter prints
  counter("BadInput")  += int(badinput) ;
  counter("FitFailed") += int(fitfailed) ;

  return sc ;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackBestTrackCreator::finalize()
{
  if( m_debugLevel ) debug() << "==> Finalize" << endmsg;

  if( m_fitTracks ) {
    double perf = (1.0 - counter("FitFailed").mean())*100 ;
    info() << "  Fitting performance   : "
	   << format( " %7.2f %%", perf ) << endmsg;
  }
  
  m_stateinittool.release().ignore() ;
  m_fitter.release().ignore() ;
  return GaudiHistoAlg::finalize();  // must be called after all other actions
}
