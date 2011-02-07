// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <climits>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Incident.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
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
// ============================================================================
/** @file
 *  Implementation file for class LoKi::Hlt1::UpgradeTool
 *  @date 2008-11-13 
 *  @author Vanya BELYAEV
 */
// ============================================================================
// create the tool from info 
// ============================================================================
LoKi::Hlt1::UpgradeTool::UpgradeTool
( const LoKi::Hlt1::UpgradeConf& info ) 
  : LoKi::Hlt1::HelperTool ( 1 ) 
  , m_config  ( info ) 
  , m_recoID  ( -1   ) 
  , m_upgrade (      ) 
//
{
  // retrive the tool 
  m_upgrade = alg()->tool<ITracksFromTrack> ( trTool() ) ;
  //
  /// get the service 
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  //
  boost::optional<IANNSvc::minor_value_type> _info = ann->value("InfoID" , trTool()  );
  Assert( _info , " request for unknown Info ID : " + trTool() );
  //
  m_recoID = _info->second ;
  //
}
// ============================================================================
// anonymos namespace to hide some local tchnical details 
namespace
{
  // ==========================================================================
  /// move LHCbIDs
  inline void moveIDs ( const LHCb::Track* tr1 , LHCb::Track* tr2 )
  {
    typedef std::vector<LHCb::LHCbID> LHCbIDs ;
    const LHCbIDs& ids = tr1->lhcbIDs() ;
    for ( LHCbIDs::const_iterator iID = ids.begin() ; ids.end() != iID ; ++iID )
    { tr2->addToLhcbIDs ( *iID ) ; }
  }  
  // ==========================================================================
  /// move LHCbIDs
  inline void moveIDs ( const LHCb::Track* tr1 , std::vector<LHCb::Track*>& tracks )
  {
    for ( std::vector<LHCb::Track*>::const_iterator itr = 
            tracks.begin() ; tracks.end() != itr ; ++itr ) { moveIDs ( tr1 , *itr ) ; }
  } 
  // ==========================================================================
  /** is ancestor  ?
   *  simple structure to check the ancestors 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-11-13
   */
  struct IsAncestor : public std::unary_function<const LHCb::Track*,bool>
  {
    // ========================================================================
    /// constructor form the seed 
    IsAncestor ( const LHCb::Track* seed ) : m_seed ( seed ) {}
    // ========================================================================
    bool operator() ( const LHCb::Track* track ) 
    { 
      if ( 0 == track ) { return false ; }
      typedef SmartRefVector<LHCb::Track> Ancestors ;
      const Ancestors& ancestors = track->ancestors() ;
      return ancestors.end() != std::find_if
        ( ancestors . begin () , 
          ancestors . end   () , 
          std::bind2nd( std::equal_to<const LHCb::Track*>() , m_seed ) )  ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    IsAncestor() ;                       // the defautl constructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the seed 
    const LHCb::Track* m_seed ;
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                end of anonynmous namespace
// ============================================================================
// find the tracks within the recontructed 
// ============================================================================
size_t LoKi::Hlt1::UpgradeTool::find 
( const LHCb::Track*         seed    , 
  LHCb::Track::ConstVector&  tracks  ,
  LHCb::Track::Container*    otracks ) const 
{
  //
  const size_t ntracks = tracks.size() ;
  if ( 0 == otracks ) 
  { 
    Error ("find(): LHCb::Track::Container* points to NULL!") ;
    return 0 ;
  }
  LoKi::Algs::copy_if 
    ( otracks -> begin () , 
      otracks -> end   () , 
      std::back_inserter ( tracks ) , IsAncestor ( seed ) ) ;
  // return 
  return tracks.size() - ntracks ;
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
  { return Error ( "Error from ITrackFromTrack tool '" + m_upgrade->name()+  "'" , sc ) ; }
  // post action:
  for ( OUTPUT::iterator iout = out.begin() ; out.end() != iout ; ++iout ) 
  {
    LHCb::Track* trk = *iout ;
    trk->setType ( m_config.trType () ) ;
    if ( moveIDs  () ) { ::moveIDs ( seed , trk )                ; }
    if ( moveAncs () ) { trk->addToAncestors ( seed )            ; }
    if ( moveInfo () ) { trk->setExtraInfo ( seed->extraInfo() ) ; }
    trk->addInfo ( recoID() , seed->key() ) ; 
  }
  //
  // insert only "good" tracks into the stream 
  //
  LoKi::Algs::copy_if  
    ( out.begin () , 
      out.end   () , 
      std::back_inserter ( tracks ) ,
      m_config     ) ;
  //
  if ( owner() ) // register all tracks in TES
  {
    if ( 0 == otracks ) 
    { return Error ("reco(): LHCb::Track::Container* pointns to NULL!") ; }
    for ( OUTPUT::const_iterator itr = out.begin() ; out.end() != itr ; ++itr ) 
    { otracks->insert ( *itr ) ; }  
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
  Assert ( 0 != alg()    , "GaudiAlgorithm*   points to NULL!" ) ;
  
  // get or create the output
  LHCb::Track::Container* otracks = storedTracks( address() ) ;
  
  // ==========================================================================
  // loop over input tracks, upgrade track-by-track 
  // ==========================================================================
  typedef LHCb::Track::ConstVector  OUTPUT ;
  for ( OUTPUT::const_iterator iseed = itracks.begin() ;
        itracks.end() != iseed ; ++iseed ) 
  {
    const LHCb::Track* seed = *iseed ;
    if ( 0 == seed ) { continue ; }                                  // CONTINUE 
    StatusCode sc = iupgrade ( seed , output , otracks ) ;
    if ( sc.isFailure () ) 
    { Error ( "Error from iupgrade, skip track", sc ) ; continue ; } // CONTINUE 
    // ========================================================================
  } // end of the loop over input tracks 
  // ==========================================================================
  // sort tracks ?
  if ( ptOrder() ) 
  { std::stable_sort ( output.begin () , output.end () , Hlt::SortTrackByPt() ) ; }
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
  if ( 0 == itrack ) 
  { return Error ( "Upgrade: LHCb::Track* points to NULL" ) ; }
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != alg()    , "GaudiAlgorithm*   points to NULL!" ) ;
  //
  // get or create the output
  LHCb::Track::Container* otracks = storedTracks ( address() ) ;
  //
  StatusCode sc = iupgrade ( itrack , output , otracks ) ;
  if ( sc.isFailure() ) { return Error ( "upgrade: error from iupgrade" , sc ) ; }
  //
  // sort tracks ? 
  if ( ptOrder() ) 
  { std::stable_sort ( output.begin () , output.end () , Hlt::SortTrackByPt() ) ; }
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
  // not reconstructed yet ?
  if ( -1 == seed->info ( recoID () , -1 ) ) 
  {
    // recontruct it!!!
    StatusCode sc = reco ( seed , output , otracks ) ;
    if ( sc.isFailure () ) 
    { return Error ( "Failure from ITrackFromTrack tool, skip track", sc ) ; }
  }
  // find in the list of recontructed 
  else { find ( seed , output , otracks ) ; } // find in the list of recontructed 
  //
  return StatusCode::SUCCESS ;
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
  for ( Hlt::Candidate::ConstVector::const_iterator iseed = input.begin() ;
        input.end() != iseed ; ++iseed ) 
  {
    const Hlt::Candidate* candidate = *iseed ;
    if ( 0 == candidate ) 
    { Error ( "Invalid Hlt::Candidate, skip it!") ; continue ; } // CONTINUE 
    const Hlt::Stage*     stage  = candidate->currentStage() ;
    if ( 0 == stage     )
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
    if ( sc.isFailure() ) { Error ( "Error from i_upgrade_1track", sc ) ; }
    //
  } //
  //
  return StatusCode::SUCCESS ;
}
// ======================================================================      
/*  upgrade the candidates          
 *  @param itrack  input track/seeds 
 *  @param otracks output container of upgraded tracks (cumulative) 
 *  @return status code 
 */
// ======================================================================      
StatusCode LoKi::Hlt1::UpgradeTool::upgradeMultiTracks
( const Hlt::Candidate::ConstVector& input  , 
  Hlt::Candidate::ConstVector&       output ) const 
{
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != alg()    , "GaudiAlgorithm*   points to NULL!" ) ;
  //
  LHCb::Track::Container* otracks = storedTracks ( address () ) ;
  // 
  // loop over input candidates, upgrade one-by-one 
  for ( Hlt::Candidate::ConstVector::const_iterator iseed = input.begin() ;
        input.end() != iseed ; ++iseed ) 
  {
    const Hlt::Candidate* candidate = *iseed ;
    if ( 0 == candidate ) 
    { Error ( "Invalid Hlt::Candidate, skip it!") ; continue ; } // CONTINUE 
    const Hlt::Stage*     stage  = candidate->currentStage() ;
    if ( 0 == stage     )
    { Error ( "Invalid Hlt::Stage,     skip it!") ; continue ; } // CONTINUE 
    //
    // upgrade single track
    if ( !stage->is<Hlt::MultiTrack>() ) 
    { Error ( "No Hlt::MultiTrack,     skip it!") ; continue ; } // CONTINUE  
    //
    StatusCode sc = _i_upgrade_multi_track 
      ( candidate     , 
        output        , 
        otracks       ) ;
    //
    if ( sc.isFailure() ) { Warning( "Error from i_uprgade_multi_track", sc ) ; }
    //
  } //                                      end of the loop over new candidates 
  //
  return StatusCode::SUCCESS ;
} 
// ======================================================================      
/*  upgrade the candidates          
 *  @param itrack  input track/seeds 
 *  @param otracks output container of upgraded tracks (cumulative) 
 *  @return status code 
 */
// ======================================================================      
StatusCode LoKi::Hlt1::UpgradeTool::upgradeMultiTracks
( const Hlt::Candidate::ConstVector& input  , 
  const unsigned int                 index  ,
  Hlt::Candidate::ConstVector&       output ) const 
{
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != alg()    , "GaudiAlgorithm*   points to NULL!" ) ;
  //
  LHCb::Track::Container* otracks = storedTracks ( address () ) ;
  // 
  // loop over input candidates, upgrade one-by-one 
  for ( Hlt::Candidate::ConstVector::const_iterator iseed = input.begin() ;
        input.end() != iseed ; ++iseed ) 
  {
    const Hlt::Candidate* candidate = *iseed ;
    if ( 0 == candidate ) 
    { Error ( "Invalid Hlt::Candidate, skip it!") ; continue ; } // CONTINUE 
    const Hlt::Stage*     stage  = candidate->currentStage() ;
    if ( 0 == stage     )
    { Error ( "Invalid Hlt::Stage,     skip it!") ; continue ; } // CONTINUE 
    //
    // upgrade single track
    if ( !stage->is<Hlt::MultiTrack>() ) 
    { Error ( "No Hlt::MultiTrack,     skip it!") ; continue ; } // CONTINUE  
    //
    StatusCode sc = _i_upgrade_multi_track_j  
      ( candidate     , 
        index         ,
        output        , 
        otracks       ) ;
    //
    if ( sc.isFailure() ) { Warning( "Error from i_uprgade_multi_track", sc ) ; }
    //
  } //                                      end of the loop over new candidates 
  //
  return StatusCode::SUCCESS ;
} 
// ======================================================================      
/*  upgrade the candidates          
 *  @param itrack  input track/seeds 
 *  @param otracks output container of upgraded tracks (cumulative) 
 *  @return status code 
 */
// ======================================================================      
StatusCode LoKi::Hlt1::UpgradeTool::upgradeAll 
( const Hlt::Candidate::ConstVector& input  , 
  Hlt::Candidate::ConstVector&       output ) const 
{
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != alg()    , "GaudiAlgorithm*   points to NULL!" ) ;
  //
  LHCb::Track::Container* otracks = storedTracks ( address() ) ;
  // 
  // loop over input candidates, upgrade one-by-one 
  for ( Hlt::Candidate::ConstVector::const_iterator iseed = input.begin() ;
        input.end() != iseed ; ++iseed ) 
  {
    const Hlt::Candidate* candidate = *iseed ;
    if ( 0 == candidate ) 
    { Error ( "Invalid Hlt::Candidate, skip it!") ; continue ; } // CONTINUE 
    const Hlt::Stage*     stage  = candidate->currentStage() ;
    if ( 0 == stage     )
    { Error ( "Invalid Hlt::Stage,     skip it!") ; continue ; } // CONTINUE 
    //
    // upgrade single track
    if     ( stage->is<LHCb::Track>     () ) 
    {
      StatusCode sc = _i_upgrade_1track 
        ( candidate   , 
          output      , 
          otracks     ) ;
      if ( sc.isFailure() ) { Warning( "Error from i_uprgade_track", sc ) ; }
    }
    else if ( stage->is<Hlt::MultiTrack>() ) 
    {
      StatusCode sc = _i_upgrade_multi_track 
        ( candidate     , 
          output        , 
          otracks       ) ;
      if ( sc.isFailure() ) { Warning( "Error from i_uprgade_multi_track", sc ) ; }
    }
    else { Warning("Invalid type for track-upgrade!") ; }
    // ========================================================================
  } //                                      end of the loop over new candidates 
  // ==========================================================================
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
  if ( 0 == input ) { return Error ( "Hlt::Candidate* points to NULL") ; }
  //
  const Hlt::Stage* stage = input->currentStage() ;
  if ( 0 == stage || !stage->is<LHCb::Track> () ) 
  { return Error ( "Invalid Hlt::Stage*" ) ; }
  //
  const LHCb::Track*    seed = stage->get<LHCb::Track> () ;
  //
  LHCb::Track::ConstVector out ;
  StatusCode sc = iupgrade ( seed , out , otracks ) ;              // USE TOOL 
  if ( sc.isFailure () ) 
  { return Error ( "Error from iupgrade, skip track", sc ) ; }     // RETURN 
  //
  if      ( out.empty()     ) {  /* continue */ ; }                // CONTINUE 
  else if ( 1 == out.size() && out[0] == seed ) 
  {
    // no actual upgrade, just update the history 
    Hlt::Stage* s = const_cast<Hlt::Stage*> ( stage ) ;
    Hlt::Stage::Lock lock ( s , upgradeTool() ) ;
    // lock.addToHistory ( myName() ) ;
    //
    Hlt::Candidate* _input = const_cast<Hlt::Candidate*>  ( input ) ;
    _input   -> addToWorkers ( alg() ) ;
    //
    output.push_back ( input ) ;
  }
  else if ( 1 == out.size() ) 
  {
    // track has been upgraded
    Hlt::Stage* newstage = newStage() ;
    //
    Hlt::Stage::Lock lock ( newstage , upgradeTool() ) ;
    // lock.addToHistory ( myName() ) ;
    newstage -> set( out[0] ) ; // add track to the stage
    Hlt::Candidate* _input = const_cast<Hlt::Candidate*>  ( input ) ;
    _input   -> addToWorkers ( alg() ) ;
    _input   -> addToStages ( newstage ) ;
    output.push_back ( input ) ;                                   // OUTPUT++
  }
  else 
  {
    //
    Warning ( "Reconstruction causes split of candidate" ) ;
    //
    /// start new candidates 
    for ( LHCb::Track::ConstVector::const_iterator iout = out.begin() ;
          out.end() != iout ; ++iout ) 
    {
      const LHCb::Track* track = *iout ;
      if ( 0 == track ) { continue ; }
      // 
      // start new Candidate: 
      Hlt::Candidate* candidate = newCandidate() ;
      candidate -> addToWorkers ( alg() ) ;
      output.push_back      ( candidate ) ;                          // OUTPUT++
      //
      // the initiator of new candidate is the stage of the initial candidate:
      Hlt::Stage* stage1 = newStage() ;
      candidate -> addToStages ( stage1 ) ;
      //
      Hlt::Stage::Lock lock1 ( stage1, upgradeTool() ) ;
      lock1.addToHistory ( input->workers() ) ;
      // lock1.addToHistory ( myName() ) ;
      stage1 -> set ( stage ) ; // add stage into stage as initiator 
      
      //
      // the actual stage: new candidate is the track
      Hlt::Stage* stage2 = newStage() ;
      candidate -> addToStages ( stage2 ) ;
      // 
      Hlt::Stage::Lock lock2 ( stage2, upgradeTool() ) ;
      // lock2.addToHistory     ( myName() ) ;
      stage2 -> set ( track ) ; // add track into stage
      //
    } //                                  end of the loop over new candidates
    // ======================================================================
  }
  // ========================================================================
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  upgrade the candidate          
 *  @param itrack  input track/seeds 
 *  @param otracks output container of upgraded tracks (cumulative) 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeTool::_i_upgrade_multi_track 
( const Hlt::Candidate*              input        , 
  Hlt::Candidate::ConstVector&       output       , 
  LHCb::Track::Container*            otracks      ) const 
{
  //
  if ( 0 == input ) { return Error("Hlt::Candidate points to NULL") ; }
  const Hlt::Stage* stage = input->currentStage() ;
  //
  if ( 0 == stage || !stage->is<Hlt::MultiTrack> () ) 
  { return Error("Invalid Hlt::Stage*" ) ; }
  //
  const Hlt::MultiTrack* multi_track = stage->get<Hlt::MultiTrack> () ;
  //
  typedef std::vector<LHCb::Track::ConstVector> OUTs ;
  
  bool OK    = true  ;
  bool split = false ;
  OUTs outs ;
  
  const Hlt::MultiTrack::Tracks& tracks = multi_track->tracks() ;
  for ( Hlt::MultiTrack::Tracks::const_iterator itrack = tracks.begin() ; 
        tracks.end() != itrack ; ++itrack ) 
  {
    LHCb::Track::ConstVector out ;
    StatusCode sc = iupgrade ( *itrack , out , otracks ) ;
    if ( sc.isFailure() ) 
    { Error    ( "Error from iupgrade", sc ) ; OK = false ; break ; } // BREAK
    if ( out.empty()    ) 
    { Warning  ( "No tracks")                ; OK = false ; break ; } // BREAK 
    //
    if ( 1 != out.size() ) { split = true ; }
    // 
    outs.push_back ( out ) ;
  }
  //
  if ( !OK || tracks.size() != outs.size() ) 
  { return Error ( "Upgrade failure" ) ; }                            // RETURN 
  //
  if ( !split ) 
  {
    // new multi-track  
    Hlt::MultiTrack* mtrack = newMultiTrack() ;
    //
    for ( OUTs::const_iterator iout = outs.begin() ; outs.end() != iout ; ++iout ) 
    { mtrack->addToTracks( iout->front() ) ; }
    //
    // new stage  
    Hlt::Stage* newstage = newStage() ;
    //
    Hlt::Stage::Lock lock ( newstage , upgradeTool () ) ;
    // lock.addToHistory ( myName() ) ;
    newstage -> set( mtrack ) ; // add multitrack to the stage
    Hlt::Candidate* _input = const_cast<Hlt::Candidate*>  ( input ) ;
    _input -> addToWorkers ( alg()    ) ;
    _input -> addToStages  ( newstage ) ;
    output.push_back ( input ) ;                               // OUTPUT++ 
  }
  else
  {
    //
    Warning ( "Reconstruction causes split of candidate" ) ;
    //
    typedef LoKi::Combiner_<LHCb::Track::ConstVector> COMBINER ;
    COMBINER combiner ;
    //
    // fill combiner with data: 
    for ( OUTs::const_iterator iout = outs.begin() ; outs.end() != iout ; ++iout ) 
    { combiner.add ( LHCb::Track::Range ( *iout ) ) ; }
    //
    // start looping over all combinations 
    for ( ; combiner.valid() ; ++combiner ) 
    {
      //
      Hlt::Candidate*  newcand   = newCandidate  () ;
      newcand  -> addToWorkers ( alg () ) ;
      //
      Hlt::Stage*      newstage1 = newStage      () ;
      Hlt::Stage*      newstage2 = newStage      () ;
      
      Hlt::MultiTrack* newtrack  = newMultiTrack () ;
      //
      Hlt::Stage::Lock lock1 ( newstage1 , upgradeTool () ) ;
      lock1.addToHistory ( input->workers() ) ;
      // lock1.addToHistory ( myName() ) ;
      newstage1 -> set<Hlt::Stage>      ( stage    ) ;
      //
      Hlt::Stage::Lock lock2 ( newstage1 , upgradeTool () ) ;
      // lock2.addToHistory ( myName() ) ;
      newstage2 -> set<Hlt::MultiTrack> ( newtrack ) ;
      //
      // fill multi-track:
      const COMBINER::Select& cur = combiner.current() ;
      for ( COMBINER::Select::const_iterator ia = cur.begin() ; 
            cur.end() != ia ; ++ia ) 
      { newtrack->addToTracks ( **ia ) ; }
      //
      newcand -> addToStages ( newstage1 ) ;
      newcand -> addToStages ( newstage2 ) ;
      //
      output.push_back ( newcand ) ;
    }
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  upgrade the candidate          
 *  @param itrack  input track/seeds 
 *  @param otracks output container of upgraded tracks (cumulative) 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeTool::_i_upgrade_multi_track_j
( const Hlt::Candidate*              input        , 
  const unsigned int                 index        , // the index 
  Hlt::Candidate::ConstVector&       output       , 
  LHCb::Track::Container*            otracks      ) const 
{
  //
  if ( 0 == input ) { return Error("Hlt::Candidate points to NULL") ; }
  const Hlt::Stage* stage = input->currentStage() ;
  //
  if ( 0 == stage || !stage->is<Hlt::MultiTrack> () ) 
  { return Error("Invalid Hlt::Stage*" ) ; }
  //
  const Hlt::MultiTrack* multi_track = stage->get<Hlt::MultiTrack> () ;
  //
  
  typedef std::vector<LHCb::Track::ConstVector> OUTs ;
  

  const Hlt::MultiTrack::Tracks& tracks = multi_track->tracks() ;
  
  //
  if ( tracks.size() <= index ) 
  { return Error( "Invalid size of track-vector") ; }
  
  // get the track:
  const LHCb::Track* track = tracks[index] ;
  if ( 0 == track ) 
  { return Error ( "Invalid track by index" ) ; }
  
  // upgrade it!
  LHCb::Track::ConstVector out ;
  StatusCode sc = iupgrade ( track , out , otracks ) ;
  if ( sc.isFailure() ) { return Error    ( "Error from iupgrade", sc ) ; } 
  if ( out.empty()    ) { return Warning  ( "No tracks" )               ; }
  //
  
  for ( LHCb::Track::ConstVector::const_iterator iout = out.begin() ; 
        out.end() != iout ; ++iout ) 
  {
    const LHCb::Track* _track = *iout ;
    if ( 0 == _track ) { continue ; }
    //
    Hlt::Candidate* newcand = 0 ;
    if ( iout == out.begin() ) { newcand = const_cast<Hlt::Candidate*> (  input ) ; }
    //
    if ( 0 == newcand ) 
    {
      newcand              = newCandidate () ;
      newcand  -> addToWorkers ( alg()    ) ;
      //
      Hlt::Stage* newstage = newStage       () ;
      newcand  -> addToStages  ( newstage ) ;
      //
      Hlt::Stage::Lock lock ( newstage , upgradeTool () ) ;
      lock.addToHistory ( input -> workers() ) ;
      // lock.addToHistory ( myName() ) ;
      newstage -> set<Hlt::Stage>      ( stage ) ;
      //
    }
    //
    Hlt::Stage*      newstage   = newStage        () ;
    newcand  -> addToStages  ( newstage ) ;
    //
    Hlt::MultiTrack* newmtrack  = newMultiTrack   ()  ;
    Hlt::Stage::Lock lock ( newstage , upgradeTool () ) ;
    // lock.addToHistory ( myName() ) ;
    newstage -> set<Hlt::MultiTrack>  ( newmtrack ) ;
    //
    // fill multi-track: 
    LHCb::Track::ConstVector _tracks ( tracks.begin() , tracks.end  () ) ;
    _tracks [index] = _track ;
    // 
    for ( LHCb::Track::ConstVector::const_iterator it = _tracks.begin() ; 
          _tracks.end() != it ; ++it ) 
    { newmtrack -> addToTracks ( *it ) ; }
    //
    output.push_back ( newcand ) ;
  }
  //
  return StatusCode::SUCCESS ;
  
}
// ============================================================================
// The END 
// ============================================================================
