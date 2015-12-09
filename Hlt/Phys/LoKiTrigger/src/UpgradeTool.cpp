// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <climits>
#include <sstream>
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Incident.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:1572) // non-pointer conversion ... may lose significant bits
#pragma warning(push)
#endif
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algs.h"
#include "LoKi/UpgradeTool.h"
#include "LoKi/Combiner.h"
#include "LoKi/HelperTool.h"
// ============================================================================
// Local
// ============================================================================
#include  "LTTools.h"
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
// ============================================================================
/** @file
 *  Implementation file for class LoKi::Hlt1::UpgradeTool
 *  @date 2008-11-13
 *  @author Vanya BELYAEV
 */
// ============================================================================
// anonymos namespace to hide some local technical details
namespace
{
  // ==========================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) //  function .. was declared but never referenced
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
  // ==========================================================================
  /// move LHCbIDs
  inline void moveIDs ( const LHCb::Track* tr1 , LHCb::Track* tr2 )
  {
    tr2->addSortedToLhcbIDs( tr1->lhcbIDs() );
  }
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}
// ==========================================================================
//                                                end of anonynmous namespace
// ============================================================================
// create the tool from info
// ============================================================================
LoKi::Hlt1::UpgradeTool::UpgradeTool
( const LoKi::Hlt1::UpgradeConf& info )
  : LoKi::Hlt1::HelperTool { 1 }
  , m_config     { info    }
  , m_upgrade    { nullptr }
{
  // retrive the tool
  m_upgrade = alg()->tool<ITracksFromTrack> ( trTool() ) ;
  //
}
// ============================================================================
// create the tool from info
// ============================================================================
LoKi::Hlt1::UpgradeTool::UpgradeTool
( const LoKi::Hlt1::UpgradeConf& info,
  const std::string& comp )
  : LoKi::Hlt1::HelperTool { 1 }
  , m_config     { info            }
  , m_complement { std::move(comp) }
  , m_upgrade    { nullptr         }
{
  // retrive the tool
  m_upgrade = alg()->tool<ITracksFromTrack> ( trTool() ) ;
  //
  /// get the service
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  //
}
// ============================================================================
// upgrade/reconstruct one track
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeTool::reco
( const LHCb::Track*         seed    ,
  LHCb::Track::ConstVector&  tracks  ,
  LHCb::Track::Container*    otracks ) const
{
  typedef std::vector<LHCb::Track*> OUTPUT ;
  //
  OUTPUT out ;
  StatusCode sc = m_upgrade->tracksFromTrack ( *seed , out ) ;
  if ( sc.isFailure() )
  { return Error ( "Error from ITrackFromTrack tool '" + m_upgrade->name()+  "'" , sc, 0 ) ; }
  // post action:
  for ( LHCb::Track *trk : out )
  {
    if ( owner    () ) { trk->setType ( m_config.trType () )     ; }
    if ( moveIDs  () ) { ::moveIDs ( seed , trk )                ; }
    if ( moveAncs () ) { trk->addToAncestors ( seed )            ; }
    if ( moveInfo () ) { trk->setExtraInfo ( seed->extraInfo() ) ; }
  }
  //
  // insert only "good" tracks into the stream
  //
  std::copy_if( std::begin(out) , std::end(out) ,
                std::back_inserter ( tracks ) , m_config ) ;
  //
  for ( const auto& track : out )
  {
    /// invalid or already registered track
    if ( !track || track->parent() != 0 ) { continue ; }
    /// force TES-registration if not done yet
    if ( !owner() ) { Warning ( "reco(): misconfiguration of 'Owner'-property! ignore" ); }
    if ( !otracks )
    { return Error ("reco(): LHCb::Track::Container* points to NULL!") ; }
     // finally: register
    otracks->insert ( track ) ;
  }
  //
  return StatusCode::SUCCESS ;                                       // RETURN
}
// ============================================================================
// upgrade set of tracks
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeTool::upgrade
( const LHCb::Track::ConstVector&  itracks ,
  LHCb::Track::ConstVector&        output  ) const
{
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( alg()         , "GaudiAlgorithm*   points to NULL!" ) ;

  // get or create the output
  LHCb::Track::Container* otracks = storedTracks( address() ) ;

  // ==========================================================================
  // loop over input tracks, upgrade track-by-track
  // ==========================================================================
  for (const LHCb::Track* seed : itracks )
  {
    if ( !seed ) { continue ; }                                  // CONTINUE
    StatusCode sc = iupgrade ( seed , output , otracks ) ;
    if ( sc.isFailure () )
    { Error ( "Error from iupgrade, skip track", sc, 0 ) ; continue ; }
    // ========================================================================
  } // end of the loop over input tracks
  // ==========================================================================
  // sort tracks ?
  if ( ptOrder() )
  { std::stable_sort ( std::begin(output) , std::end(output) , Hlt::SortTrackByPt() ) ; }
  //
  return StatusCode::SUCCESS ;                                        // RETURN
}
// ============================================================================
// upgrade one seed
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeTool::upgrade
( const LHCb::Track*         itrack ,
  LHCb::Track::ConstVector&  output ) const
{
  //
  if ( !itrack )
  { return Error ( "Upgrade: LHCb::Track* points to NULL" ) ; }
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( alg()         , "GaudiAlgorithm*   points to NULL!" ) ;
  //
  // get or create the output
  LHCb::Track::Container* otracks = storedTracks ( address() ) ;
  //
  StatusCode sc = iupgrade ( itrack , output , otracks ) ;
  if ( sc.isFailure() ) { return Error ( "upgrade: error from iupgrade" , sc, 0 ) ; }
  //
  // sort tracks ?
  if ( ptOrder() )
  { std::stable_sort ( std::begin(output) , std::end(output) , Hlt::SortTrackByPt() ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// upgrade one seed
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeTool::iupgrade
( const LHCb::Track*         seed    ,
  LHCb::Track::ConstVector&  output  ,
  LHCb::Track::Container*    otracks ) const
{
   // Simplify code with a lambda
   using Cache = LHCb::Relation1D<LHCb::Track, LHCb::Track>;
   auto checkCache = [this, &output](const LHCb::Track* seed, Cache* cache) {
      auto results = cache->i_relations(seed);
      if ( results.empty() ) {
         return std::make_pair(false, 0ul);
      } else if ( results.size() == 1 && !results.front().to() ) {
         return std::make_pair(true, 0ul);
      } else {
         if (owner()) {
            // If tracks are owned, we pass the results on.
            for (const auto& rel : results)
               output.push_back(rel.to());
         } else {
            // Else the seed is passed on. This allows passthrough of
            // candidates in the case of filtering before complement
            // upgrade.
            output.push_back(seed);
         }
         return std::make_pair(true, results.size());
      }
   };

   // Check for complement upgrade
   if (!complement().empty()) {
      auto cached = cache<LHCb::Track>(complement());
      auto result = checkCache(seed, cached);
      // Previous upgrade with result.
      if (result.second) {
         return  StatusCode::SUCCESS;
      }
   }

   // Check for previous upgrade.
   auto cached = cache<LHCb::Track>(config().cache());
   if (!cached) {
      return Error(std::string("No cache to check for ") + trTool(), StatusCode::FAILURE, 10);
   }
   auto result = checkCache(seed, cached);
   if (result.first) {
      // Previous upgrade, failed or successful
      return StatusCode::SUCCESS ;
   } else {
      // No previous upgrade, perform it.
      StatusCode sc = reco(seed, output, otracks);
      if ( sc.isFailure () ) {
         return Error ( "Failure from ITrackFromTrack tool, skip track", sc, 0 ) ;
      } else if (output.empty()) {
         // Upgrade failed.
         cached->i_relate(seed, nullptr);
      } else {
         // Successful upgrade; store in cache
         for (auto track : output) {
            cached->i_relate(seed, track);
         }
      }
      return StatusCode::SUCCESS ;
   }
}
// ======================================================================
/*  upgrade the 'track'candidates
 *  @param itrack  input track/seeds
 *  @param otracks output container of upgraded tracks (cumulative)
 *  @return status code
 */
// ======================================================================
StatusCode LoKi::Hlt1::UpgradeTool::upgradeTracks
( const Hlt::Candidate::ConstVector& input  ,
  Hlt::Candidate::ConstVector&       output ) const
{
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != alg()    , "GaudiAlgorithm*   points to NULL!" ) ;
  //
  LHCb::Track::Container*     otracks = storedTracks ( address () ) ;
  //
  // loop over input candidates, upgrade one-by-one
  for ( const Hlt::Candidate* candidate : input ) {
    if ( !candidate )
    { Error ( "Invalid Hlt::Candidate, skip it!") ; continue ; } // CONTINUE
    const Hlt::Stage*     stage  = candidate->currentStage() ;
    if ( !stage     )
    { Error ( "Invalid Hlt::Stage,     skip it!") ; continue ; } // CONTINUE
    //
    if ( !stage->is<LHCb::Track> () )
    { Error ( "No      LHCb::Track,    skip it!") ; continue ; } // CONTINUE
    //
    // upgrade single track
    StatusCode sc = _i_upgrade_1track
      ( candidate   ,
        output      ,
        otracks     ) ;
    //
    if ( sc.isFailure() ) { Error ( "Error from i_upgrade_1track", sc, 0 ) ; }
    //
  } //
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  upgrade the candidate
 *  @param itrack  input track/seeds
 *  @param otracks output container of upgraded tracks (cumulative)
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeTool::_i_upgrade_1track
( const Hlt::Candidate*              input      ,
  Hlt::Candidate::ConstVector&       output     ,
  LHCb::Track::Container*            otracks    ) const
{
  //
  if ( ! input ) { return Error ( "Hlt::Candidate* points to NULL") ; }
  //
  const Hlt::Stage* stage = input->currentStage() ;
  if ( ! stage || !stage->is<LHCb::Track> () )
  { return Error ( "Invalid Hlt::Stage*" ) ; }
  //
  const LHCb::Track*    seed = stage->get<LHCb::Track> () ;
  //
  LHCb::Track::ConstVector out ;
  StatusCode sc = iupgrade ( seed , out , otracks ) ;              // USE TOOL
  if ( sc.isFailure () )
  { return Error ( "Error from iupgrade, skip track", sc, 0 ) ; }    // RETURN
  //
  // Process output tracks, create a new candidate for tracks beyond the first
  for ( LHCb::Track::ConstVector::const_iterator iout = std::begin(out) ;
        std::end(out) != iout ; ++iout ) {
     const LHCb::Track* track = *iout ;
     if ( !track ) continue ;
     //
     Hlt::Candidate* _input = 0;
     if ( iout == out.begin() ) {
        _input = const_cast<Hlt::Candidate*>  ( input ) ;
     } else {
        //
        // start new Candidate:
        _input = newCandidate() ;
        _input -> addToWorkers ( alg() ) ;
        //
        // the initiator of new candidate is the stage of the initial candidate:
        Hlt::Stage* stage1 = newStage() ;
        _input -> addToStages ( stage1 ) ;
        //
        Hlt::Stage::Lock lock1 ( stage1, upgradeTool() ) ;
        lock1.addToHistory ( input->workers() ) ;
        // lock1.addToHistory ( this ) ;
        stage1 -> set ( stage ) ; // add stage into stage as initiator
     }
     if ( track != seed ) {
        // Add the new stage to the candidate
        Hlt::Stage* newstage = newStage() ;
        //
        Hlt::Stage::Lock lock ( newstage , upgradeTool() ) ;
        newstage -> set( track ) ; // add track to the stage
        _input   -> addToStages ( newstage ) ;
     }
     _input   -> addToWorkers ( alg() ) ;
     output.push_back ( _input ) ;                                 // OUTPUT++
  }  //                                     end of the loop over output tracks
  return StatusCode::SUCCESS ;
}
// ============================================================================
std::string Gaudi::Utils::toCpp ( const LoKi::Hlt1::UpgradeTool& o )
{ std::string s = " LoKi::Hlt1::UpgradeTool( " + toCpp ( o.config () );
   if ( !o.complement().empty() ) {
      s += ", " + toCpp( o.complement() );
   }
   return s + ") ";
}

// ============================================================================
// The END
// ============================================================================
