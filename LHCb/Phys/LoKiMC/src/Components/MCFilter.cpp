// $Id$
// ============================================================================
// Include files 
// ============================================================================
// MCEvent 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/MCTypes.h"
#include "LoKi/IMCHybridFactory.h"
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
  /** @class MCFilter 
   *  Simple filtering algorithm bases on LoKi/Bender "hybrid" framework
   *  for filtering according to MC -information 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-06-02
   */
  class MCFilter : public LoKi::FilterAlg 
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<LoKi::MCFilter> ;
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
      StatusCode sc = i_decode<LoKi::IMCHybridFactory> ( m_cut ) ;
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
    MCFilter
    ( const std::string& name ,    // the algorithm instance name 
      ISvcLocator*       pSvc ) ; // pointer to the service locator
    /// virtual and protected destructor 
    virtual ~MCFilter () {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    MCFilter () ;                        // the default constructor is disabled 
    /// the copy constructor is disabled 
    MCFilter ( const MCFilter& ) ;          // the copy constructor is disabled 
    /// the assignement operator is disabled 
    MCFilter& operator=( const MCFilter& ) ;     // the assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::Types::MCCutVal m_cut ; // the functor itself 
    /// TES location of LHCb::MCParticle::Container object 
    std::string m_location ;     // TES location of LHCb::MCParticle::Container
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
namespace 
{
  // ==========================================================================
  LoKi::BasicFunctors<LHCb::MCParticle::ConstVector>::BooleanConstant s_NONE = 
    LoKi::BasicFunctors<LHCb::MCParticle::ConstVector>::BooleanConstant ( false ) ;
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
LoKi::MCFilter::MCFilter
( const std::string& name , // the algorithm instance name 
  ISvcLocator*       pSvc ) // pointer to the service locator
  : LoKi::FilterAlg ( name , pSvc ) 
// the functor itself
  , m_cut      ( s_NONE ) 
// TES location of LHCb::MCParticle::Constainer object
  , m_location ( LHCb::MCParticleLocation::Default ) 
{
  //
  declareProperty 
    ( "Location" , 
      m_location , 
      "TES location of LHCb::MCParticle::Container object" ) ;    
  //
  StatusCode sc = setProperty ( "Code" , "~MCEMPTY" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Code'"    , sc ) ;
  sc = setProperty 
    ( "Factory" , "LoKi::Hybrid::MCTool/MCFactory:PUBLIC" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Factory'" , sc ) ;
}
// ============================================================================
// finalize 
// ============================================================================
StatusCode LoKi::MCFilter::finalize ()
{
  m_cut = s_NONE ;
  return LoKi::FilterAlg::finalize () ;
}
// ============================================================================
// the main method: execute 
// ============================================================================
StatusCode LoKi::MCFilter::execute () // the main method: execute 
{
  if ( updateRequired() ) 
  {
    StatusCode sc = decode() ; 
    Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  }
  //
  // get MC information from TES 
  //
  const LHCb::MCParticle::Container* particles = 
    get<LHCb::MCParticle::Container>( m_location ) ;
  if ( 0 == particles ) { return StatusCode::FAILURE ; }
  //
  // copy all particles into single vector 
  // 
  LHCb::MCParticle::ConstVector parts ( particles -> begin () , 
                                        particles -> end   () ) ;
  // 
  // use the functor 
  // 
  const bool result = m_cut ( parts ) ;
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
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,MCFilter)
// ============================================================================
// The END 
// ============================================================================
