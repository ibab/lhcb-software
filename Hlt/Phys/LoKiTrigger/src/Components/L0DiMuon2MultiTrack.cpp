// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/L0MuonCandidate.h"
#include "Event/L0DUReport.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltBase.h"
// ============================================================================
// Local
// ============================================================================
#include "L0Muon2TrackBase.h"
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class L0DiMuon2MultiTrack
   *  Simple class which converts L0Muon candidates into "tracks" using
   *  the special tool by Johannes albrecht
   *
   *  @see IMuonSeedTrack
   *
   *  The actual lines are stolen from
   *     Gerhard Raven & Jose Angel Hernando  Morata
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2000-03-19
   *
   *  $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  class L0DiMuon2MultiTrack : public Hlt::L0Muon2TrackBase
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Hlt::L0DiMuon2MultiTrack> ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the algorithm
    virtual StatusCode initialize ()
    {
      /// initialize the base
      StatusCode sc = Hlt::L0Muon2TrackBase::initialize() ;
      if ( sc.isFailure() )  return sc ;           // REUTRN
      //
      return StatusCode::SUCCESS ;
    }
    /// execute the algorithm
    virtual StatusCode execute  () ;
    /// finalize the algorithm
    virtual StatusCode finalize ()
    { return Hlt::L0Muon2TrackBase::finalize () ; }
    /// =======================================================================
  protected:
    // ========================================================================
    /** standard constructor
     *  @param name algorithm instance name
     *  @param pSvc pointer to Service Locator
     */
    L0DiMuon2MultiTrack
    ( const std::string& name ,                  //     algorithm instance name
      ISvcLocator*       pSvc )                  //  pointer to Service Locator
      : Hlt::L0Muon2TrackBase ( name , pSvc )
    {}
    /// virtual and protected destructor
    virtual ~L0DiMuon2MultiTrack() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    L0DiMuon2MultiTrack () ;             // the default constructor is disabled
    /// the copy constructor is disabled
    L0DiMuon2MultiTrack ( const L0DiMuon2MultiTrack& ) ; // no copy constructor
    /// the assignement operator is disabled
    L0DiMuon2MultiTrack& operator=( const L0DiMuon2MultiTrack& ) ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
// execute the algorithm
// ============================================================================
StatusCode Hlt::L0DiMuon2MultiTrack::execute  ()
{
  typedef  Hlt::TSelection<Hlt::Candidate> Input ;

  /// get input selection from Hlt-service
  const Input* input = hltSvc()->get<Hlt::Candidate>  ( m_input , this ) ;
  Assert ( input           , "Input  selection is invalid!" ) ;

  /// check the output selectiom
  Assert ( m_selection->empty() , "Output selection is not empty!" ) ;

  /// create the container of muons/tracks and register it in TES
  LHCb::Track::Container*     muons   = getMuons    () ;

  /// container of stages
  Hlt::Stage::Container*      stages  = hltStages   () ;

  /// container of multitracks
  Hlt::MultiTrack::Container* mtracks = hltMultiTracks () ;


  /// why we shodul decode it for each event ????
  std::pair<LoKi::L0::L0MuonCut,bool> cuts = muonCut() ;

  const LoKi::L0::L0MuonCut cut     = cuts.first  ;
  const bool                noMuon  = cuts.second ;

  if ( noMuon ) {}
  else {
    // loop over input data
    for ( const Hlt::Candidate* candidate : *input)
    {
      if ( !candidate ) { continue ; }
      //
      const Hlt::Stage* stage     = candidate->currentStage() ;
      if ( !stage     ) { continue ; }
      //
      const Hlt::L0DiMuonCandidate* l0dimuon = stage->get<Hlt::L0DiMuonCandidate>() ;
      //
      if (!l0dimuon ) { continue ; }
      //
      const LHCb::L0MuonCandidate* l0muon1 = l0dimuon->first  () ;
      const LHCb::L0MuonCandidate* l0muon2 = l0dimuon->second () ;
      //
      if ( msgLevel(MSG::DEBUG) ) debug() << "l0pt1 " << l0muon1->pt() << " l0encodedPt " << l0muon1->encodedPt()<< endmsg;
      if ( msgLevel(MSG::DEBUG) ) debug() << "l0pt2 " << l0muon2->pt() << " l0encodedPt " << l0muon2->encodedPt()<< endmsg;
      // check the cut:
      if ( !l0muon1 || !cut ( l0muon1 ) ) { continue ; }  // CONTINUE
      if ( !l0muon2 || !cut ( l0muon2 ) ) { continue ; }  // CONTINUE
      //
      if ( msgLevel(MSG::DEBUG) ) debug() << "l0pt1 " << l0muon1->pt() << " l0encodedPt " << l0muon1->encodedPt()<< " accept " << endmsg;
      if ( msgLevel(MSG::DEBUG) ) debug() << "l0pt2 " << l0muon2->pt() << " l0encodedPt " << l0muon2->encodedPt()<< " accept " << endmsg;
      //

      // Has been this L0MuonCandidate already converted into track?
      const LHCb::Track*   track1 = seekMuon ( *l0muon1 , muons ) ;
      if (   !track1   ) { track1 = makeMuon ( *l0muon1 , muons ) ; }
      if (   !track1   )
      {
        Error ( "Unable to get/create Track for the first   component" ) ;
        continue ;
      }

      // Has been this L0MuonCandidate already converted into track?
      const LHCb::Track*   track2 = seekMuon ( *l0muon2 , muons ) ;
      if (  !track2    ) { track2 = makeMuon ( *l0muon2 , muons ) ; }
      if (  !track2    )
      {
        Error ( "Unable to get/create Track for the second  component" ) ;
        continue ;
      }

      // keep them!
      Hlt::MultiTrack* mtrack = new Hlt::MultiTrack () ;
      mtracks     -> push_back ( mtrack ) ;
      mtrack      -> addToTracks ( track1 ) ;
      mtrack      -> addToTracks ( track2 ) ;
      //
      // keep it: create new stage
      Hlt::Stage* newstage = new Hlt::Stage() ;
      stages      -> push_back   ( newstage  ) ;
      Hlt::Candidate* candidate_ = const_cast<Hlt::Candidate*>( candidate ) ;
      candidate_ -> addToWorkers ( this     ) ;
      candidate_ -> addToStages  ( newstage ) ;
      //
      Hlt::Stage::Lock lock      ( newstage , maker() ) ;
      newstage    -> set         ( mtrack             ) ;
      //
      m_selection -> push_back   ( candidate ) ;
    }
  }

  if (msgLevel(MSG::DEBUG))
  {
    debug() << "# Input: "      << input       -> size()
            << " -> # Output: " << m_selection -> size() << endmsg;
  }

  counter ( "#input"  ) +=  input       -> size  () ;
  counter ( "#output" ) +=  m_selection -> size  () ;
  counter ( "#accept" ) += !m_selection -> empty () ;

  m_selection -> setDecision ( !m_selection->empty() );
  setFilterPassed ( !m_selection->empty() ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================
// the algorithm factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Hlt,L0DiMuon2MultiTrack)
// ============================================================================
// The END
// ============================================================================
