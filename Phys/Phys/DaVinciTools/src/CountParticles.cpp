// $Id: CountParticles.cpp,v 1.1 2010-01-06 08:23:46 pkoppenb Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
// ============================================================================
/** @class CountParticles 
 *  Count particles per PID. Useful for tests.
 *  @author Patrick Koppenburg
 *  @date 2010-01-06
 */
class CountParticles : public DVAlgorithm
{
  // ==========================================================================
  // the friend factory for instantiation 
  friend class AlgFactory<CountParticles> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the standard execution of the algorithm
  virtual StatusCode execute() 
  {
    // get the particles 
    LHCb::Particle::ConstVector parts = desktop()->particles();
    //
    setFilterPassed ( !parts.empty() ) ;
    //
    for ( LHCb::Particle::ConstVector::const_iterator i = parts.begin() ; 
          i!= parts.end() ; ++i){
      const LHCb::ParticleProperty* pp = ppSvc()->find((*i)->particleID());
      counter(pp->name())++;
    }

    //
    return StatusCode::SUCCESS ;
  }
  /// the standard finalization of the algorithm
  virtual StatusCode finalize () 
  {
    return DVAlgorithm::finalize () ;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /** the standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc service locator 
   */
  CountParticles 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : DVAlgorithm ( name , pSvc ) 
  {
  }
  /// virtual & protected destructor 
  virtual ~CountParticles () {}
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  CountParticles () ; // the default constructor is disabled 
  /// the copy constructor is disabled 
  CountParticles ( const CountParticles& ) ; // no copy constructor 
  /// the assignement operator is disabled 
  CountParticles& operator=( const CountParticles& ) ; // no assignement
  // ==========================================================================
private:
  // ==========================================================================
  // ==========================================================================
};
// ============================================================================
/// declare the factory (needed for instantiation)
DECLARE_ALGORITHM_FACTORY(CountParticles) ;
// ============================================================================
// The END 
// ============================================================================
