// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltCandidate.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltBase.h"
#include "LoKi/ITrackFunctorFactory.h"
#include "LoKi/HltBase.h"
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
 * @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 * @date 2010-11-18
 *  
 *                   $Revision$
 *  Last moficiation $Date$
 *                by $Author$
 */
// ============================================================================
namespace Hlt 
{
  // ==========================================================================
  /** @class Track2Candidate
   *  Simple class which converts tarcks into Hlt::Candidate
   *  @see Hlt::Candidate
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-11-18
   */
  class Track2Candidate : public Hlt::Base 
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<Hlt::Track2Candidate> ;
    // ========================================================================    
  public:
    // ========================================================================
    /// initialize the algorithm 
    virtual StatusCode initialize () ;
    /// execute the algorithm 
    virtual StatusCode execute    () ;
    /// finalize the algorithm 
    virtual StatusCode finalize   () ;
    /// =======================================================================
  protected:
    // ========================================================================
    /** standard constructor  
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    Track2Candidate
    ( std::string        name ,                  //     algorithm instance name 
      ISvcLocator*       pSvc ) ;                //  pointer to Service Locator 
    /// virtual and protected destructor 
    virtual ~Track2Candidate () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Track2Candidate () ;                              // no default constructor
    /// the copy constructor is disabled 
    Track2Candidate ( const Track2Candidate& ) ;         // no copy constructor 
    /// the assignement operator is disabled 
    Track2Candidate & operator=( const Track2Candidate& ) ;   // no assignement
    // ========================================================================
  protected:
    // ========================================================================
    void       updateCode      ( Property& /* p */ ) ;
    void       updateFactory   ( Property& /* p */ ) ;
    void       updatePreambulo ( Property& /* p */ ) ;
    StatusCode decode          () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the selection 
    Hlt::TSelection<Hlt::Candidate>* m_selection ;             // the selection 
    /// the output selection 
    Location m_output         ;                                //    the output
    /// the input track location in TES 
    Location m_input ;                       // the input tarck location in TES
    // ========================================================================
    std::string              m_code       ;
    std::string              m_factory    ;
    std::vector<std::string> m_preambulo  ;
    LoKi::FunctorFromFunctor<const LHCb::Track*,bool> m_cut ;
    // ========================================================================
    bool m_code_updated      ;
    bool m_factory_updated   ;
    bool m_preambulo_updated ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                       end of namespace Hlt 
// ============================================================================
/*  standard constructor  
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
Hlt::Track2Candidate::Track2Candidate
( std::string        name ,                  //     algorithm instance name 
  ISvcLocator*       pSvc )                  //  pointer to Service Locator 
  : Hlt::Base   ( name , pSvc ) 
  , m_selection { nullptr     } 
  , m_output    { std::move(name) } 
  , m_input     { "<UNDEFINED>" } 
//
  , m_code      { "<UNSPECIFIED>" }
  , m_factory   { "LoKi::Hybrid::TrackFunctorFactory/TrackFactory:PUBLIC" } 
  , m_preambulo {}
  , m_cut       { LoKi::Constant<const LHCb::Track*,bool> { false } }
//
  , m_code_updated      { true } 
  , m_factory_updated   { true } 
  , m_preambulo_updated { true } 
// 
{
  declareProperty 
    ( "OutputSelection"                 , 
      m_output                          ,
      "The name of output selection of Hlt::Candidates"    ) ;
  declareProperty
    ( "InputSelection"                  , 
      m_input                           , 
      "TES Location of inptut tracks"   ) ;
  // ==========================================================================
  declareProperty 
    ( "Code"       , m_code      , "LoKi/Bender functor for track selection" ) 
    -> declareUpdateHandler ( &Hlt::Track2Candidate::updateCode     , this ) ;
  declareProperty 
    ( "Factory"    , m_factory   , "Type/Name for TrackFunctorFactory"       ) 
    -> declareUpdateHandler ( &Hlt::Track2Candidate::updateFactory   , this ) ;
  declareProperty 
    ( "Preambulo"  , m_preambulo , "Preambulo to be used"                    ) 
    -> declareUpdateHandler ( &Hlt::Track2Candidate::updatePreambulo , this ) ;
  // ==========================================================================
}
// ============================================================================
// virtual and protected destructor 
// ============================================================================
Hlt::Track2Candidate::~Track2Candidate(){}
// ============================================================================
void Hlt::Track2Candidate::updateCode  ( Property& /* p */ ) // update the factory 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated" 
  m_code_updated = true ;
  //
}
// ============================================================================
void Hlt::Track2Candidate::updateFactory ( Property& /* p */ ) // update the factory 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated" 
  m_factory_updated = true ;
  //
}
// ============================================================================
void Hlt::Track2Candidate::updatePreambulo ( Property& /* p */ ) // update the factory 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated" 
  m_preambulo_updated = true ;
  //
}
// ============================================================================
// decode 
// ============================================================================
StatusCode Hlt::Track2Candidate::decode () 
{
  //
  m_code_updated       = true ;
  m_factory_updated    = true ;
  m_preambulo_updated  = true ;
  //
  LoKi::ITrackFunctorFactory* factory = 
    tool<LoKi::ITrackFunctorFactory> ( m_factory , this ) ;
  // construct the preambulo 
  std::string _preambulo ;
  for ( auto iline = 
          m_preambulo.begin() ; m_preambulo.end() != iline ; ++iline ) 
  { 
    if ( m_preambulo.begin() != iline ) { _preambulo += "\n" ; }
    _preambulo += (*iline) ;
  }
  StatusCode sc = factory->get( m_code , m_cut , _preambulo ) ;
  Assert ( sc.isSuccess () , "Unable to decode the Code : " + m_code ) ;
  //
  release ( factory ) ;
  //
  m_code_updated       = false ;
  m_factory_updated    = false ;
  m_preambulo_updated  = false ;
  //
  return sc ;  
}
// ============================================================================
// initialize the algorithm 
// ============================================================================
StatusCode Hlt::Track2Candidate::initialize () 
{
  /// initialize the base 
  StatusCode sc = Hlt::Base::initialize () ;
  if ( sc.isFailure() ) { return sc ; }                          // REUTRN
  //
  sc = decode () ;
  Assert ( sc.isSuccess () , "Unable to decode the Code : " + m_code ) ;
  //
  /// Lock the service to enable the output selection registration 
  Hlt::IRegister::Lock lock { regSvc() , this } ;
  /// register TES input selection
  sc = lock -> registerTESInput ( m_input     , this ) ;
  Assert ( sc.isSuccess () , "Unable to register INPUT  selection" , sc ) ;
  /// register the output selection
  m_selection = new Hlt::TSelection<Hlt::Candidate>( m_output ) ;
  sc = lock -> registerOutput ( m_selection , this ) ;
  Assert ( sc.isSuccess () , "Unable to register OUTPUT selection" , sc );
  //
  declareInfo ( "#accept" , "" , &counter("#accept") , 
                0, std::string ("Events accepted by "    ) + name () ) ;
  declareInfo ( "#input"  , "" , &counter("#input" ) ,
                0, std::string ("Candidates seen by "    ) + name () ) ;
  declareInfo ( "#output" , "" , &counter("#output") ,
                0, std::string ("Candidates accepted by ") + name () ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// execute the algorithm 
// ============================================================================
StatusCode Hlt::Track2Candidate::execute  () 
{
  //
  if ( m_code_updated || m_factory_updated || m_factory_updated ) 
  {
    StatusCode sc = decode () ; 
    Assert ( sc.isSuccess() , "Unable to decode the cut" );
  }
  //
  // some sanity checks:
  Assert ( m_selection , "Invalid Local pointer to selection" ) ;
  if ( !m_selection->empty() ) { Warning("Local selection is not empty!") ; }
  
  // get all L0 Muons from TES  
  typedef LHCb::Track::Container Tracks;
  const Tracks* tracks = tesData<Tracks> ( m_input ) ;
  
  // get(or create) containter of Hlt-Candidates from TES
  Hlt::Candidate::Container*  candidates = hltCandidates () ;
  // get(or create) containter of Hlt-Stages     from TES
  Hlt::Stage::Container*      stages     =  hltStages    () ;
  
  // loop over tracks
  for ( const LHCb::Track *track : *tracks ) 
  {
    // filter it! 
    if ( !track || ! m_cut ( track ) ) { continue ; } // CONTINUE 
    //
    //
    // create the new candidate:
    Hlt::Candidate* candidate = new Hlt::Candidate() ;
    //
    candidate  -> addToWorkers ( m_input ) ;
    candidate  -> addToWorkers ( this    ) ;
    candidates -> push_back ( candidate )  ; // add candidate into TES constainer 
    //
    // create the stage
    Hlt::Stage* stage = new Hlt::Stage() ;
    // add it into candidate 
    stages     -> push_back ( stage    )   ;  // add stage  into TES container 
    // add it into Candidate! 
    candidate  -> addToStages ( stage ) ;  // add stage to candidate 
    
    // lock the stage!
    Hlt::Stage::Lock lock { stage , this } ;
    lock.addToHistory ( m_input ) ;
    stage->set ( track ) ;
    
    // insert the candidate into output selection 
    m_selection->push_back( candidate ) ;
  }
  //
  // a bit of monitoring 
  counter ( "#input"  ) +=  tracks      -> size  () ;
  counter ( "#output" ) +=  m_selection -> size  () ;
  counter ( "#accept" ) += !m_selection -> empty () ;
  m_selection -> setDecision( !m_selection->empty() );
  
  setFilterPassed ( ! m_selection -> empty() ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// finalize the algorithm 
// ============================================================================
StatusCode Hlt::Track2Candidate::finalize () 
{
  m_selection = nullptr ;
  return Hlt::Base::finalize () ;
}
// ============================================================================
// the algorithm factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Hlt,Track2Candidate)
// ============================================================================
// The END 
// ============================================================================  
