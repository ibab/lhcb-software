// $Id: TrUpgrade.cpp,v 1.1 2008-11-13 13:14:42 ibelyaev Exp $
// ============================================================================
// Include Files 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Hlt1.h"
#include "LoKi/Algs.h"
#include "LoKi/TrUpgrade.h"
// ============================================================================
// Local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  The implementation file for class LoKi:Hlt::TrUpgrade 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-12
 */
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output  the output selection name 
 *  @param config  the tool configuration 
 */
// ============================================================================
LoKi::Hlt1::TrUpgrade::TrUpgrade 
( const std::string&             output  ,         // output selection name/key 
  const LoKi::Hlt1::UpgradeConf& config  )         //             configuration 
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe () 
  , m_config    ( config ) 
  // 
  , m_sink      ( output ) 
  //
  , m_recoID    ( -1 ) 
  , m_upgrade   (   ) 
  , m_alg       ( 0 )
{
  init () ;
}
// ============================================================================
/*  constructor from all configuration parameters 
 *  @param output    the output selection name 
 *  @param trTool    the name of ITracksFromTrack tool 
 *  @param trType    the track type for upgrade 
 *  @param moveIDs   transfer IDs ?
 *  @param moveAncs  transfer anscendents ?
 *  @param moveInfo  transfer Extra Info ?
 *  @param ptOrder   order in pt ?
 */
// ============================================================================
LoKi::Hlt1::TrUpgrade::TrUpgrade 
( const std::string&        output   ,          //    output selection name/key 
  const std::string&        trTool   ,          //   ITrackFromTrack  tool name
  const std::string&        address  ,          //   TES location of the tracks 
  const LHCb::Track::Types  trType   ,          //                   track type 
  const bool                owner    ,          //                      owner ?
  const bool                moveIDs  ,          //               transfer IDs ? 
  const bool                moveAncs ,          //       transfer anscendents ? 
  const bool                moveInfo ,          //        transfer extra info ?
  const bool                ptOrder  )          //                   order pt ?
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe () 
  , m_config ( trTool , address , trType , 
               owner  , moveIDs , moveAncs , moveInfo , ptOrder ) 
  // 
  , m_sink      ( output ) 
  //
  , m_recoID    ( -1 ) 
  , m_upgrade   (    ) 
  , m_alg       ( 0  )
{
  init () ;
}
// ============================================================================
// intialize the structures 
// ============================================================================
void LoKi::Hlt1::TrUpgrade::init () const 
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
  Assert( info , " request for unknown Info ID");
  //
  m_recoID = info->second ;
}
// ============================================================================
// OPTIONAL:: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::TrUpgrade::fillStream ( std::ostream& s ) const 
{ return s ; }
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
// MANDATORY : theonly essentiam method 
// ============================================================================
LoKi::Hlt1::TrUpgrade::result_type 
LoKi::Hlt1::TrUpgrade::operator() 
  ( LoKi::Hlt1::TrUpgrade::argument a ) const
{
  //
  if ( !m_upgrade  || 0 == m_alg || -1 == recoID () ) { init () ; } 
  //
  Assert ( !(!m_upgrade) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != m_alg    , "GaudiAlgorithm*   points to NULL!" ) ;
  
  typedef LHCb::Track::Container TRACKS ;
  typedef result_type            OUTPUT ;
  // get or create the output
  TRACKS* otracks = m_alg->getOrCreate<TRACKS,TRACKS> ( address() ) ;
  
  result_type output ;
  // ==========================================================================
  // loop over input tracks, upgrade track-by-track 
  // ==========================================================================
  for ( OUTPUT::const_iterator iseed = a.begin() ; a.end() != iseed ; ++iseed ) 
  {
    LHCb::Track* seed = *iseed ;
    if ( 0 == seed ) { continue ; }
    // seed is already ugraded track ?
    if ( seed->checkType ( trType () ) ) { output.push_back ( seed ) ; continue ; }
    // not reconstructed yet ?
    else if ( -1 == seed->info ( m_recoID , -1 ) ) 
    {
      // recontruct it!!!
      StatusCode sc = reco ( seed , output , otracks ) ;
      if ( sc.isFailure () ) 
      { 
        Error ( "Failure from ITrackFromTrack tool, skip track", sc ) ;
        continue ; 
      }
    }
    // find in the list of recontructed 
    else { find ( seed , output , otracks ) ; } // find in the list of recontructed 
    // ========================================================================
  } // end of the loop over input tracks 
  // ==========================================================================
  // sort tracks 
  if ( ptOrder() ) 
  { std::stable_sort ( output.begin () , output.end   () , Hlt::SortTrackByPt() ) ; }
  // register the selection for Hlt Data Service 
  return m_sink ( output ) ;                                          // RETURN 
}
// ============================================================================
// upgrade/reconstruct one track 
// ============================================================================
StatusCode LoKi::Hlt1::TrUpgrade::reco 
( const LHCb::Track*         seed    , 
  std::vector<LHCb::Track*>& tracks  , 
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
    for ( OUTPUT::const_iterator itr = out.begin() ; out.end() != itr ; ++itr ) 
    { otracks->insert ( *itr ) ; }  
  }
  //
  return StatusCode::SUCCESS ;                                       // RETURN
}
// ============================================================================
// find the tracks within the recontructed 
// ============================================================================
size_t LoKi::Hlt1::TrUpgrade::find 
( const LHCb::Track*         seed    , 
  std::vector<LHCb::Track*>& tracks  ,
  LHCb::Track::Container*    otracks ) const 
{
  //
  const size_t ntracks = tracks.size() ;
  LoKi::Algs::copy_if 
    ( otracks -> begin () , 
      otracks -> end   () , 
      std::back_inserter ( tracks ) , IsAncestor ( seed ) ) ;
  // return 
  return tracks.size() - ntracks ;
}


// ============================================================================
// The END 
// ============================================================================
