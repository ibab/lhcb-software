// Include files 
// -------------
// from STD
#include <algorithm>

#include <LHCbMath/BloomFilter.h>

// TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"

// local
#include "TrackEventCloneKiller.h"
#include "remove_if_partitioned.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackEventCloneKiller
//
// 2006-03-01 : Eduardo Rodrigues
// Based on the clone killer algorithm of Rutger van der Eijk (2002-06-17)
// 2008-04-15 : Adrian Perieanu
// Update for speed and clone rate
// 2014-12-08 : Manuel Schiller
// update for speed, other performance figures largely unchanged
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackEventCloneKiller )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEventCloneKiller::TrackEventCloneKiller( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_cloneFinder("TrackCloneFinder",this)
  , m_debugLevel(false)
{
  // default list of input containers
  m_tracksInContainers.push_back( LHCb::TrackLocation::Forward    );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Match      );
  m_tracksInContainers.push_back( LHCb::TrackLocation::VeloTT     );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Downstream );
  m_tracksInContainers.push_back( LHCb::TrackLocation::Tsa        );

  declareProperty( "TracksInContainers",
                   m_tracksInContainers );
  declareProperty( "TracksOutContainer", 
                   m_tracksOutContainer = LHCb::TrackLocation::Default );
  declareProperty( "IgnoredTrackTypes",
                   m_ignoredTrackTypes );
  declareProperty( "SkipSameContainerTracks",
                   m_skipSameContainerTracks = true );
  declareProperty( "CompareInSameContainerForwardUpstream",
                   m_compareInSameContainerForwardUpstream = false);
  declareProperty( "CloneFinderTool", m_cloneFinder) ;
// In some cases we just want to flag the tracks but not copy them to an output 
// container. The following algorithms can supress clones by checking 
// LHCb::Track::Clone
// default is old setting
  declareProperty( "CopyTracks",
	    	   m_copyTracks = true );
  
}

//=============================================================================
// Destructor
//=============================================================================
TrackEventCloneKiller::~TrackEventCloneKiller() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackEventCloneKiller::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;
  
  m_debugLevel = msgLevel( MSG::DEBUG );
  
  // Retrieve the clone finder tool
  // ------------------------------
  sc = m_cloneFinder.retrieve() ;
  
  if (1 < m_ignoredTrackTypes.size()) {
    // sort and sanitise the list of ignored track types
    std::sort(std::begin(m_ignoredTrackTypes), std::end(m_ignoredTrackTypes));
    m_ignoredTrackTypes.erase(std::unique(
	  std::begin(m_ignoredTrackTypes), std::end(m_ignoredTrackTypes)),
	std::end(m_ignoredTrackTypes));
  }

  // Print out the user-defined settings
  // -----------------------------------
  info() << endmsg
         << "============ TrackEventCloneKiller Settings ===========" << endmsg
         << "TracksInContainers                    : "
         << getProperty( "TracksInContainers" ).toString()                    << endmsg
         << "TrackOutContainer                     : "
         << m_tracksOutContainer                                              << endmsg
         << "IgnoredTrackTypes                     : "
         << getProperty( "IgnoredTrackTypes" ).toString()                     << endmsg
         << "CloneFinderTool                       : "
         << m_cloneFinder.type()                                              << endmsg
         << "SkipSameContainerTracks               : "
         << getProperty( "SkipSameContainerTracks" ).toString()               << endmsg
         << "CompareInSameContainerForwardUpstream : "
         << getProperty( "CompareInSameContainerForwardUpstream" ).toString() << endmsg
         << "=======================================================" << endmsg
         << endmsg;

  return sc ;

}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackEventCloneKiller::finalize()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  m_cloneFinder.release().ignore() ;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackEventCloneKiller::execute()
{
  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;

  // Put all the input tracks into a temporary vector of pointers
  std::vector<LHCb::Track*> allTracks;
  getInputTracks(allTracks);

  const unsigned nTotal = allTracks.size();

  // sort such that longer tracks come first (more likely to be non-clones)
  std::stable_sort(std::begin(allTracks), std::end(allTracks),
      [] (const LHCb::Track* tr1, const LHCb::Track* tr2)
      {
	// first long tracks, then upstream/downstream, then Velo and T, then
	// VeloR tracks, then all the rest
	static constexpr unsigned t2w[] = { 0, 2, 1, 4, 3, 3, 2 };
	static constexpr unsigned t2wlen = sizeof(t2w) / sizeof(t2w[0]);
	const unsigned w1 = (tr1->type() < t2wlen) ? t2w[tr1->type()] : 0;
	const unsigned w2 = (tr2->type() < t2wlen) ? t2w[tr2->type()] : 0;
	if (w1 > w2) return true;
	else if (w2 > w1) return false;
	// in case of a tie based on the track type ordering above, the track
	// with more LHCbIDs goes first
	else return tr1->nLHCbIDs() > tr2->nLHCbIDs();
      } );

  /* strategy:
   *
   * we build up a list of likely non-clone tracks, track by track
   *
   * steps:
   * 1. for each unassessed track, loop over the the list of likely non-clone
   *    tracks, and check if the unassessed track is a clone of the likely
   *    non-clone tracks already present
   *
   * 2. if the unassessed track turns out to be a clone of a track on the
   *    likely non-clone list, discard it
   *
   * 3. else save the track to the likely non-clone list, and proceed to the
   *    next unassessed track
   *
   * the list of likely non-clone tracks is built in place in the allTracks
   * vector by using remove_if_partitioned
   *
   * since the actual check if two tracks are clones is somewhat involved, the
   * isClone predicate below is somewhat involved, trying to avoid the actual
   * test for "cloneness" by various tricks before delegating the hard work to
   * the m_cloneFinder tool
   *
   * for each track put on the likely non-clone list, we save a BloomFilter
   * fingerprinting its hit content, so we can quickly check for
   * non-overlapping tracks
   *
   * the task is further complicated by the fact that we have no strict track
   * quality ordering here, and the m_cloneFinder tool may flag either track as
   * a clone of the other (sorting the tracks as above helps quite a bit in the
   * sense that it's much easier to predict which track is flagged as clone,
   * actually); therefore a final pass over the allTrack array to remove the
   * "out-of-order" clones is required; these should be rare as long as long
   * tracks are fed to the algorithm first, and T (and Velo) tracks are last
   */

  // build a BloomFilter for the hit content of each likely non-clone track
  typedef BloomFilter<LHCb::LHCbID, 64, 154028, 1 << 20> BF;
  std::vector<BF> bloomfilters; // 32 bytes per track, p=14.7%
  bloomfilters.reserve(allTracks.size());

  // check if a track is a clone of one of the tracks on the likely non-clone
  // list (which is in std:begin(allTracks)...doneend)
  const auto isClone = [&allTracks, &bloomfilters, this] (
      LHCb::Track* tr1, decltype(std::begin(allTracks)) doneend)
  {
    const auto isForward = [] (const LHCb::Track* tr)
    { return LHCb::Track::PatForward == tr->history() ||
      LHCb::Track::PrForward == tr->history(); };
    const auto isUpstream = [] (const LHCb::Track* tr)
    { return LHCb::Track::Upstream == tr->type(); };

    const auto parent1(LIKELY(m_skipSameContainerTracks) ? tr1->parent() : nullptr);
    const bool isForwardOrUpstream1 = (isForward(tr1) || isUpstream(tr1));
    const auto& ids1(tr1->lhcbIDs());
    const BF bf1(std::begin(ids1), std::end(ids1));
    auto jt = std::begin(bloomfilters);
    for (auto it = std::begin(allTracks); doneend != it; ++it, ++jt) {
      LHCb::Track* tr2(*it);
      // ignore known clones - don't attempt to find clones of clones, unless
      // they're also clones of good tracks (this can only happen for the
      // "out-of-order clones" mentioned above, any they ought to be unlikely,
      // since the list is sorted by decreasing number of LHCbIDs)
      if (UNLIKELY(tr2->checkFlag(LHCb::Track::Clone))) continue;
      // skip comparison of tracks from the same container, if so desired,
      // subject to restrictions...
      if (UNLIKELY(parent1 == tr2->parent())) {
	// if m_compareInSameContainerForwardUpstream is true, do compare
	// tracks in the Forward and Upstream containers nevertheless
	const bool isForwardOrUpstream2 = (isForward(tr2) || isUpstream(tr2));
	if (!m_compareInSameContainerForwardUpstream ||
	    !(isForwardOrUpstream1 && isForwardOrUpstream2 && 
	     tr1->type() == tr2->type()))
	  continue;
      }
      // skip tracks that do not overlap in terms of hits - they cannot be
      // clones
      const BF& bf2(*jt);
      if (LIKELY((bf1 & bf2).empty())) continue;
      // flag if tr1 and tr2 are clones of each other
      m_cloneFinder->flagClones(*tr2, *tr1);
      // if tr1 has just been flagged as a clone, we can stop
      if (UNLIKELY(tr1->checkFlag(LHCb::Track::Clone))) return true;
    }
    // tr1 is a good candidate to be kept - save its bloomfilter for later use
    bloomfilters.push_back(bf1);
    return false;
  };
  // build up the list of likely non-clone tracks, and remove definite clones
  allTracks.erase(remove_if_partitioned(
	std::begin(allTracks), std::end(allTracks), isClone),
      std::end(allTracks));

  // copy non clone tracks to an output container, if wished
  if (m_copyTracks) { 
    // remove any remaining "out-of-order clones"
    allTracks.erase(std::remove_if(
	  std::begin(allTracks), std::end(allTracks),
	  [] (const LHCb::Track* tr)
	  { return tr->checkFlag(LHCb::Track::Clone);}),
	  std::end(allTracks));
    // count unique (non-clone) tracks
    const unsigned nUnique = allTracks.size();
    // remove ignored track types if so desired
    if (!m_ignoredTrackTypes.empty()) {
      allTracks.erase(std::remove_if(
	    std::begin(allTracks), std::end(allTracks),
	    [this] (const LHCb::Track* tr)
	    { return std::binary_search(
	      std::begin(m_ignoredTrackTypes), std::end(m_ignoredTrackTypes),
	      tr->type()); }),
	  std::end(allTracks));
    }
    const unsigned nFiltered = nUnique - allTracks.size();
    
    LHCb::Tracks* tracksOutCont = new LHCb::Tracks();
    tracksOutCont->reserve(allTracks.size());
    put(tracksOutCont, m_tracksOutContainer);
    std::for_each(std::begin(allTracks), std::end(allTracks),
	[tracksOutCont] (const LHCb::Track* tr)
	{ tracksOutCont->add(tr->clone()); });

    if (m_debugLevel) {
      debug() << "Stored " << allTracks.size() << " tracks, identified " <<
	(nTotal - nUnique) << " clones of which " << nFiltered <<
	" were not stored." << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Retrieve the input tracks from all the user-specified containers
// Note: are only taken into account Valid and Fitted tracks!
//=============================================================================
void TrackEventCloneKiller::getInputTracks(
    std::vector<LHCb::Track*>& allTracks)
{
  { // figure out the capacity that allTracks needs to provide
    size_t maxsz = 0;
    for (const auto& inCont: m_tracksInContainers) {
      auto inTracks = get<LHCb::Track::Range>(inCont);
      maxsz += inTracks.size();
    }
    allTracks.reserve(maxsz);
  }
  for (const auto& inCont: m_tracksInContainers) {
    auto tracks = get<LHCb::Track::Range>(inCont);
    if (m_debugLevel) debug() << "# Tracks in " << inCont
			      << " = " << tracks.size() << endmsg;
    const bool canHaveAncestors = ("LHCb::TrackLocation::Tsa" != inCont);
    // loop over container
    for (auto tr: tracks) {
      // skip invalid tracks up front
      if (UNLIKELY(tr->checkFlag(LHCb::Track::Invalid))) continue;
      // label ancestors as clones
      if (LIKELY(canHaveAncestors)) {
	for (auto anc: tr->ancestors()) {
	  anc->setFlag(LHCb::Track::Clone, true);
	}
      }
      // skip clone tracks
      if (tr->checkFlag(LHCb::Track::Clone)) continue;
      
      allTracks.push_back(const_cast<LHCb::Track*>(tr));
    }
  }
  if (m_debugLevel) debug() << "-> total # of tracks retrieved = "
                              << allTracks.size() << endmsg;
}

