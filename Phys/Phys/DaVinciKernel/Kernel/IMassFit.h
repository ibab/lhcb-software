// $Id: IMassFit.h,v 1.1 2008-01-15 18:16:52 ibelyaev Exp $
// ============================================================================
#ifndef KERNEL_IMASSCONSTRAINEDFIT_H 
#define KERNEL_IMASSCONSTRAINEDFIT_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleReFitter.h"
// ============================================================================
/** @class IMassFit Kernel/IMassFit.h
 *
 *  The simple abstract interface to apply mass-constrained fit for the 
 *  particles. It inherits from the basic abstarct interface IParticleReFitter
 *  and contains twho basic methods:
 *     - (re)fit the particle with explicit mass constraint
 *     - (re)fit the particle into the nominal mass
 *
 *  It is expected that the method IParticleReFitter::reFit is reimplemented 
 *  through @c fit:
 *
 *  @code
 *
 *    StatusCode reFit ( LHCb::Particle& particle ) const 
 *     {
 *       return fit ( particle ) ;
 *     }
 *
 *  @endcode
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2008-01-09
 */
class IMassFit : public virtual IParticleReFitter 
{
public: 
  // ==========================================================================
  /** perform the mass-constrained fit of the particle into the specified mass
   *
   *  @code
   *
   *  LHCb::Particle* B = ... ;
   *
   *  IMassFit* fitter = ... ;
   *
   *  StatusCode sc = fitter -> fit ( B , 5.279 * GeV ) ;
   *  if ( sc.isFailure() ) { .. error here ... } 
   *
   *  @endcode
   * 
   *
   *  @param particle (in/out) pointer to the particle 
   *  @param mass     (in)     mass to be constrained 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( LHCb::Particle* particle , const double mass ) const = 0 ;
  // ==========================================================================
  /** perform the mass-constrained fit of the particle into the nominal mass
   *
   *  @code
   *
   *  LHCb::Particle* B = ... ;
   *
   *  IMassFit* fitter = ... ;
   *
   *  StatusCode sc = fitter -> fit ( B ) ;
   *  if ( sc.isFailure() ) { .. error here ... } 
   *
   *  @endcode
   * 
   *
   *  @param particle (in/out) pointer to the particle 
   *  @return status code 
   */
  virtual StatusCode fit ( LHCb::Particle* particle ) const = 0 ;
  // ==========================================================================
public:
  // ==========================================================================
  /** perform the mass-constrained fit of the particle into the specified mass
   *
   *  @code
   *
   *  LHCb::Particle& B = ... ;
   *
   *  IMassFit* fitter = ... ;
   *
   *  StatusCode sc = fitter -> fit ( B , 5.279 * GeV ) ;
   *  if ( sc.isFailure() ) { .. error here ... } 
   *
   *  @endcode
   * 
   *  @param particle (in/out) reference to the particle 
   *  @param mass     (in)     mass to be constrained 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( LHCb::Particle& particle , const double mass ) const
  { return fit ( &particle , mass ) ; }
  // ==========================================================================
  /** perform the mass-constrained fit of the particle into the nominal mass
   *
   *  @code
   *
   *  LHCb::Particle& B = ... ;
   *
   *  IMassFit* fitter = ... ;
   *
   *  StatusCode sc = fitter -> fit ( B ) ;
   *  if ( sc.isFailure() ) { .. error here ... } 
   *
   *  @endcode
   * 
   *  @param particle  (in/out) reference to the particle 
   *  @return status code 
   */
  virtual StatusCode fit ( LHCb::Particle& particle ) const 
  { return fit ( &particle ) ; }
  // ==========================================================================
public:
  // ==========================================================================
  /** perform the mass-constrained fit of the particle into 
   *  the specified mass and explicitly return chi2
   *
   *  @code
   *
   *  LHCb::Particle* B = ... ;
   *
   *  IMassFit* fitter = ... ;
   *
   *  double chi2 = 0.0 ;
   *  StatusCode sc = fitter -> fit ( B , 5.279 * GeV , chi2 ) ;
   *  if ( sc.isFailure() ) { .. error here ... } 
   *
   *  @endcode
   * 
   *  @param particle (in.out) pointer to the particle 
   *  @param mass     (in)     mass to be constrained 
   *  @param chi2     (in/out) chi2 of the mass constrained fit 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( LHCb::Particle* particle , const double mass , double& chi2 ) const = 0 ;
  // ==========================================================================
  /** perform the mass-constrained fit of the particle into 
   *  the specified mass and explicitly return chi2
   *
   *  @code
   *
   *  LHCb::Particle& B = ... ;
   *
   *  IMassFit* fitter = ... ;
   *
   *  double chi2 = 0.0 ;
   *  StatusCode sc = fitter -> fit ( B , 5.279 * GeV , chi2 ) ;
   *  if ( sc.isFailure() ) { .. error here ... } 
   *
   *  @endcode
   * 
   *  @param particle (in.out) reference to the particle 
   *  @param mass     (in)     mass to be constrained 
   *  @param chi2     (in/out) chi2 of the mass constrained fit 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( LHCb::Particle& particle , const double mass , double& chi2 ) const 
  { return fit ( &particle, mass, chi2 ) ; }
  // ==========================================================================
public:
  // ==========================================================================
  /** The method which allows to fit a sequence of particles. 
   * 
   *  @code 
   *  
   *  // locate the tool 
   *  const IMassFit* fitter = ... ;
   * 
   *  // refit *ALL* Bs  
   *  LHCb::Particle:Container* allB = get<Particle>( "/Event/Phys/B" ) ;
   * 
   *  StatusCode sc = fitter->fit( allB->begin() , allB->end() , 5.279 * GeV ) ;
   *
   *  @endcode 
   * 
   *  @param begin  begin-iterator for sequence of particles
   *  @param end    end-iterator for sequence of particles
   *  @param mass     mass to be constrained 
   *  @return status code 
   */
  template <class PARTICLE>
  inline StatusCode fit 
  ( PARTICLE        begin , 
    PARTICLE        end   , 
    const double    mass  ) const 
  {
    StatusCode code = StatusCode::SUCCESS ;
    for( ; begin != end ; ++begin ) 
    {
      StatusCode sc = fit ( *begin , mass ) ;
      if ( sc.isFailure() ) { code = sc ; }
    }
    return code ;
  } 
  // ==========================================================================
  /** The method which allows to fit a sequence of particles to the nominal mass
   * 
   *  @code 
   *  
   *  // locate the tool 
   *  const IMassFit* fitter = ... ;
   * 
   *  // refit *ALL* Bs  
   *  LHCb::Particle:Container* allB = get<Particle>( "/Event/Phys/B" ) ;
   * 
   *  StatusCode sc = fitter->fit( allB->begin() , allB->end() ) ;
   *
   *  @endcode 
   * 
   *  @param begin  begin-iterator for sequence of particles
   *  @param end    end-iterator for sequence of particles
   *  @return status code 
   */
  template <class PARTICLE>
  inline StatusCode fit 
  ( PARTICLE        begin , 
    PARTICLE        end   ) const 
  {
    StatusCode code = StatusCode::SUCCESS ;
    for( ; begin != end ; ++begin ) 
    {
      StatusCode sc = fit ( *begin ) ;
      if ( sc.isFailure() ) { code = sc ; }
    }
    return code ;
  } 
  // ==========================================================================
public:
  /// the unique interface identgifier 
  static const InterfaceID& interfaceID() ;
protected:
  // virtual and protected destructor  
  virtual ~IMassFit( ); ///< Destructor
};
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_IMASSCONSTRAINEDFIT_H
// ============================================================================
