//-----------------------------------------------------------------------------
// Implementation file for class : TrackUniqueSegmentSelectorS20p3, Stripping20p3 version
//
// 2013-12-22 Wouter Hulsbergen
//-----------------------------------------------------------------------------

#include <functional>

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


// Include files
// -------------

// From TrackEvent
//#include "Event/Track.h"

// From PhysEvent
//#include "Event/Particle.h"


class TrackUniqueSegmentSelectorS20p3 : public GaudiTool, virtual public ITrackUniqueSegmentSelector
{
public:
  /// Constructor
  TrackUniqueSegmentSelectorS20p3( const std::string& type, 
			      const std::string& name,
			      const IInterface* parent );
  
  /// Destructor
  virtual ~TrackUniqueSegmentSelectorS20p3( );

  /// Initialize
  virtual StatusCode initialize() ;

  /// Finalize
  virtual StatusCode finalize() ;

  /// flag clones by modifying track Clone flag. note this function /changes/ the tracks!
  virtual StatusCode flag( std::vector<LHCb::Track*>& tracks ) const ;
  
  /// select a subset of tracks with unique velo/T segments
  virtual StatusCode select( const LHCb::Track::Range& tracksin, LHCb::Track::Selection& tracksout ) const ;
  /// overload WARNING ignores track selector
  virtual StatusCode select( const LHCb::Track::Range& tracksin, LHCb::Track::ConstVector& tracksout ) const ;
  
  /// select a subset of particles with unique velo/T segments
  virtual StatusCode select( const LHCb::Particle::Range& tracksin, LHCb::Particle::Selection& tracksout ) const ;
  
 private:
  /// this is the routine that does the actual work.
  StatusCode flagClones(const LHCb::Track::Range& tracks, std::vector<bool>& isclone) const ;
  ToolHandle<ITrackSelector> m_selector ;
  bool m_debugLevel;
};

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TrackUniqueSegmentSelectorS20p3 )

//=============================================================================
// Constructor
//=============================================================================

TrackUniqueSegmentSelectorS20p3::TrackUniqueSegmentSelectorS20p3( const std::string& type,
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
TrackUniqueSegmentSelectorS20p3::~TrackUniqueSegmentSelectorS20p3() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackUniqueSegmentSelectorS20p3::initialize()
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
StatusCode TrackUniqueSegmentSelectorS20p3::finalize()
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

  class TrackData : public LHCb::TrackCloneData<false, false>
  {
  public:
    TrackData(const LHCb::Track* track, size_t index) :
	LHCb::TrackCloneData<false, false>(TrackPtr(track)),
	m_weightFromHits(track->nDoF() - track->chi2()),
	m_index(index),
	m_flaggedByCloneCleaner(track->info(LHCb::Track::CloneDist, -1) > 0)
    {
      // the sorting criteria is based on track type, number of hits
      // and chi2/dof, just like in trackclonecleaner. I am not sure
      // if number of hits is so great criterion: can we somehow give
      // the chi2 a bit more weight?
      static const int weightFromType[10] = {0,11,10,20,15,16,12,2,3} ;
      m_weightFromType = weightFromType[track->type()] ;
    }
    double weightFromHits() const { return m_weightFromHits ; }
    
    size_t index() const { return m_index ; }

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
    double m_weightFromHits ;
    size_t m_index ;
    int m_weightFromType ;
    bool m_flaggedByCloneCleaner ;
  } ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackUniqueSegmentSelectorS20p3::flagClones( const LHCb::Track::Range& tracks,
						  std::vector<bool>& isclone) const
{  
  // initialize all values to false
  size_t N = tracks.size() ;
  isclone.resize(0) ;
  isclone.resize(N, false ) ;
  
  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;
  
  // we don't want dynamic memory allocation as it is expensive.
  std::vector<TrackData> alltracksstore ; alltracksstore.reserve(N) ;
  size_t i(0);
  for (const LHCb::Track* tr: tracks) 
    alltracksstore.push_back(TrackData(tr,i++)) ;
  
  // create vector of pointers and sort it
  std::vector<std::reference_wrapper<TrackData> > alltracks(
	  std::begin(alltracksstore), std::end(alltracksstore));
  std::stable_sort( alltracks.begin(), alltracks.end(), 
	  [] (const TrackData &lhs, const TrackData &rhs)
	  {return lhs < rhs; } ) ;
  
  // loop over all tracks and add them to output container if they
  // pass selector and are not yet there.
  std::vector<std::reference_wrapper<TrackData> > selectedtracks ;
  selectedtracks.reserve( alltracks.size() ) ;
  for( auto it = alltracks.begin() ; it != alltracks.end() ; ++it ) {
    TrackData& itd(*it);
    // std::cout << "processing track: " 
    // 	      << itd.track().type() << " " << itd.lhcbIDs(TrackData::VeloPhi).size() << " " 
    // 	      << itd.lhcbIDs(TrackData::VeloR).size() << " "
    // 	      << itd.weightFromHits() << " "
    // 	      << itd.track().info(LHCb::Track::CloneDist,-1) << std::endl ;
    
    bool found(false) ;
    bool iHasVelo = itd.nHits(TrackData::VeloR) >0 ;
    for( auto jt(selectedtracks.begin()), jend(selectedtracks.end()); 
	 jt != jend &&!found; ++jt) {
      TrackData& jtd(*jt);
      bool jHasVelo = jtd.nHits(TrackData::VeloR) >0 ;

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
	( (nCommonR = itd.nCommon(jtd, TrackData::VeloR )) >=2 &&
	  (nCommonR >=3 || std::min( itd.nHits( TrackData::VeloR ), jtd.nHits( TrackData::VeloR )) < 5 ) ) &&
	( (nCommonPhi = itd.nCommon(jtd, TrackData::VeloPhi )) >=2 &&
	  (nCommonPhi >=3 || std::min( itd.nHits( TrackData::VeloPhi ), jtd.nHits( TrackData::VeloPhi )) < 5 ) ) ;
      
      if( !found &&
	  (itd.overlapFraction(jtd, TrackData::VeloR ) >=0.5 &&
	   itd.overlapFraction(jtd, TrackData::VeloPhi ) >=0.5) ) {
	info() << "Huh? " 
	       << itd.nCommon(jtd, TrackData::VeloR ) << " "
	       << itd.nHits( TrackData::VeloR ) << " " <<  jtd.nHits( TrackData::VeloR ) << " | "
	       << itd.nCommon(jtd, TrackData::VeloPhi ) << " "
	       << itd.nHits( TrackData::VeloPhi ) << " " <<  jtd.nHits( TrackData::VeloPhi ) << endreq ;
      }

      //itd.nCommon(jtd, TrackData::VeloPhi ) >=2) ;
      //(itd.overlapFraction(jtd, TrackData::VeloR ) >=0.5 &&
      //itd.overlapFraction(jtd, TrackData::VeloPhi ) >=0.5) ;
      // if( !foundVelo &&
      // 	  itd.nCommon(jtd, TrackData::VeloR ) >=2 &&
      // 	  itd.nCommon(jtd, TrackData::VeloPhi ) >=2)  {
      // 	std::cout << "*****************************************************************" << std::endl ;
      // 	std::cout << "overlapfractionsL " 
      // 		  << itd.overlapFraction(jtd, TrackData::VeloR ) << " "
      // 		  << itd.overlapFraction(jtd, TrackData::VeloPhi ) << std::endl ;
      // 	std::cout << LHCb::HitPattern( itd.track().lhcbIDs() ) << std::endl ;
      // 	std::cout << LHCb::HitPattern( jtd.track().lhcbIDs() ) << std::endl ;

      // 	const LHCb::Track::LHCbIDContainer& ihits = itd.track().lhcbIDs() ;
      // 	const LHCb::Track::LHCbIDContainer& jhits = jtd.track().lhcbIDs() ;
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
	found  = itd.overlapFraction( jtd, TrackData::T ) > 0.5 ;
      
      if( found && m_debugLevel ) {
	verbose() << "Found clone: "
		  <<  itd.nCommon(jtd, TrackData::VeloPhi ) 
		  << " "
		  << itd.nCommon(jtd, TrackData::VeloR ) 
		  << " j: "
		  << jtd.track().type() << " " << jtd.lhcbIDs(TrackData::VeloPhi).size() << " " << jtd.lhcbIDs(TrackData::VeloR).size() 
		  << " " << jtd.track().info(LHCb::Track::CloneDist,-1) 
		  <<  " i: "
		  << itd.track().type() << " " << itd.lhcbIDs(TrackData::VeloPhi).size() << " " << itd.lhcbIDs(TrackData::VeloR).size()
		  << " " << itd.track().info(LHCb::Track::CloneDist,-1) 
		  << endreq ;
      }
    }
    if(!found) selectedtracks.push_back( itd ) ;
    else {
      isclone[ itd.index() ] = true ;
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

StatusCode TrackUniqueSegmentSelectorS20p3::flag(std::vector<LHCb::Track*>& tracks) const
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

StatusCode TrackUniqueSegmentSelectorS20p3::select(const LHCb::Track::Range& tracks,
					      LHCb::Track::Selection& tracksout ) const
{
  // if the selector is set, we'll need to select those before flagging the clones
  std::vector<const LHCb::Track*> selectedsubset ;
  LHCb::Track::Range selrange = tracks ;
  if( !m_selector.empty() ) {
    selectedsubset.reserve( tracks.size() ) ;
    for (const LHCb::Track* trk: tracks) 
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

// simple overload
StatusCode TrackUniqueSegmentSelectorS20p3::select( const LHCb::Track::Range& tracksin, LHCb::Track::ConstVector& tracksout ) const
{
  std::vector<bool> isclone(tracksin.size(), false);
  StatusCode sc = flagClones(tracksin, isclone);
  int i(0);
  //for( auto itr = tracks.begin(), end = tracks.end() ; itr != end ; ++itr,++i )
  for( LHCb::Track::Range::const_iterator itr(tracksin.begin()),end(tracksin.end()); itr != end; ++itr,++i)
    if( !isclone[i] ) tracksout.push_back( *itr ) ;
  return sc;
}

//=============================================================================
// Choose a subset of particles. Particles without a track will be
// automatically selected. Particles that have a track in common will
// be selected only once. (Only the first is kept.)
// =============================================================================

StatusCode TrackUniqueSegmentSelectorS20p3::select(const LHCb::Particle::Range& particles,
					      LHCb::Particle::Selection& output ) const
{
  // Select the set of tracks
  std::map< const LHCb::Track*, const LHCb::Particle*> track2particle ;
  std::vector<const LHCb::Track*> tracks ;
  const LHCb::Track* trk(0) ;
  for (const LHCb::Particle* itr: particles) {
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
