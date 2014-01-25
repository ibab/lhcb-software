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
/* @file
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2000-03-19
 *
 *                   $Revision$
 *  Last moficiation $Date$
 *                by $Author$
 */
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class L0Muon2Track
   *  Simple class which converts L0Muon candidates into "tracks" using
   *  the special tool by Johannes albrecht
   *  @see IMuonSeedTrack
   *  The actual lines are stolen from
   *     Gerhard Raven & Jose Angel Hernando  Morata
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2000-03-19
   */
  class L0Muon2Track : public Hlt::L0Muon2TrackBase
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Hlt::L0Muon2Track> ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the algorithm
    virtual StatusCode initialize ()
    {
      /// initialize the base
      StatusCode sc = Hlt::L0Muon2TrackBase::initialize() ;
      if ( sc.isFailure() ) { return sc ; }          // REUTRN
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
    L0Muon2Track
    ( std::string        name ,                  //     algorithm instance name
      ISvcLocator*       pSvc )                  //  pointer to Service Locator
      : Hlt::L0Muon2TrackBase( name , pSvc )
    {}
    /// virtual and protected destructor
    virtual ~L0Muon2Track() {}
    // ========================================================================
  private:
    // ========================================================================
    const Hlt::Candidate*
    checkMuon( const LHCb::Track* muon,
               const Hlt::TSelection<Hlt::Candidate>* candidates ) const
    {
        if ( !muon ) return nullptr;
        if ( !candidates ) return nullptr;
        auto candidate =
            std::find_if( std::begin( *candidates ), std::end( *candidates ),
                          [muon]( const Hlt::Candidate* c ) {
                return c && std::any_of( std::begin( c->stages() ),
                                         std::end( c->stages() ),
                                         [muon]( const Hlt::Stage* s ) {
                                return s && muon == s->get<LHCb::Track>();
                            } );
            } );
        return candidate != std::end( *candidates )  ? *candidate : nullptr;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    L0Muon2Track () ;                    // the default constructor is disabled
    /// the copy constructor is disabled
    L0Muon2Track ( const L0Muon2Track& ) ;  // the copy constructor is disabled
    /// the assignement operator is disabled
    L0Muon2Track& operator=( const L0Muon2Track& ) ; // assignement is disabled
    // ========================================================================
  protected:
    // ========================================================================
    bool checkClone ( const LHCb::L0MuonCandidate&  muon) const ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
// execute the algorithm
// ============================================================================
StatusCode Hlt::L0Muon2Track::execute  ()
{
  typedef  Hlt::TSelection<Hlt::Candidate> Input ;

  /// get input selection from Hlt-service
  const Input* input = hltSvc()->get<Hlt::Candidate>  ( m_input , this ) ;
  Assert (      input           , "Input  selection is invalid!" ) ;

  /// check the output selectiom
  Assert ( m_selection->empty() , "Output selection is not empty!" ) ;

  /// create the container of muons/tracks and register it in TES
  LHCb::Track::Container* muons = getMuons  () ;

  /// constaiener of stages
  Hlt::Stage::Container* stages = hltStages () ;


  /// why we shodul decode it for each event ????
  std::pair<LoKi::L0::L0MuonCut,bool> _cuts = muonCut() ;

  const LoKi::L0::L0MuonCut cut     = _cuts.first  ;
  const bool                noMuon  = _cuts.second ;

  if ( noMuon ) {}
  else
  {
    // loop over input data
    for ( const Hlt::Candidate* candidate : *input )
    {
      if ( !candidate ) { continue ; }
      //
      const Hlt::Stage* stage     = candidate->currentStage() ;
      if ( !stage     ) { continue ; }
      //
      const LHCb::L0MuonCandidate* l0muon = stage->get<LHCb::L0MuonCandidate>() ;
      //
      if ( msgLevel(MSG::DEBUG) ) debug() << "l0pt " << l0muon->pt() << " l0encodedPt " << l0muon->encodedPt()<< endmsg;
      // check the cut:
      if ( !l0muon || !cut ( l0muon) ) { continue ; }     // CONTINUE
      if ( msgLevel(MSG::DEBUG) ) debug() << "l0pt " << l0muon->pt() << " l0encodedPt " << l0muon->encodedPt()<< " accept " << endmsg;


      // Has been this L0MuonCandidate already converted into track?
      const LHCb::Track* track = seekMuon ( *l0muon , muons  ) ;

      // it is already selected and it is clone ?
      if      ( track && checkMuon ( track , m_selection ) )
      {
        if ( msgLevel(MSG::DEBUG) ) debug() << "is clone " << endmsg;
        continue ;
      }

      // there is no track
      if ( ! track )
      {
        track = makeMuon ( *l0muon , muons ) ;
        if ( !track ) { continue ; }
      }

      //
      // keep it: create new stage
      //

      Hlt::Stage* newstage = new Hlt::Stage() ;
      stages      -> push_back   ( newstage ) ;
      //
      Hlt::Candidate* candidate_ = const_cast<Hlt::Candidate*>(candidate) ;
      candidate_ -> addToWorkers ( this     ) ;
      candidate_ -> addToStages  ( newstage ) ;
      //
      Hlt::Stage::Lock lock    { newstage , maker() } ;
      newstage    -> set       ( track              ) ;
      //
      m_selection -> push_back ( candidate ) ;
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

  //
  m_selection -> setDecision ( !m_selection->empty() );

  setFilterPassed ( !m_selection->empty() ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================
// the algorithm factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Hlt,L0Muon2Track)
// ============================================================================
// The END
// ============================================================================
