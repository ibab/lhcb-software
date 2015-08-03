// $Id: ODINFilter.cpp,v 1.2 2009-05-20 08:57:40 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// DAQEvent
// ============================================================================
#include "Event/ODIN.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/OdinTypes.h"
#include "LoKi/IHltFactory.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class ODINFilter 
   *  Simple filtering algorithm bases on LoKi/Bender "hybrid" approach
   *  for filtering according to ODIN bank
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2008-09-23
   */
  class ODINFilter : public LoKi::FilterAlg 
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<LoKi::ODINFilter> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the main method: execute 
    virtual StatusCode execute () ;
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
      StatusCode sc = i_decode<LoKi::Hybrid::IHltFactory> ( m_cut ) ;
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
    ODINFilter
    ( const std::string& name , // the algorithm instance name 
      ISvcLocator*       pSvc ) // pointer to the service locator
      : LoKi::FilterAlg ( name , pSvc ) 
      // the functor itself
      , m_cut ( LoKi::BasicFunctors<const LHCb::ODIN*>::BooleanConstant( false ) ) 
      // TES location of LHCb::ODIN object
      , m_location ( LHCb::ODINLocation::Default ) 
    {
      //
      declareProperty 
        ( "Location" , 
          m_location , 
          "TES location of LHCb::ODIN object" ) ;    
      //
      StatusCode sc = setProperty ( "Code" , "ODIN_NONE" ) ;
      Assert ( sc.isSuccess () , "Unable (re)set property 'Code'"    , sc ) ;
      sc = setProperty 
        ( "Factory" , 
          0 == name.find ( "Hlt1" ) ? 
          "LoKi::Hybrid::HltFactory/Hlt1HltFactory:PUBLIC" : 
          0 == name.find ( "Hlt2" ) ?
          "LoKi::Hybrid::HltFactory/Hlt2HltFactory:PUBLIC" :
          "LoKi::Hybrid::HltFactory/HltFactory:PUBLIC"     ) ;
      Assert ( sc.isSuccess () , "Unable (re)set property 'Factory'" , sc ) ;
    } 
    /// virtual and protected destructor 
    virtual ~ODINFilter () {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    ODINFilter () ;                      // the default constructor is disabled 
    /// the copy constructor is disabled 
    ODINFilter ( const ODINFilter& ) ;      // the copy constructor is disabled 
    /// the assignement operator is disabled 
    ODINFilter& operator=( const ODINFilter& ) ; // the assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::Types::ODIN_Cut  m_cut ;                        // the functor itself 
    /// TES location of LHCb::ODIN object 
    std::string m_location ;               // TES location of LHCb::ODIN object 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// the main method: execute 
StatusCode LoKi::ODINFilter::execute () // the main method: execute 
{
  if ( updateRequired() ) 
  {
    StatusCode sc = decode() ; 
    Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  }
  // get LHCb::ODIN from TES 
  const LHCb::ODIN* odin = get<LHCb::ODIN> ( m_location ) ;
  // 
  // use the functor 
  // 
  const bool result = m_cut ( odin ) ;
  //
  // some statistics
  counter ("#passed" ) += result ;
  // 
  // set the filter:
  setFilterPassed ( result ) ;
  //
  return StatusCode::SUCCESS ;
}  
// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,ODINFilter)
// ============================================================================
// The END
// ============================================================================

