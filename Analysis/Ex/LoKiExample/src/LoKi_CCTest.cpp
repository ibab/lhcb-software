// $Id: LoKi_CCTest.cpp,v 1.1 2008-08-04 11:00:59 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/CC.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::CCTest
 *  simple test for CC-operation 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-08-04
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class CCTest   CCTest.cpp
   *  Simple test for LoKi::cc utility 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-08-04
   */
  class CCTest : public GaudiAlgorithm 
  {
    // ========================================================================
    // friend class for algorithm instantiation
    friend class AlgFactory<LoKi::CCTest> ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialization of the algorithm 
    virtual StatusCode initialize () 
    {
      // initialize the base 
      StatusCode sc = GaudiAlgorithm::initialize () ;
      if ( sc.isFailure () ) { return sc ; }
      // load LoKi 
      svc<IService>( "LoKiSvc" , true ) ; // load LoKi 
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
    /// the actual execution of the algorithm 
    virtual StatusCode execute () 
    {
      //
      for ( DECAYS::const_iterator id = m_decays.begin() ; 
            m_decays.end() != id ; ++id ) 
      {
        always () 
          << " The decay: '" <<              ( *id ) << "'" << endreq 
          << " CC-decay : '" << LoKi::CC::cc ( *id ) << "'" << endreq ;
      }   
      //
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constrcutor 
     *  @param name the algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    CCTest ( const std::string& name ,   // Algorithm instance name 
             ISvcLocator*       pSvc )   // Service Locator
      : GaudiAlgorithm ( name , pSvc ) 
      , m_decays () 
    {
      declareProperty ( "Decays" , 
                        m_decays , 
                        "List of decays to be 'cc'-fied" ) ;
    }
    /// virtual and protected desctructor 
    virtual ~CCTest() {} // virtual and protected desctructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    CCTest () ;                          // the default constructor is disabled 
    /// the copy constructor is disabled 
    CCTest ( const CCTest& ) ;              // the copy constructor is disabled 
    /// the assigenemet operator is disabled 
    CCTest& operator=( const CCTest& ) ;     // assigement operator is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the actual type for the list of decays 
    typedef std::vector<std::string> DECAYS ; // the list of decays
    // ========================================================================
  private:
    // ========================================================================
    /// the list of decays to be "cc-fied":
    DECAYS m_decays ; // the list of decays to be "cc-fied"
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,CCTest) 
// ============================================================================
// The END 
// ============================================================================
