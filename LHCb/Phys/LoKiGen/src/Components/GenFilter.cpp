// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GenEvent 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/GenTypes.h"
#include "LoKi/IGenHybridFactory.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class ODINFilter 
   *  Simple filtering algorithm bases on LoKi/Bender "hybrid" framework
   *  for filtering according to Generator (HepMC) information 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-06-02
   */
  class GenFilter : public LoKi::FilterAlg 
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<LoKi::GenFilter> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the main method: execute
    virtual StatusCode execute  () ;
    virtual StatusCode finalize () ;    
    // ========================================================================
  public:
    // ========================================================================
    /** Decode the functor (use the factory)
     *  @see LoKi::FilterAlg
     *  @see LoKi::FilterAlg::decode
     *  @see LoKi::FilterAlg::i_decode
     */
    virtual StatusCode decode () 
    {
      StatusCode sc = i_decode<LoKi::IGenHybridFactory> ( m_cut ) ;
      Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @see LoKi::FilterAlg 
     *  @see GaudiAlgorithm 
     *  @see      Algorithm 
     *  @see      AlgFactory
     *  @see     IAlgFactory
     *  @param name the algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    GenFilter
    ( const std::string& name ,    // the algorithm instance name 
      ISvcLocator*       pSvc ) ; // pointer to the service locator
    /// virtual and protected destructor 
    virtual ~GenFilter () {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    GenFilter () ;                       // the default constructor is disabled 
    /// the copy constructor is disabled 
    GenFilter ( const GenFilter& ) ;        // the copy constructor is disabled 
    /// the assignement operator is disabled 
    GenFilter& operator=( const GenFilter& ) ;   // the assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::Types::GCutVal m_cut ; // the functor itself 
    /// TES location of LHCb::HepMCEvent::Container object 
    std::string m_location ;     // TES location of LHCb::HepMCEvent::Container
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
namespace 
{
  // ==========================================================================
  LoKi::BasicFunctors<LoKi::GenTypes::GenContainer>::BooleanConstant s_NONE = 
    LoKi::BasicFunctors<LoKi::GenTypes::GenContainer>::BooleanConstant ( false ) ;
  // ==========================================================================
}
// ============================================================================
/* standard constructor 
 *  @see LoKi::FilterAlg 
 *  @see GaudiAlgorithm 
 *  @see      Algorithm 
 *  @see      AlgFactory
 *  @see     IAlgFactory
 *  @param name the algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ===========================================================================
LoKi::GenFilter::GenFilter
( const std::string& name , // the algorithm instance name 
  ISvcLocator*       pSvc ) // pointer to the service locator
  : LoKi::FilterAlg ( name , pSvc ) 
// the functor itself
  , m_cut      ( s_NONE ) 
// TES location of LHCb::HEpMCEvent::Constainer object
  , m_location ( LHCb::HepMCEventLocation::Default ) 
{
  //
  declareProperty 
    ( "Location" , 
      m_location , 
      "TES location of LHCb::HepMCEvent::Container object" ) ;    
  //
  StatusCode sc = setProperty ( "Code" , "~GEMPTY" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Code'"    , sc ) ;
  sc = setProperty 
    ( "Factory" , "LoKi::Hybrid::GenTool/GenFactory:PUBLIC" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Factory'" , sc ) ;
}
// ============================================================================
// finalize 
// ============================================================================
StatusCode LoKi::GenFilter::finalize ()
{
  m_cut = s_NONE ;
  return LoKi::FilterAlg::finalize () ;
}
// ============================================================================
// the main method: execute 
// ============================================================================
StatusCode LoKi::GenFilter::execute () // the main method: execute 
{
  if ( updateRequired() ) 
  {
    StatusCode sc = decode() ; 
    Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  }
  //
  // get HepMC information from TES 
  //
  const LHCb::HepMCEvent::Container* events = 
    get<LHCb::HepMCEvent::Container>( m_location ) ;
  if ( 0 == events ) { return StatusCode::FAILURE ; }
  //
  // copy all particles into single vector 
  // 
  LoKi::GenTypes::GenContainer particles ;
  for ( LHCb::HepMCEvent::Container::const_iterator ievent = 
          events->begin() ; events->end() != ievent ; ++ievent )
  {
    const LHCb::HepMCEvent* event = *ievent ;
    if ( 0 == event ) { continue ; }                       // CONTINUE 
    //
    const HepMC::GenEvent* evt =  event->pGenEvt() ;
    if ( 0 == evt   ) { continue ; }                       // CONTINUE 
    //
    particles.insert ( particles.end          () , 
                       evt -> particles_begin () , 
                       evt -> particles_end   () ) ;
  }
  // 
  // use the functor 
  // 
  const bool result = m_cut ( particles ) ;
  //
  // some statistics
  //
  counter ("#passed" ) += result ;
  // 
  // set the filter:
  //
  setFilterPassed ( result ) ;
  //
  return StatusCode::SUCCESS ;
}  
// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,GenFilter)
// ============================================================================
// The END 
// ============================================================================
