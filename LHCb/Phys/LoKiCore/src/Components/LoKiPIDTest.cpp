// $Id: LoKiPIDTest.cpp,v 1.3 2007-08-22 17:25:27 pkoppenb Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h" 
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
namespace LoKi 
{
  /** @class PIDTest  LoKiPIDest.cpp
   *  Simple algorithm to veryfy teh fast acces to PID information
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-065-31
   */
  class PIDTest : public GaudiAlgorithm
  {
    // the friend factory for instantiation
    friend class AlgFactory<PIDTest> ;
  public:
    /// standard initialzation
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiAlgorithm::initialize() ;
      if ( sc.isFailure() ) { return sc ; }                     // RETURN 
      svc<LoKi::ILoKiSvc>("LoKiSvc",true) ;
      m_svc = svc<IParticlePropertySvc>("ParticlePropertySvc",true) ;
      Assert ( 0 != m_svc , "Invalid pointer to  Particle Properety Service!" ) ;
      return StatusCode::SUCCESS ;                              // RETURN
    }
    virtual StatusCode execute () 
    {
      // avoid long names 
      using namespace LoKi::Particles ;
      // check
      Assert ( 0 != m_svc , "Invalid pointer to  Particle Properety Service!" ) ;
      // loop over properties:
      for ( IParticlePropertySvc::const_iterator ipp = m_svc->begin() ; 
            m_svc->end() != ipp ; ++ipp ) 
      {
        const ParticleProperty* pp = *ipp ;
        if ( 0 == pp ) { continue ; }                            // RETURN
        debug() << "Name/PID: "
                << "'" << pp->particle() << "':" << pp->pdgID() << endreq ;
        // verify name -> pid map  
        Assert ( pp->particle() == nameFromPID ( LHCb::ParticleID( pp->pdgID    () ))       , 
                 "Mismatch in Name/PID map for '" + pp->particle() + "'" ) ;
        // verify pid  -> name map  
        Assert ( pp->pdgID   () == pidFromName ( pp->particle () ).pid() , 
                 "Mismatch in PID/Name map for '" + pp->particle() + "'" ) ;
      }
      return StatusCode::SUCCESS ;
    } 
  protected:
    /// standard constructor 
    PIDTest 
    ( const std::string& name ,
      ISvcLocator*       pSvc )
      : GaudiAlgorithm ( name , pSvc )
      , m_svc ( 0 ) 
    {}
    /// virtual protected constructor 
    virtual ~PIDTest() {}
  private:
    // default constructror is disabled 
    PIDTest() ; ///< no default constructor 
    // copy constructror is disabled 
    PIDTest( const PIDTest& ) ; ///< no copy 
    // assignement is disabled 
    PIDTest& operator=( const PIDTest& ) ; ///< no assignement
  private:
    // The pointer to particle property sevice
    IParticlePropertySvc* m_svc ; ///< The pointer to particle property sevice
  } ;
} // end of namespace LoKi 
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,PIDTest);
// ============================================================================
// The END 
// ============================================================================
