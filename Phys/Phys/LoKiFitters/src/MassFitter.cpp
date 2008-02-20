// $Id: MassFitter.cpp,v 1.1.1.1 2008-02-20 15:48:44 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h" 
#include "GaudiKernel/SystemOfUnits.h" 
#include "GaudiKernel/Vector4DTypes.h" 
#include "GaudiKernel/GenericVectorTypes.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h" 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IMassFit.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================
namespace LoKi
{
  /** @class MassFitter
   *  The most simple implementation of abstract interface IMassFit
   *  The tool performs the mass-constrained fit
   *
   *  @todo the tool LoKi::MassFitter is not properly tested yet!  
   *
   *  @see IMassFit
   *  @see IParticleRefitter
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2008-01-15
   */
  class MassFitter 
    : public virtual IMassFit
    , public GaudiTool 
  {
    // ========================================================================
    // the friend factory (needed for instantiuation)
    friend class ToolFactory<LoKi::MassFitter> ; ///< needed for instantiation
    // ========================================================================
  public:
    // ========================================================================
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
     *  @see IMassFit
     *
     *
     *  The machinery from P.Avery's lectures is used 
     *  @see http://www.phys.ufl.edu/~avery/fitting/kinematic.pdf
     *
     *  @param particle (in/out) pointer to the particle 
     *  @param mass     (in)     mass to be constrained 
     *  @return status code 
     */
    virtual StatusCode fit 
    ( LHCb::Particle* particle , const double mass ) const 
    {
      double chi2 = 0.0 ;
      return fit ( particle , mass , chi2 ) ;
    }
    // ========================================================================
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
     *  @see IMassFit
     *
     *  @param particle (in/out) pointer to the particle 
     *  @return status code 
     */
    virtual StatusCode fit ( LHCb::Particle* particle ) const 
    {
      if ( 0 == particle ) 
      { return Error ( "LHCb::Particle* points to NULL" ) ; }
      const ParticleProperty* prop = pp ( particle->particleID() ) ;
      if ( 0 == prop     )
      { return Error ( "Invalid LHCb::ParticleID" ) ; }
      return fit ( particle , prop->mass() ) ;
    }
    // ========================================================================
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
    ( LHCb::Particle* particle , const double mass , double& chi2 ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /** The basic method for "refit" of the particle
     *`
     *  @code 
     *
     *  // locate the tool 
     *  const IParticleReFitter* refitter = tool<IParticleRefitter>( ... ) ;
     * 
     *  // particle to be refit 
     *  LHCb::Particle* p = ... ;
     *
     *  StatusCode sc = refitter->reFit ( *p ) ;
     *  if ( sc.isFailure() ) { Warning("Error in reFit" , sc ) ; }
     *
     *  @endcode 
     *
     *  @see IParticleReFitter 
     *
     *  @param particle (in/out) reference to the particle 
     *  @return status code 
     */
    virtual StatusCode reFit ( LHCb::Particle& particle ) const 
    { return fit ( &particle ) ; }
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param type tool instance type(?)
     *  @param name tool instance name
     *  @param parent pointer to the parent interface 
     */
    MassFitter
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
      , m_ppSvc         ( 0 ) 
      , m_iterMax       ( 5 )
      , m_tolerance     ( 0.1 * Gaudi::Units::MeV ) 
      , m_change_vertex ( true )
    {
      //
      declareInterface<IMassFit>          ( this ) ;
      declareInterface<IParticleReFitter> ( this ) ;
      //
      declareProperty 
        ( "MaxIterations" , 
          m_iterMax       , 
          "The maximal number of iterations"    ) ;
      declareProperty 
        ( "Tolerance"     , 
          m_tolerance     , 
          "The absolute tolerance"              ) ; 
      declareProperty 
        ( "ChangeVertex"  , 
          m_change_vertex , 
          "The flag to allow the modification of vertex " ) ; 
    }
    /// virtual and protected destructor 
    virtual ~MassFitter() {}
    // ========================================================================
  private:
    // ========================================================================
    // The default constructor is disabled 
    MassFitter() ; ///< The default constructor is disabled 
    // The dcopy constructor is disabled 
    MassFitter ( const MassFitter& ) ; ///< The copy constructor is disabled 
    // The assigmenent operator is disabled 
    MassFitter& operator=( const MassFitter& ) ; ///< The assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// get the particle property for the given particle ID 
    inline const ParticleProperty* pp ( const LHCb::ParticleID& pid ) const 
    {
      // locate the service (if needed)
      if ( 0 == m_ppSvc )  
      { m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc" , true ) ; }  
      Assert ( 0 != m_ppSvc , "Unable to locate Particle Property Service" ) ;
      const ParticleProperty* p = m_ppSvc -> findByStdHepID( pid.pid() ) ;
      if ( 0 == p )
      {
        Error ( "Invalid ParticleProperty for ID=" + 
                boost::lexical_cast<std::string> ( pid.pid() ) ) ;
      }
      return p ;
    }  
    // ========================================================================
    /** check the mass of the particle 
     *  @param momentum 4-momemtum
     *  @param mass the reference mass 
     *  @return true is the mass is within the allowed interval
     */
    inline bool massOK 
    ( const Gaudi::LorentzVector& momentum , const double mass ) const 
    { return  fabs ( momentum.M() - mass ) <=  m_tolerance ; }
    // ========================================================================
    /** check the mass of the particle 
     *  @param particle the particle 
     *  @param mass the reference mass 
     *  @return true is the mass is within the allowed interval
     */
    inline bool massOK 
    ( const LHCb::Particle* particle , const double mass ) const 
    { return massOK ( particle -> momentum() , mass ) ; }
    // ========================================================================
  private:
    // ========================================================================
    // pointer to the particle property service 
    mutable IParticlePropertySvc* m_ppSvc     ; ///< the particle property service 
    // maximal number of iterations 
    unsigned int                  m_iterMax   ; ///< maximal number of iterations 
    // the tolerance
    double                        m_tolerance ; ///< the tolerance 
    // flag to control the modification of end-vertex
    bool m_change_vertex ; ///< flag to control the modification of end-vertex
    // ========================================================================
  } ;  
} // end of namespace LoKi
// ============================================================================
/* perform the mass-constrained fit of the particle into 
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
// ============================================================================
StatusCode LoKi::MassFitter::fit
( LHCb::Particle* particle , const double mass , double& chi2 ) const 
{
  if ( 0 == particle ) { return Error ( "LHCb::Particle* points to NULL" ) ; }
  // check the mass
  if ( massOK ( particle , mass ) ) { return StatusCode::SUCCESS ; }  // RETURN
  
  // prepare the local static storage
  
  static Gaudi::LorentzVector   s_momentum1  ;
  static Gaudi::XYZPoint        s_position1  ;
  static Gaudi::SymMatrix4x4    s_vpp        ;
  static Gaudi::SymMatrix3x3    s_vxx        ;
  static Gaudi::Matrix4x3       s_vpx        ;
  
  // extract all data from the particle:
  
  s_momentum1 = particle -> momentum         () ;
  s_position1 = particle -> referencePoint   () ;
  s_vpp       = particle -> momCovMatrix     () ;
  s_vxx       = particle -> posCovMatrix     () ;
  s_vpx       = particle -> posMomCovMatrix  () ;
  
  double s_chi2 = 0 ;
  
  // Lorentz vector as linear algebra 4-vector:
  static Gaudi::Vector4 s_momentum2 ;
  // Vertex position vector as linear algebra 3-vector:
  static Gaudi::Vector3 s_position2 ;
  
  // reduced part of the projection matrix 
  static Gaudi::Vector4 s_D        ;
  
  // the product of D * lambda
  static Gaudi::Vector4 s_DL       ;
  
  // the product of D * V_p
  static Gaudi::Vector4    s_dvp  ;
  // the product of D * V_px
  static Gaudi::Vector3    s_dvpx ;
  
  // perform the iterations
  for ( unsigned int iter  = 0 ; iter < m_iterMax ; ++iter ) 
  {
    // residual
    const double dmass2 = s_momentum1.M2() - mass * mass ;
    
    // fill the reduced projection matrix 
    s_D [ 0 ] = -2 * s_momentum1.Px () ;
    s_D [ 1 ] = -2 * s_momentum1.Py () ;
    s_D [ 2 ] = -2 * s_momentum1.Pz () ;
    s_D [ 3 ] =  2 * s_momentum1.E  () ;
    
    // evaluate V_D     = (D*V*D^T)-1
    const double v_D    = 1.0 / ROOT::Math::Similarity ( s_vpp, s_D ) ;
    
    // evaluate lambda  = V_D * ( D * delta_alpha + d ) 
    const double lambda = v_D * dmass2 ;
    
    // product of D*lambda
    s_DL  = s_D ;
    s_DL *= lambda ;
    
    // new momentum:
    
    // copy the Lorentz Vector of the particle momentum into the linear algebra 4-vector  
    Gaudi::Math::geo2LA ( s_momentum1 , s_momentum2 ) ;
    // calculate new momentum
    s_momentum2 -= s_DL * s_vpp ;
    Gaudi::Math::la2geo ( s_momentum2 , s_momentum1 ) ;
    
    // new position:
    
    // copy the Vertex Position of the particle into linear algebra 3-vector  
    Gaudi::Math::geo2LA ( s_position1 , s_position2 ) ;
    // calculate new position ;
    s_position2 -= s_DL * s_vpx ;
    Gaudi::Math::la2geo ( s_position2 , s_position1 ) ;
    
    // the updated covariance matrices
    
    s_dvp  = s_D * s_vpp  ;
    s_dvpx = s_D * s_vpx  ;
    
    // update the covariance matrices 
    Gaudi::Math::update ( s_vpp , s_dvp  ,          -v_D ) ;
    Gaudi::Math::update ( s_vxx , s_dvpx ,          -v_D ) ;
    Gaudi::Math::update ( s_vpx , s_dvp  , s_dvpx , -v_D ) ;
    
    // update (accumulate???) chi2 value 
    // s_chi2 += lambda * dmass2 ;
    s_chi2 = lambda * dmass2 ;
    
    if ( massOK ( s_momentum1 , mass ) ) 
    {
      // fill the particle
      particle -> setMomentum        ( s_momentum1 ) ;
      particle -> setReferencePoint  ( s_position1 ) ;
      particle -> setMomCovMatrix    ( s_vpp       ) ;
      particle -> setPosCovMatrix    ( s_vxx       ) ;
      particle -> setPosMomCovMatrix ( s_vpx       ) ; 
      //
      if ( particle -> hasInfo ( LHCb::Particle::Chi2OfMassConstrainedFit ) )
      { particle -> eraseInfo ( LHCb::Particle::Chi2OfMassConstrainedFit ) ; }
      // set chi2:
      chi2 = s_chi2 ;
      particle -> addInfo ( LHCb::Particle::Chi2OfMassConstrainedFit , s_chi2 ) ;
      //
      if ( m_change_vertex ) 
      {
        LHCb::VertexBase* vertex = particle -> endVertex () ;
        if ( 0 != vertex ) 
        {
          vertex -> setPosition  ( particle -> referencePoint () ) ;
          vertex -> setCovMatrix ( particle -> posCovMatrix   () ) ;
        }
      }
      //
      if ( msgLevel( MSG::DEBUG ) ) { counter ("#iterations") += iter + 1 ; }
      //
      return StatusCode::SUCCESS ;                                // RETURN
    }    
  }  // end of iterations
  // 
  // we are here, looks like there is no convergency
  counter ( "dmass" ) += s_momentum1.M() - mass ;
  counter ( "chi2"  ) += s_chi2 ;
  //
  return Warning ( "No convergency for mass-constraned fit, particle is unachanged") ;
}
// ============================================================================
/// the factory ( needed for instantiation)
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,MassFitter) ;
// ============================================================================
// The END 
// ============================================================================
