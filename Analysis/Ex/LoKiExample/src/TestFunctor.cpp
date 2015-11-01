// $Id: TestFunctor.cpp,v 1.2 2008-08-04 09:55:23 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHybridFactory.h"
#include "LoKi/ParticleCuts.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class TestFunctor
   *  Simple class which illustrates how one can test LoKi/Bender 
   *  (python) functors&predicates 
   *  directly in C++ 
   *
   *  @attention It has sense only for debugging of the python strings!
   * 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-06-12
   */
  class TestFunctor : public DaVinciAlgorithm
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<LoKi::TestFunctor> ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialization of the algorithm
    virtual StatusCode initialize () 
    {
      StatusCode sc = DaVinciAlgorithm::initialize() ;
      if ( sc.isFailure()  ) { return sc ; }                        // RETURN 
      //
      // locate the factory:
      LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory ) ;
      //
      // use the factory to instantiate the functors
      //
      // create the function:
      //
      sc = factory->get ( m_funCode , m_fun ) ;
      if ( sc.isFailure () ) { return sc ; }                        // RETURN 
      info() << " The created function  is '" << m_fun << "'" << endmsg ;
      //
      // create the predicate:
      //
      sc = factory->get ( m_cutCode , m_cut ) ;
      if ( sc.isFailure () ) { return sc ; }                        // RETURN 
      info() << " The created predicate is '" << m_cut << "'" << endmsg ;
      //
      return StatusCode::SUCCESS ;  
    }
    // ========================================================================
    /// LoKi/Bender functor factory itself:
    /// execute the algorithm
    virtual StatusCode execute () ;
    // ========================================================================
  protected:
    /** standard constructor
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    TestFunctor
    ( const std::string& name , // algorithm instance name 
      ISvcLocator*       pSvc ) // pointer to Service Locator 
      : DaVinciAlgorithm ( name , pSvc ) 
      //
      , m_fun ( LoKi::Constant<const LHCb::Particle*,double>( -1.0e+10 ) ) 
      , m_cut ( LoKi::Constant<const LHCb::Particle*,bool>  ( false    ) )
      //
      , m_factory ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC" ) 
      //
      , m_funCode ( "P"    )
      , m_cutCode ( "NONE" )
    {
      declareProperty ( "Factory" , m_factory , "The type/name of factory"   ) ;
      declareProperty ( "FunCode" , m_funCode , "The code for the function"  ) ;
      declareProperty ( "CutCode" , m_cutCode , "The code for the predicate" ) ;
    } 
    /// virtual & protected destructor 
    virtual ~TestFunctor() {} // virtual & protected destructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    TestFunctor () ;                     // the default constructor is disabled 
    /// the copy constructor is disabled 
    TestFunctor ( const TestFunctor& ) ;    // the copy constructor is disabled 
    /// the assignement operator is disabled 
    TestFunctor& operator=( const TestFunctor& ) ;   // no assignement operator
    // ========================================================================
  private:
    // ========================================================================
    /// the functor ("function"), return double  
    LoKi::Types::Fun m_fun ; // the functor ("function")
    // ========================================================================
    /// the functor ("predicate"), return bool 
    LoKi::Types::Cut m_cut ; // the functor ("predicate")
    // ========================================================================
    /// LoKi/Bender factory type/name 
    std::string m_factory     ;    // LoKi/Bender factory type/name 
    /// The code for fuction 
    std::string m_funCode     ;    // The code for fuction 
    /// The code for predicate 
    std::string m_cutCode     ;    // The code for predicate 
    // ========================================================================    
  } ;
  // ==========================================================================
} // end of namespaced LoKi 
// ============================================================================
StatusCode LoKi::TestFunctor::execute()
{
  // I hate long names...
  typedef  LHCb::Particle::Range PARTICLES ;
  
  // get the particles from desktop 
  const PARTICLES particles = this->particles() ;
  
  // do something with functors, e.g. print them:
  for ( PARTICLES::const_iterator iparticle = particles.begin() ; 
        particles.end() != iparticle ; ++iparticle ) 
  {
    const LHCb::Particle* p = *iparticle ;
    
    // use the function and predicate

    const double dvalue = m_fun ( p ) ;      // USE THE FUNCTION ==============  
    const bool   bvalue = m_cut ( p ) ;      // USE THE PREDICATE =============
    
    info () << " Test the function:  " << m_fun << "(p)=" << dvalue << endmsg ;
    info () << " Test the predicate: " << m_cut << "(p)=" <<
      ( bvalue ? "True" : "False" ) << endmsg ;

    counter ( "funValue") += dvalue ;
    counter ( "cutEff"  ) += bvalue ;
    
  }
  
  return StatusCode::SUCCESS ;
} 
// ============================================================================
/// the algorithm factory:
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,TestFunctor)
// ============================================================================
// The END 
// ============================================================================


