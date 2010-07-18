// $Id: UpgradeTool.cpp,v 1.3 2008-11-17 13:51:27 ibelyaev Exp $
// ============================================================================
// Include files 
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
  : LoKi::AuxFunBase() 
  , m_config  ( info ) 
  , m_recoID  ( -1   ) 
  , m_upgrade (      ) 
  , m_alg     ( 0    ) 
{
  init() ;
}
// ============================================================================
// intialize the object 
// ============================================================================
void LoKi::Hlt1::UpgradeTool::init () const  
{
  // get GaudiAlgorithm 
  GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg ( *this ) ;
  Assert ( 0 != alg , "GaudiAlgorithm points to NULL!" ) ;
  // get IAlgorithm 
  IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg ( *this ) ;
  Assert ( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ) ;
  //
  m_alg = alg ;
  // retrive the tool 
  m_upgrade = m_alg->tool<ITracksFromTrack> ( trTool() , alg ) ;
  /// it must be private tool! 
  Assert ( m_upgrade->parent() == m_alg , "ITracksFromTrack tool must be private!");  
  //
  /// get the service 
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  //
  boost::optional<IANNSvc::minor_value_type> info = ann->value("InfoID" , trTool()  );
  Assert( info , " request for unknown Info ID : " + trTool() );
  //
  m_recoID = info->second ;
}
// ============================================================================
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
} // end of anonynmous namespace
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
    Error ("find(): LHCb::Track::Container* pointns to NULL!") ;
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
  // insert the tracks into the container of output tracks 
  tracks.insert( tracks.end() , out.begin() , out.end() ) ;
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
  if ( !m_upgrade  || 0 == m_alg || -1 == recoID () ) { init () ; } 
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != m_alg    , "GaudiAlgorithm*   points to NULL!" ) ;
  
  typedef LHCb::Track::Container    TRACKS ;
  typedef LHCb::Track::ConstVector  OUTPUT ;
  
  // get or create the output
  TRACKS* otracks = 0 ;
  if ( owner() ) { otracks = alg()->getOrCreate<TRACKS,TRACKS> ( address() ) ; }
  
  // ==========================================================================
  // loop over input tracks, upgrade track-by-track 
  // ==========================================================================
  for ( OUTPUT::const_iterator iseed = itracks.begin() ; itracks.end() != iseed ; ++iseed ) 
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
  
  if ( !m_upgrade  || 0 == m_alg || -1 == recoID () ) { init () ; } 
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != m_alg    , "GaudiAlgorithm*   points to NULL!" ) ;
  
  typedef LHCb::Track::Container    TRACKS ;
  typedef std::vector<LHCb::Track*> OUTPUT ;
  // get or create the output
  TRACKS* otracks = 0 ;
  if ( owner() ) { otracks = alg()->getOrCreate<TRACKS,TRACKS> ( address() ) ; }
  
  StatusCode sc = iupgrade ( itrack , output , otracks ) ;
  if ( sc.isFailure() ) { return Error ( "upgrade: error from iupgrade" , sc ) ; }
  
  // sort tracks ? 
  if ( ptOrder() ) 
  { std::stable_sort ( output.begin () , output.end () , Hlt::SortTrackByPt() ) ; }
  
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
  // seed is already ugraded track ?
  if ( seed->checkType ( trType () ) ) 
  { output.push_back ( const_cast<LHCb::Track*> ( seed ) ) ; }
  // not reconstructed yet ?
  else if ( -1 == seed->info ( recoID () , -1 ) ) 
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
// ============================================================================
// The END 
// ============================================================================
