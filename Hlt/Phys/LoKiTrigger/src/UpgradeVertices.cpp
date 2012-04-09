// ============================================================================
// Include files
// ============================================================================
// boost
// ============================================================================
#include "boost/foreach.hpp"
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
#include "LoKi/UpgradeVertices.h"
#include "LoKi/Combiner.h"
#include "LoKi/Constants.h"
// ============================================================================
// local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  Implementation file for class LoKi::Hlt1::UpgradeVertices
 *  @date 2012-02-24
 *  @author Pieter David pieter.david@cern.ch
 */
// ============================================================================
/* constructor from all configuration parameters
 *  @param output  the output selection name
 *  @param config  the tool configuration
 */
// ============================================================================
LoKi::Hlt1::UpgradeVertices::UpgradeVertices
( const std::string&              output     ,     // output selection name/key
  const LoKi::Hlt1::UpgradeConf&  config     ,     //             configuration
        bool                      allow1Fail )
  : LoKi::Hlt1::Upgrade   ( output, config )
  , m_cuts_trivial        ( true           )
  , m_cuts                ( LoKi::Constant<const LHCb::Track*,bool>( true ) )
  , m_allow1Fail          ( allow1Fail     )
{ if ( m_allow1Fail ) { retrieveFailKey() ; } }
// ============================================================================
/* constructor from all configuration parameters
 *  @param output  the output selection name
 *  @param config  the tool configuration
 */
// ============================================================================
LoKi::Hlt1::UpgradeVertices::UpgradeVertices
( const std::string&              output     ,     // output selection name/key
  const LoKi::Hlt1::UpgradeTool&  config     ,     //             configuration
        bool                      allow1Fail )
  : LoKi::Hlt1::Upgrade( output, config )
  , m_cuts_trivial        ( true           )
  , m_cuts                ( LoKi::Constant<const LHCb::Track*,bool>( true ) )
  , m_allow1Fail          ( allow1Fail     )
{ if ( m_allow1Fail ) { retrieveFailKey() ; } }
// ============================================================================
/* constructor from all configuration parameters
 *  @param output  the output selection name
 *  @param config  the tool configuration
 *  @param cuts    cuts to be applied on the tracks after upgrade
 */
// ============================================================================
LoKi::Hlt1::UpgradeVertices::UpgradeVertices
( const std::string&              output     ,     // output selection name/key
  const LoKi::Hlt1::UpgradeConf&  config     ,     //             configuration
  const LoKi::TrackTypes::TrCuts& cuts       ,     //                track cuts
        bool                      allow1Fail )
  : LoKi::Hlt1::Upgrade   ( output, config )
  , m_cuts_trivial        ( false          )
  , m_cuts                ( cuts           )
  , m_allow1Fail          ( allow1Fail     )
{ if ( m_allow1Fail ) { retrieveFailKey() ; } }
// ============================================================================
/* constructor from all configuration parameters
 *  @param output  the output selection name
 *  @param config  the tool configuration
 *  @param cuts    cuts to be applied on the tracks after upgrade
 */
// ============================================================================
LoKi::Hlt1::UpgradeVertices::UpgradeVertices
( const std::string&              output     ,     // output selection name/key
  const LoKi::Hlt1::UpgradeTool&  config     ,     //             configuration
  const LoKi::TrackTypes::TrCuts& cuts       ,     //                track cuts
        bool                      allow1Fail )
  : LoKi::Hlt1::Upgrade( output, config )
  , m_cuts_trivial        ( false          )
  , m_cuts                ( cuts           )
  , m_allow1Fail          ( allow1Fail     )
{ if ( m_allow1Fail ) { retrieveFailKey() ; } }
// ============================================================================
// Retrieve ExtraInfo key for failed tracks
// ============================================================================
void LoKi::Hlt1::UpgradeVertices::retrieveFailKey ()
{
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  const std::string infoIDName = "HltUnit/" + alg()->name();
  boost::optional<IANNSvc::minor_value_type> _info = ann->value("InfoID" , infoIDName ) ;
  Assert( _info , " request for unknown Info ID : " + infoIDName ) ;
  m_failKey = _info->second;
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Hlt1::UpgradeVertices::~UpgradeVertices(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::UpgradeVertices*
LoKi::Hlt1::UpgradeVertices::clone() const
{ return new LoKi::Hlt1::UpgradeVertices ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::UpgradeVertices::fillStream ( std::ostream& s ) const
{
    s  << "TC_UPGRADE_VX('" << selName()
       << "',"              << config ();
    if ( m_allow1Fail ) {
      s  << ", " << m_failKey;
    }
    s  << ")";
    return s ;
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Hlt1::UpgradeVertices::result_type
LoKi::Hlt1::UpgradeVertices::operator()
  ( LoKi::Hlt1::UpgradeVertices::argument a ) const
{
  result_type output ;
  if ( !a.empty() )
  {
    StatusCode sc = upgradeVertices ( a, output ) ;
    if ( sc.isFailure() ) { Error ( "Error from upgrade" , sc, 0 ) ; }
  }
  // register the selection
  return !m_sink ? output : m_sink ( output ) ;
}
// ============================================================================
// The "worker" method - combination of fragments from TrUpgrade and VxUpgrade
// ============================================================================
StatusCode LoKi::Hlt1::UpgradeVertices::upgradeVertices
  ( const Hlt::Candidate::ConstVector& input  ,
          Hlt::Candidate::ConstVector& output ) const
{
  //
  Assert ( !(!upgradeTool()) , "ITracksFromTrack* points to NULL!" ) ;
  Assert ( 0 != alg()        , "GaudiAlgorithm*   points to NULL!" ) ;
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
    if ( !stage->is<LHCb::RecVertex>() )
    { Error ( "No LHCb::RecVertex,     skip it!") ; continue ; } // CONTINUE
    //
    StatusCode sc = _i_upgrade_recvertex_j
      ( candidate     ,
        output        ,
        otracks       ) ;
    //
    if ( sc.isFailure() ) { Warning( "Error from i_uprgade_multi_track", sc, 0 ) ; }
    //
  } //                                      end of the loop over new candidates
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
StatusCode LoKi::Hlt1::UpgradeVertices::_i_upgrade_recvertex_j
( const Hlt::Candidate*              input        ,
  Hlt::Candidate::ConstVector&       output       ,
  LHCb::Track::Container*            otracks      ) const
{
  typedef SmartRefVector<LHCb::Track>   TRKs     ;
  typedef LHCb::Track::ConstVector      TRACKS   ;
  typedef std::vector<LHCb::RecVertex*> OUTPUT   ;
  //
  if ( 0 == input ) { return Error ( "Hlt::Candidate* points to NULL") ; }
  //
  const Hlt::Stage* stage = input->currentStage() ;
  if ( 0 == stage || !stage->is<LHCb::RecVertex> () )
  { return Error ( "Invalid Hlt::Stage*" ) ; }
  //
  const LHCb::RecVertex* inputVertex = stage->get<LHCb::RecVertex> () ;
  //
  const TRKs& tracks = inputVertex -> tracks();
  if ( 2 != tracks.size() ) { return Error ( " nTrack != 2, skip it! " ) ; }
  const LHCb::Track* tr1 = tracks[0] ;
  const LHCb::Track* tr2 = tracks[1] ;
  if ( 0 == tr1 || 0 == tr2 )
  { return Error ( "LHCb::Track* points to NULL, skip it") ; }

  TRACKS tracks1 ;
  TRACKS tracks2 ;
  if ( ! m_allow1Fail ) {

    // upgrade the first track
    StatusCode sc1 = iupgrade ( tr1 , tracks1, otracks ) ;
    if ( sc1.isFailure() )
    { return Error ( "Error from upgrade(track1) , skip " , sc1 ) ; }

    // upgrade the second track
    StatusCode sc2 = iupgrade ( tr2 , tracks2, otracks ) ;
    if ( sc2.isFailure() )
    { return Error ( "Error from upgrade(track2) , skip " , sc2 ) ; }

  } else {
    bool tr1Failed = tr1->hasInfo(m_failKey);
    bool tr2Failed = tr2->hasInfo(m_failKey);
    bool tr1FailedNow = false;
    bool tr2FailedNow = false;

    StatusCode sc1 = StatusCode::SUCCESS;
    StatusCode sc2 = StatusCode::SUCCESS;
    if ( ! tr1Failed ) {
      // upgrade the first track
      sc1 = iupgrade( tr1 , tracks1, otracks ) ;
      tr1FailedNow = ( sc1.isFailure() || tracks1.empty() );
      tr1Failed = tr1FailedNow;
    }
    if ( ! tr2Failed ) {
      // upgrade the second track
      sc2 = iupgrade ( tr2 , tracks2, otracks ) ;
      tr2FailedNow = ( sc2.isFailure() || tracks2.empty() );
      tr2Failed = tr2FailedNow;
    }
    if ( tr1Failed && ( ! tr2Failed ) ) {
      // recover track 1
      if ( tr1FailedNow ) {
        // failed now, need to check the flag
        LHCb::Track* newTr = tr1->clone();
        newTr->addInfo(m_failKey, 1.);
        tracks1.push_back(newTr);
      } else {
        // failed previously, just pass on
        tracks1.push_back(tr1);
      }
    }
    if ( tr2Failed && ( ! tr1Failed ) ) {
      // recover track 2
      if ( tr2FailedNow ) {
        // failed now, need to check the flag
        LHCb::Track* newTr = tr2->clone();
        newTr->addInfo(m_failKey, 1.);
        tracks2.push_back(newTr);
      } else {
        // failed previously, just pass on
        tracks2.push_back(tr2);
      }
    }
  }

  if ( tracks1.empty() || tracks2.empty() ) { return StatusCode::SUCCESS ; }

  OUTPUT out;
  StatusCode sc = make ( tracks1 , tracks2 , out );
  if ( sc.isFailure() )
  { return Error ( "Error from vertex creation, skip", sc, 0 ) ; }

  //
  // Process output vertices, create a new candidate for vertices (beyond the first ??)
  for ( OUTPUT::const_iterator iout = out.begin() ;
        out.end() != iout ; ++iout ) {
     const LHCb::RecVertex* vx = *iout ;
     if ( 0 == vx ) {
        continue ;
     }
     //
     Hlt::Candidate* _input = 0;
     if ( iout == out.begin() ) {
        _input = const_cast<Hlt::Candidate*>  ( input ) ;
        // *assumed* to be correct for vertices as well...
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
        // lock1.addToHistory ( myName() ) ;
        stage1 -> set ( stage ) ; // add stage into stage as initiator
     }
     if ( vx != inputVertex ) {
        // Add the new stage to the candidate
        Hlt::Stage* newstage = newStage() ;
        //
        Hlt::Stage::Lock lock ( newstage , upgradeTool() ) ;
        newstage -> set( vx ) ; // add vertex to the stage
        _input   -> addToStages ( newstage ) ;
     }
     _input   -> addToWorkers ( alg() ) ;
     output.push_back ( _input ) ;                                 // OUTPUT++
  }  //                                   end of the loop over output vertices
  return StatusCode::SUCCESS ;
}
// ============================================================================
// make the vertices
// ============================================================================
size_t LoKi::Hlt1::UpgradeVertices::make
( const LHCb::Track::ConstVector&  tracks1  ,
  const LHCb::Track::ConstVector&  tracks2  ,
  std::vector<LHCb::RecVertex*>&   vertices ) const
{
  if ( tracks1.empty() || tracks2.empty() ) { return 0 ; }

  const bool same =
    ( &tracks1 == &tracks2 ) || ( tracks1 == tracks2 ) ;

  const size_t n1 = tracks1.size() ;
  const size_t n2 = tracks2.size() ;

  vertices.reserve ( same ?
                     vertices.size() + n1*(n1-1)/2 :
                     vertices.size() + n1*n2       ) ;

  typedef LHCb::Track::ConstVector   Tracks   ;
  typedef LoKi::Combiner_<Tracks>    Combiner ;
  typedef Combiner::Range            Range    ;

  const Hlt::VertexCreator           creator    ;
  const LoKi::Hlt1::Utils::CmpTrack  compare = LoKi::Hlt1::Utils::CmpTrack()  ;

  const size_t size = vertices.size() ;

  /// create the combiner & fill it with data
  Combiner loop ;
  loop.add ( Range ( tracks1 )  ) ;
  loop.add ( Range ( tracks2 )  ) ;

  // make the combinations
  for ( ; loop.valid() ; loop.next() )
  {
    // get the current combination
    Tracks tracks ( 2 ) ;
    loop.current ( tracks.begin() ) ;

    const LHCb::Track* first  = tracks[0] ;
    const LHCb::Track* second = tracks[1] ;

    // skip invalid
    if ( 0 == first || 0 == second      ) { continue ; }              // CONTINUE
    // apply cut on tracks
    if ( !m_cuts_trivial ) {
      if ( !m_allow1Fail ) {
        if ( ! ( m_cuts(first) && m_cuts(second) ) ) { continue ; }
      } else {
        bool failFirst = first->hasInfo(m_failKey);
        bool failSecond = second->hasInfo(m_failKey);
        if ( failFirst && failSecond ) {
          // should not occur
          continue ;
        } else if ( failFirst ) {
          if ( ! m_cuts(second) ) { continue ; }
        } else if ( failSecond ) {
          if ( ! m_cuts(first) ) { continue ; }
        } else { // both passed up to this point
          if ( ! m_cuts(first) ) {
            if ( m_cuts(second) ) {
              LHCb::Track* newTrack = first->clone();
              newTrack->addInfo(m_failKey, 1.);
              first = newTrack;
            } else { continue; }
          } else {
            if ( ! m_cuts(second) ) {
              LHCb::Track* newTrack = second->clone();
              newTrack->addInfo(m_failKey, 1.);
              second = newTrack;
            } // else, both pass, so nothing to do
          }
        }
      }
    }
    // skip the same
    if ( first == second                ) { continue ; }              // CONTINUE
    // reduce the double count :
    if ( same && !compare ( first , second ) ) { continue ; }         // CONTINUE

    // chech the overrlap
    if ( HltUtils::matchIDs ( *first , *second ) ) { continue ; }     // CONTINUE

    // create the vertex
    std::auto_ptr<LHCb::RecVertex> vertex ( new LHCb::RecVertex() ) ;

    /// fill it with some information
    creator ( *first , *second , *vertex ) ;

    // good vertex! add it to the outptu container
    vertices.push_back ( vertex.release() ) ; // "vertex is not valid anymore

  }  // end of loop over all combinations
  //
  // return number of created vertcies
  return vertices.size() - size ; // RETURN
}
// ============================================================================
// The END
// ============================================================================
