// $Id$
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
// ============================================================================
/** @class CountParticles 
 *  Count particles per PID. Useful for tests.
 *  @author Patrick Koppenburg
 *  @date 2010-01-06
 */
class CountParticles : public DaVinciAlgorithm
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
    const LHCb::Particle::ConstVector& parts = this->i_particles();
    //
    setFilterPassed ( !parts.empty() ) ;
    //
    for ( LHCb::Particle::ConstVector::const_iterator i = parts.begin() ; 
          i != parts.end(); ++i )
    {
      const LHCb::ParticleProperty * pp = ppSvc()->find((*i)->particleID());
      if ( pp ) { ++counter(pp->name()); }
    }
    //
    return StatusCode::SUCCESS ;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /** the standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc service locator 
   */
  CountParticles ( const std::string& name , 
                   ISvcLocator*       pSvc ) 
    : DaVinciAlgorithm ( name , pSvc ) 
  { }
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
DECLARE_ALGORITHM_FACTORY(CountParticles)
// ============================================================================
// The END 
// ============================================================================
