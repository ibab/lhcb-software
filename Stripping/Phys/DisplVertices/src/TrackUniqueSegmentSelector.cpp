//-----------------------------------------------------------------------------
// Implementation file for class : TrackUniqueSegmentSelector
//
// 2013-12-22 Wouter Hulsbergen
//-----------------------------------------------------------------------------

// Interface class
#include "Kernel/ITrackUniqueSegmentSelector.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiTool.h"

// TrackEvent
//#include "Kernel/HitPattern.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackSelector.h"

// TrackKernel
#include "TrackKernel/TrackCloneData.h"

// from BOOST
#include "boost/foreach.hpp"


// Include files
// -------------

// From TrackEvent
//#include "Event/Track.h"

// From PhysEvent
//#include "Event/Particle.h"


class TrackUniqueSegmentSelector : public GaudiTool, virtual public ITrackUniqueSegmentSelector
{
public:
  /// Constructor
  TrackUniqueSegmentSelector( const std::string& type, 
			      const std::string& name,
			      const IInterface* parent );
  
  /// Destructor
  virtual ~TrackUniqueSegmentSelector( );

  /// Initialize
  virtual StatusCode initialize() ;

  /// Finalize
  virtual StatusCode finalize() ;

  /// flag clones by modifying track Clone flag. note this function /changes/ the tracks!
  virtual StatusCode flag( std::vector<LHCb::Track*>& tracks ) const ;
  
  /// select a subset of tracks with unique velo/T segments
  virtual StatusCode select( const LHCb::Track::Range& tracksin, LHCb::Track::Selection& tracksout ) const ;
  
  /// select a subset of particles with unique velo/T segments
  virtual StatusCode select( const LHCb::Particle::Range& tracksin, LHCb::Particle::Selection& tracksout ) const ;
  
 private:
  /// this is the routine that does the actual work.
  StatusCode flagClones(const LHCb::Track::Range& tracks, std::vector<bool>& isclone) const ;
  ToolHandle<ITrackSelector> m_selector ;
  bool m_debugLevel;
};

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TrackUniqueSegmentSelector )

//=============================================================================
// Constructor
//=============================================================================

TrackUniqueSegmentSelector::TrackUniqueSegmentSelector( const std::string& type,
						  const std::string& name,
						  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
  m_selector("",this)
{
  declareInterface<ITrackUniqueSegmentSelector>(this) ;
  declareProperty( "TrackSelector", m_selector ) ;
  m_debugLevel = false ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackUniqueSegmentSelector::~TrackUniqueSegmentSelector() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackUniqueSegmentSelector::initialize()
{
  //StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_debugLevel = msgLevel( MSG::DEBUG );  
  if( m_debugLevel ) debug() << "==> Initialize" << endmsg;
  
  // ------------------------------
  if(!m_selector.empty()) sc = m_selector.retrieve() ;
  if ( sc.isFailure() ) return sc; 
  
  return sc ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackUniqueSegmentSelector::finalize()
{
  if( m_debugLevel ) debug() << "==> Finalize" << endmsg;
  if(!m_selector.empty()) m_selector.release().ignore() ;
  return GaudiTool::finalize();  // must be called after all other actions
}


namespace {

  // void printvelohits( const std::vector<LHCb::LHCbID>& hits)
  // {
  //   std::cout << "velo hits: " ;
  //   for( std::vector<LHCb::LHCbID>::const_iterator it = hits.begin() ;
  // 	 it!= hits.end(); ++it)
  //     if( it->isVelo() )
  // 	std::cout << it->veloID().channelID()  << " " ;
  //   std::cout << std::endl ;
  // }

  /// Helper class to cache information about tracks needed to check
  /// that they are clones. This is now derived from TrackCloneData,
  /// but since we only use velo hits, it may be better to just copy
  /// what we need.

  class TrackData : public LHCb::TrackCloneData 
  {
  public:
    TrackData(const LHCb::Track& track) : LHCb::TrackCloneData(track)
    {
      // the sorting criteria is based on track type, number of hits
      // and chi2/dof, just like in trackclonecleaner. I am not sure
      // if number of hits is so great criterion: can we somehow give
      // the chi2 a bit more weight?
      
      // these lines copied from base class. 
      int weightFromType[10] = {0,11,10,20,15,16,12,2,3} ;
      m_weightFromType = weightFromType[track.type()] ;
      m_weightFromHits = track.nDoF() - track.chi2() ;
      m_flaggedByCloneCleaner = track.info(LHCb::Track::CloneDist,-1) > 0 ? 1 : 0 ;
    }
    double weightFromHits() const { return m_weightFromHits ; }

    // this routine should actually be in base class
    size_t nHits( HitType type ) const { return lhcbIDs(type).size() ; }

    // This routine determines the order of tracks. Of two tracks that
    // are clones, only the first one is selected. We give a
    // preference to longer tracks. Furthermore, in order to ease
    // comparison with the standard clone cleaner, we rank tracks with
    // clone dist flag lower.
    bool operator<(const TrackData& rhs) const {
      return m_weightFromType > rhs.m_weightFromType ||
	(m_weightFromType == rhs.m_weightFromType &&
	 (m_flaggedByCloneCleaner < rhs.m_flaggedByCloneCleaner ||
	  ( m_flaggedByCloneCleaner == rhs.m_flaggedByCloneCleaner &&
	    m_weightFromHits > rhs.m_weightFromHits ) ) ) ;
	  }

    // void intersection( const LHCb::TrackCloneData& rhs, HitType type,
    // 		       LHCbIDs& commonhits ) const {
    //   const LHCbIDs& lhshits = lhcbIDs(type) ;
    //   const LHCbIDs& rhshits = rhs.lhcbIDs(type) ;
    //   commonhits.reserve( std::min(rhshits.size(),lhshits.size() ) ) ;
    //   commonhits.clear() ;
    //   LHCbIDs::const_iterator it = std::set_intersection( lhshits.begin(), lhshits.end(), rhshits.begin(), rhshits.end(), commonhits.begin() ) ;
    //   commonhits.resize(it - commonhits.begin() ) ;
    // }
    
  private:
    int m_weightFromType ;
    int m_flaggedByCloneCleaner ;
    double m_weightFromHits ;
  } ;

  struct TrackDataSorter {
    bool operator()(const TrackData* lhs,
		    const TrackData* rhs) {
      return *lhs < *rhs ;
    }
  } ;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackUniqueSegmentSelector::flagClones(const LHCb::Track::Range& tracks,
						  std::vector<bool>& isclone) const
{  
  // initialize all values to false
  isclone.resize(0) ;
  isclone.resize( tracks.size(), false ) ;


  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;
  
  // we don't want dynamic memory allocation as it is expensive.
  std::vector<TrackData> alltracksstore ;
  alltracksstore.reserve( tracks.size() ) ;
  for( LHCb::Track::Range::const_iterator itr = tracks.begin() ;
       itr != tracks.end(); ++itr )
    alltracksstore.push_back( TrackData(**itr) );
  
  // create vector of pointers and sort it
  int N = alltracksstore.size() ;
  std::vector<TrackData*> alltracks(N) ;
  for(int i=0; i<N; ++i) alltracks[i] = &(alltracksstore[i]) ;
  std::stable_sort( alltracks.begin(), alltracks.end(), TrackDataSorter() ) ;
  
  // loop over all tracks and add them to output container if they
  // pass selector and are not yet there.
  std::vector<TrackData*> selectedtracks ;
  selectedtracks.reserve( alltracks.size() ) ;
  for( std::vector<TrackData*>::const_iterator it = alltracks.begin() ; 
       it != alltracks.end() ; ++it ) {
    // if( m_debugLevel)
    //   verbose() << "processing track: " 
    //   		<< (*it)->track().type() << " " << (*it)->lhcbIDs(TrackData::VeloPhi).size() << " " 
    //  		<< (*it)->lhcbIDs(TrackData::VeloR).size() << " "
    //  		<< (*it)->weightFromHits() << " "
    //   		<< (*it)->track().info(LHCb::Track::CloneDist,-1) << endreq ;
    bool found(false) ;
    bool iHasVelo = (*it)->nHits(TrackData::VeloR) >0 ;
    for( std::vector<TrackData*>::const_iterator jt(selectedtracks.begin()), jend(selectedtracks.end()); 
	 jt != jend &&!found; ++jt) {
      bool jHasVelo = (*jt)->nHits(TrackData::VeloR) >0 ;

      // our main goal is two remove velo segments that have been used
      // more than once to create different long tracks.  however, we
      // also see track finding errors in the velo tracking: often
      // there are tracks that share most phi hits and a few R
      // hits. the minimal criteria to catch these all is two require
      // two R and two phi hits overlap since that essentially defines
      // a straight line. however, this looks a bit too inclusive. so
      // in the end we require 2 hits for tracks with 3 or 4 hits, and
      // 3 or more for tracks with more hits.

      //computing how many hits are in common, is expensive. that's why this looks a bit cumbersome.
      size_t nCommonR, nCommonPhi ;
      found = iHasVelo && jHasVelo &&
	( (nCommonR = (*it)->nCommon(**jt, TrackData::VeloR )) >=2 &&
	  (nCommonR >=3 || std::min( (*it)->nHits( TrackData::VeloR ), (*jt)->nHits( TrackData::VeloR )) < 5 ) ) &&
	( (nCommonPhi = (*it)->nCommon(**jt, TrackData::VeloPhi )) >=2 &&
	  (nCommonPhi >=3 || std::min( (*it)->nHits( TrackData::VeloPhi ), (*jt)->nHits( TrackData::VeloPhi )) < 5 ) ) ;
      
      if( !found &&
	  ((*it)->overlapFraction(**jt, TrackData::VeloR ) >=0.5 &&
	   (*it)->overlapFraction(**jt, TrackData::VeloPhi ) >=0.5) ) {
	info() << "Huh? " 
	       << (*it)->nCommon(**jt, TrackData::VeloR ) << " "
	       << (*it)->nHits( TrackData::VeloR ) << " " <<  (*jt)->nHits( TrackData::VeloR ) << " | "
	       << (*it)->nCommon(**jt, TrackData::VeloPhi ) << " "
	       << (*it)->nHits( TrackData::VeloPhi ) << " " <<  (*jt)->nHits( TrackData::VeloPhi ) << endreq ;
      }

      //(*it)->nCommon(**jt, TrackData::VeloPhi ) >=2) ;
      //((*it)->overlapFraction(**jt, TrackData::VeloR ) >=0.5 &&
      //(*it)->overlapFraction(**jt, TrackData::VeloPhi ) >=0.5) ;
      // if( !foundVelo &&
      // 	  (*it)->nCommon(**jt, TrackData::VeloR ) >=2 &&
      // 	  (*it)->nCommon(**jt, TrackData::VeloPhi ) >=2)  {
      // 	std::cout << "*****************************************************************" << std::endl ;
      // 	std::cout << "overlapfractionsL " 
      // 		  << (*it)->overlapFraction(**jt, TrackData::VeloR ) << " "
      // 		  << (*it)->overlapFraction(**jt, TrackData::VeloPhi ) << std::endl ;
      // 	std::cout << LHCb::HitPattern( (*it)->track().lhcbIDs() ) << std::endl ;
      // 	std::cout << LHCb::HitPattern( (*jt)->track().lhcbIDs() ) << std::endl ;

      // 	const LHCb::Track::LHCbIDContainer& ihits = (*it)->track().lhcbIDs() ;
      // 	const LHCb::Track::LHCbIDContainer& jhits = (*jt)->track().lhcbIDs() ;
      // 	printvelohits( ihits ) ;
      // 	printvelohits( jhits ) ;
      // 	LHCb::Track::LHCbIDContainer commonhits(std::max(ihits.size(),jhits.size() ) ) ;
      // 	LHCb::Track::LHCbIDContainer::const_iterator it = std::set_intersection( ihits.begin(), ihits.end(), jhits.begin(), jhits.end(), commonhits.begin() ) ;
      // 	commonhits.resize(it - commonhits.begin() ) ;
      // 	printvelohits( commonhits ) ;
      // 	std::cout << "Common hits: " << commonhits.size() << std::endl ;
      // 	std::cout << LHCb::HitPattern( commonhits ) << std::endl ;
      // }
      
      // For T tracks we just use '>50%' overlap.  After some
      // deliberation, decided that we don't check for T if tracks are
      // really different in the Velo. Otherwise we just loose too
      // many good velo segments.
      if( !found && !iHasVelo )
	found  = (*it)->overlapFraction( **jt, TrackData::T ) > 0.5 ;
      
      if( found && m_debugLevel ) {
	verbose() << "Found clone: "
		  <<  (*it)->nCommon(**jt, TrackData::VeloPhi ) 
		  << " "
		  << (*it)->nCommon(**jt, TrackData::VeloR ) 
		  << " j: "
		  << (*jt)->track().type() << " " << (*jt)->lhcbIDs(TrackData::VeloPhi).size() << " " << (*jt)->lhcbIDs(TrackData::VeloR).size() 
		  << " " << (*jt)->track().info(LHCb::Track::CloneDist,-1) 
		  <<  " i: "
		  << (*it)->track().type() << " " <<  (*it)->lhcbIDs(TrackData::VeloPhi).size() << " " << (*it)->lhcbIDs(TrackData::VeloR).size()
		  << " " << (*it)->track().info(LHCb::Track::CloneDist,-1) 
		  << endreq ;
      }
    }
    if(!found) selectedtracks.push_back( *it ) ;
    else {
      isclone[ it - alltracks.begin() ] = true ;
    }
  }
  
  if(m_debugLevel)
    debug() << "Selected " 
	    << selectedtracks.size() << " out of "
	    << alltracks.size() << endmsg ;
  return StatusCode::SUCCESS;
}


//=============================================================================
// Flag tracks by setting their 'Clone' flag
//=============================================================================

namespace {
  std::vector<const LHCb::Track*>::iterator
  itercastfunc( std::vector<LHCb::Track*>::iterator itr)
  {
    typedef std::vector<LHCb::Track*>::iterator Iterator1 ;
    typedef std::vector<const LHCb::Track*>::iterator Iterator2 ;
    union _IteratorCast
    {
      const Iterator1* input  ;
      const Iterator2* output ;
    } ;
    // somehow, volatile didn't work here in gcc46
    static _IteratorCast i;
    i.input  =&itr ;
    return *i.output ;
  }
}

StatusCode TrackUniqueSegmentSelector::flag(std::vector<LHCb::Track*>& tracks) const
{
  std::vector<bool> isclone ;
  // the usual mess with casting from non-const vector to a
  // range. this is what Vanya invented (see Track.cpp)
  StatusCode sc = flagClones( LHCb::Track::Range(itercastfunc(tracks.begin()),itercastfunc(tracks.end())), isclone ) ;
  int N = tracks.size() ;
  for( int i=0; i<N; ++i) tracks[i]->setFlag( LHCb::Track::Clone, isclone[i] ) ; 
  
  return sc ;
}

//=============================================================================
// Choose a subset of tracks
//=============================================================================

StatusCode TrackUniqueSegmentSelector::select(const LHCb::Track::Range& tracks,
					      LHCb::Track::Selection& tracksout ) const
{
  // if the selector is set, we'll need to select those before flagging the clones
  std::vector<const LHCb::Track*> selectedsubset ;
  LHCb::Track::Range selrange = tracks ;
  if( !m_selector.empty() ) {
    selectedsubset.reserve( tracks.size() ) ;
    BOOST_FOREACH( const LHCb::Track* trk, tracks ) 
      if( m_selector->accept(*trk) ) 
	selectedsubset.push_back( trk ) ;
    selrange = LHCb::Track::Range( selectedsubset ) ;
  }
  std::vector<bool> isclone ;
  StatusCode sc = flagClones(selrange, isclone ) ;
  int i(0) ;
  //for( auto itr = tracks.begin(), end = tracks.end() ; itr != end ; ++itr,++i )
  for( LHCb::Track::Range::const_iterator itr(selrange.begin()),end(selrange.end()); itr != end; ++itr,++i)
    if( !isclone[i] ) tracksout.push_back( *itr ) ;
  return sc; 
}

//=============================================================================
// Choose a subset of particles. Particles without a track will be
// automatically selected. Particles that have a track in common will
// be selected only once. (Only the first is kept.)
// =============================================================================

StatusCode TrackUniqueSegmentSelector::select(const LHCb::Particle::Range& particles,
					      LHCb::Particle::Selection& output ) const
{
  // Select the set of tracks
  std::map< const LHCb::Track*, const LHCb::Particle*> track2particle ;
  std::vector<const LHCb::Track*> tracks ;
  const LHCb::Track* trk(0) ;
  BOOST_FOREACH( const LHCb::Particle* itr, particles) {
    if( itr->proto() && (trk =itr->proto()->track()) ) {
      // make sure this track isn't yet there
      if( track2particle.find( trk ) == track2particle.end() ) {
	// insert it in the map
	track2particle[trk] = itr ;
	tracks.push_back(trk) ;
      }
    } else {
      output.push_back( itr ) ;
    }
  }
  
  // Select the tracks that are not clones
  LHCb::Track::Selection selectedtracks ;
  StatusCode sc = select( tracks, selectedtracks ) ;
  
  // Fill tracks in the output container
  for( LHCb::Track::Selection::const_iterator jt = selectedtracks.begin() ; 
       jt != selectedtracks.end(); ++jt) 
    output.push_back( track2particle[ (*jt) ] ) ; 
  return sc ;
}
