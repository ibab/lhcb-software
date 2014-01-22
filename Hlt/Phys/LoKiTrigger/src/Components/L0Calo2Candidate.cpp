// $Id$
// ============================================================================
// $URL$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/L0CaloCandidate.h"
#include "Event/L0DUReport.h"
// ============================================================================
// HltBase
// ============================================================================
#include "Event/HltCandidate.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltBase.h"
// ============================================================================
/** @file
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
namespace Hlt
{
  // ==========================================================================
  /** @class L0Calo2Candidate
   *  Simple class which converts L0Calo candidates into Hlt::Candidate
   *  @see Hlt::Candidate
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-08-01
   */
  class L0Calo2Candidate : public Hlt::Base
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class AlgFactory<Hlt::L0Calo2Candidate> ;
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
    L0Calo2Candidate
    ( const std::string& name ,                  //     algorithm instance name
      ISvcLocator*       pSvc ) ;                 //  pointer to Service Locator
    /// virtual and protected destructor
    virtual ~L0Calo2Candidate () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    L0Calo2Candidate () ;                             // no default constructor
    /// the copy constructor is disabled
    L0Calo2Candidate ( const L0Calo2Candidate& ) ;       // no copy constructor
    /// the assignement operator is disabled
    L0Calo2Candidate & operator=( const L0Calo2Candidate& ) ; // no assignement
    // ========================================================================
  private:
    // ========================================================================
  private:
    // ========================================================================
    /// the selection
    Hlt::TSelection<Hlt::Candidate>* m_selection ;             // the selection
    /// the output selection
    Location m_output ;                                        //    the output
    /// TES Location of L0CaloCandidate
    Location m_input  ;                      // TES Location of L0CaloCandidate
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
Hlt::L0Calo2Candidate::L0Calo2Candidate
( const std::string& name ,                  //     algorithm instance name
  ISvcLocator*       pSvc )                  //  pointer to Service Locator
  : Hlt::Base   ( name , pSvc )
  , m_selection { nullptr }
  , m_output    { name }
  , m_input     { LHCb::L0CaloCandidateLocation ::Default }
//
{
  declareProperty
    ( "OutputSelection"                 ,
      m_output                          ,
      "The name of output selection of Hlt::Candidates"    ) ;
  declareProperty
    ( "InputSelection"                  ,
      m_input                           ,
      "TES Location of LHCb::L0CaloCandidate" ) ;
}
// ============================================================================
// virtual and protected destructor
// ============================================================================
Hlt::L0Calo2Candidate::~L0Calo2Candidate(){}
// ============================================================================
// initialize the algorithm
// ============================================================================
StatusCode Hlt::L0Calo2Candidate::initialize ()
{
  /// initialize the base
  StatusCode sc = Hlt::Base::initialize () ;
  if ( sc.isFailure() ) { return sc ; }                          // REUTRN
  /// Lock the service to enable the output selection registration
  Hlt::IRegister::Lock lock { regSvc() , this } ;
  /// register TES input selection
  sc = lock -> registerTESInput ( m_input     , this ) ;
  Assert ( sc.isSuccess () , "Unable to register INPUT  selection" , sc ) ;
  /// register the output selection
  m_selection = new Hlt::TSelection<Hlt::Candidate>{ m_output } ;
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
StatusCode Hlt::L0Calo2Candidate::execute  ()
{
  // some sanity checks:
  Assert ( m_selection , "Invalid Local pointer to selection" ) ;
  if ( !m_selection->empty() ) { Warning("Local selection is not empty!") ; }

  // get all L0 Calos from TES
  typedef LHCb::L0CaloCandidate::Container L0Calos ;
  const L0Calos* l0calos = tesData<L0Calos> ( m_input ) ;

  // get(or create) containter of Hlt-Candidates form TES
  Hlt::Candidate::Container*  candidates = hltCandidates () ;
  // get(or create) containter of Hlt-Stages     from TES
  Hlt::Stage::Container*      stages    =  hltStages     () ;

  // loop over L0 candidates:
  for ( const auto* calo : *l0calos ) {
    if ( !calo ) { continue ; }                                  // CONTINUE
    //
    // create the new candidate:
    Hlt::Candidate* candidate = new Hlt::Candidate() ;
    candidate  -> addToWorkers ( m_input ) ;
    candidate  -> addToWorkers ( this    ) ;
    candidates -> push_back    ( candidate )  ; // add candidate into TES container
    //
    // create the stage
    Hlt::Stage* stage = new Hlt::Stage() ;
    // add it into candidate
    stages     -> push_back   ( stage     )  ;  // add stage  into TES container
    // add it into Candidate!
    candidate  -> addToStages ( stage ) ;  // add stage to candidate

    // lock the stage!
    Hlt::Stage::Lock lock ( stage , this ) ;
    lock.addToHistory ( m_input ) ;
    stage->set (  calo ) ;

    // insert the candidate into output selection
    m_selection->push_back( candidate ) ;
  }
  //
  // a bit of monitoring
  counter ( "#input"  ) +=  l0calos     -> size  ();
  counter ( "#output" ) +=  m_selection -> size  ();
  counter ( "#accept" ) += !m_selection -> empty ();
  m_selection -> setDecision( !m_selection->empty() );

  setFilterPassed ( ! m_selection -> empty() ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================
// finalize the algorithm
// ============================================================================
StatusCode Hlt::L0Calo2Candidate::finalize ()
{
  m_selection = nullptr ;
  return Hlt::Base::finalize () ;
}
// ============================================================================
// the algorithm factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Hlt,L0Calo2Candidate)
// ============================================================================
// The END
// ============================================================================
