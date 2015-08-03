// $Id: L0Filter.cpp,v 1.3 2010-01-08 13:30:47 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// L0Event
// ============================================================================
#include "Event/L0DUReport.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/L0Types.h"
#include "LoKi/IHltFactory.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class L0Filter 
   *  Simple filtering algorithm bases on LoKi/Bender "hybrid" approach
   *  for filtering according to L0DUReport
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2008-09-23
   */
  class L0Filter : public LoKi::FilterAlg 
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<LoKi::L0Filter> ;
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
    L0Filter
    ( const std::string& name , // the algorithm instance name 
      ISvcLocator*       pSvc ) // pointer to the service locator
      : LoKi::FilterAlg ( name , pSvc ) 
    // the functor itself
      , m_cut ( LoKi::BasicFunctors<const LHCb::L0DUReport*>::BooleanConstant( false ) ) 
    // TES location of LHCb::L0DUReport object
      , m_location ( LHCb::L0DUReportLocation::Default )
    // check the validity of L0DUReport 
      , m_check    ( true )
    {
      //
      declareProperty 
        ( "Location" , 
          m_location , 
          "TES location of LHCb::L0DUReport object" ) ;    
      //
      declareProperty 
        ( "CheckValidity" , 
          m_check         , 
          "Check validity of LHCb::L0DUReport object before using the functor" ) ;    
      //
      StatusCode sc = setProperty ( "Code" , "L0_NONE" ) ;
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
    virtual ~L0Filter () {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    L0Filter () ;                        // the default constructor is disabled 
    /// the copy constructor is disabled 
    L0Filter ( const L0Filter& ) ;          // the copy constructor is disabled 
    /// the assignement operator is disabled 
    L0Filter& operator=( const L0Filter& ) ;     // the assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::Types::L0_Cut    m_cut ;                        // the functor itself 
    /// TES location of LHCb::L0DUReport object 
    std::string m_location ;         // TES location of LHCb::L0DUReport object 
    /// check the validity of L0DUReport
    bool        m_check    ;                // check the validity of L0DUReport
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// the main method: execute 
StatusCode LoKi::L0Filter::execute () // the main method: execute 
{
  if ( updateRequired() ) 
  {
    StatusCode sc = decode() ; 
    Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  }
  // get LHCb::L0DUReport from TES 
  const LHCb::L0DUReport* l0 = get<LHCb::L0DUReport> ( m_location ) ;
  //
  bool result = false ;
  //
  if ( m_check && !l0->valid() ) 
  {
    static const StatusCode ok = StatusCode ( StatusCode::SUCCESS , true ) ;
    Error ( "LHCb::L0DUReportObject is invalid, return 'false'", ok ).ignore() ;
    result = false ;
  }
  else 
  {
    /// use the functor 
    result = m_cut ( l0 ) ;   /// use the functor 
  }
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
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,L0Filter)
// ============================================================================
// The END
// ============================================================================

