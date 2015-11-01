// $Id: LoKi_CCTest.cpp,v 1.2 2008-12-06 16:31:28 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
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
      for ( const auto& d : m_decays )
      {
        always () 
          << " The decay: '" <<              ( d ) << "'" << endmsg
          << " CC-decay : '" << ppSvc()-> cc ( d ) << "'" << endmsg ;
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
      , m_decays (   ) 
      , m_ppSvc  ( 0 )
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
  protected:
    // ========================================================================
    // get the particle property service 
    inline const LHCb::IParticlePropertySvc* ppSvc() const 
    {
      if ( !m_ppSvc ) m_ppSvc = svc<LHCb::IParticlePropertySvc>
        ( "LHCb::ParticlePropertySvc" , true ) ;
      return m_ppSvc ;                                            
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the list of decays to be "cc-fied":
    std::vector<std::string> m_decays ; // the list of decays to be "cc-fied"
    /// the particle properties service 
    mutable const LHCb::IParticlePropertySvc* m_ppSvc ; // the service 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,CCTest) 
// ============================================================================
// The END 
// ============================================================================
