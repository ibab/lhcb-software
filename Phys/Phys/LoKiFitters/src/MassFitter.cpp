// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
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
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================
/** @file 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2008-01-15
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class MassFitter
   *  The most simple implementation of abstract interface IMassFit
   *  The tool performs the mass-constrained fit
   *
   *  The machinery from P.Avery's lectures is used 
   *  @see http://www.phys.ufl.edu/~avery/fitting/kinematic.pdf
   *
   *
   *  Let \f$\vec{\alpha}=\left( \vec{x}, \vec{p}\right)\f$  
   *  be a 7-vector containing the parameters of the particle and 
   *  let \f$V\f$ be the covariance matrix of these parameters, 
   *
   *  The constraint equation whcih forces the particle to have an invariant
   *  mass \f$m_c\f$ is:
   *
   *   \f[ 0 = \mathbf{H} = E^2-\vec{p}^2 - m_c^2 = E^2-p_x^2-p_y^2-p_z^2 - m_c^2. \f]
   *
   *  The corresponding linear expansion near some 
   *    point \f$\vec{\alpha}_A\f$ is :
   *
   *   \f[ 0 = \mathbf{H}\left(\vec{\alpha}\right) = 
   *           \frac{\partial \mathbf{H} }{\partial \vec{\alpha}}
   *            \left( \vec{\alpha} -\vec{\alpha}_A \right) 
   *             + \mathbf{H}\left(\vec{\alpha}_A\right) = 
   *             \mathbf{D}\delta\vec{\alpha} 
   *            + \mathbf{d}, \f] 
   *
   *  where \f$\delta\vec{\alpha}=\vec{\alpha}-\vec{\alpha}_A\f$, 
   *  \f$\mathbf{D}= \left.\frac{\partial \mathbf{H} }{\partial \vec{\alpha}}\right|_{\vec{\alpha}_A} \f$
   *  and \f$\mathbf{d}= \mathbf{H}\left(\vec{\alpha}_A\right) \f$.
   *  
   *  Clearly we have:
   *
   *  - \f$ \mathbf{D} = \left( 0 , 0 , 0 , -2p_x , -2py , -2p_z, 2E \right) = 
   *       \left( \mathbf{0} , \mathbf{D}_s \right) \f$ 
   *  - \f$ \mathbf{d} =  E^2 - p_x^2 - p_y^2 - p_z^2 - m_c^2 \f$
   *
   *  The solution of the equations 
   *    \f$ \frac{\partial\chi^2}{\partial\left(\vec{\alpha},\lambda\right)}\f$ is 
   *  - \f$ \vec{\alpha} = \vec{\alpha}_0 - \mathbf{V}_0\mathbf{D}^T\lambda \f$ 
   *  - \f$ \lambda = \mathbf{V}_D \left( \mathbf{D}\delta\vec{\alpha}_0 + \mathbf{d} \right) \f$ 
   *  - \f$ \mathbf{V}_D = \left( \mathbf{D}\mathbf{V}_0\mathbf{D}^T\right)^{-1} \f$
   *  - \f$ \mathbf{V} = \mathbf{V}_0 - \mathbf{V}_0\mathbf{D}^T\mathbf{V}_D\mathbf{D}\mathbf{V}_0\f$
   *  - \f$ \chi^2 = \lambda^T\left( \mathbf{D}\delta\vec{\alpha}_0+\mathbf{d}\right) \f$ 
   *
   *  Taking into account so simple structure of the matrix 
   *  \f$\mathbf{D}\f$, one gets the drastical simplification of all machinery:
   *
   *  Taking the covariance matrix of parameters to be  
   *  \f$ \mathbf{V} = \bigl( \begin{smallmatrix} 
   *       V_x  & V_{xp} \\ V^T_{xp} & V_{p} \end{smallmatrix}\bigr), \f$
   *
   *  One easily gets:
   *   - \f$ \mathbf{V}_D = V_D = 
   *     \left( \mathbf{D}_s\mathbf{V}_p\mathbf{D}_s^T\right)^{-1}\f$, where 
   *      \f$ \mathbf{D}_s = \left( -2p_x , -2py , -2pz, 2E \right) \f$.
   * 
   *  The rest of computations is just trivial, since 
   *   \f$ \mathbf{D}_s\mathbf{V}_p\mathbf{D}_s^T\f$ is a scalar value, 
   *   and no matrix inversions are involved anymore.  
   *
   *  Using the initial measurement \f$\vec{\alpha}_0\f$ as the expansion 
   *  point \f$\vec{\alpha}_A\f$, one gets \f$\delta\vec{\alpha}_0=\f$, 
   *  and therefore:
   *   -  \f$ \lambda = V_D\mathbf{d} \f$ 
   *   -  \f$ \vec{p} = \vec{p}_0 - V_{p0}  \mathbf{D}_s\lambda \f$ 
   *   -  \f$ \vec{x} = \vec{x}_0 - V_{xp0} \mathbf{D}_s\lambda \f$ 
   *   -  \f$ \chi^2  = \lambda \mathbf{d} \f$ 
   *
   *
   *  The tool has following properties:
   *
   *    - "MaxIterations" : The maximal allowed number of iterations.
   *                        The default value is <b>10</b> 
   *    - "Tolerance" : the stopping parameter \f$\delta\f$, 
   *                    the maximal allowed deviation of the 
   *                    invarinat mass from the nominal mass
   *                    \f$\left|\mathrm{m}-\mathrm{m_c}\right|<\delta\f$.
   *                    The default value is <b>0.2*Gaudi::Units::MeV</b> 
   *    - "ChangeVertex" : the boolean flag to force the modification 
   *                     the decay vertex of the particle.
   *                     The default value is <b>true</b> 
   *
   *
   *  @attention For the current implementation the particle is 
   *             not transported to the new position, which 
   *             corresponds to the modified vertex 
   *
   *  @see IMassFit
   *  @see IParticleRefitter
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2008-01-15
   */
  class MassFitter : public extends1<GaudiTool,IMassFit>
  {
    // ========================================================================
    // the friend factory (needed for instantiuation)
    friend class ToolFactory<LoKi::MassFitter> ; ///< needed for instantiation
    // ========================================================================
  public:    
    // ========================================================================
    enum 
      {
        // Invalid Particle 
        InvalidParticle         = 801 , ///< Invalid Particle  
        // Invalid Particle ID 
        InvalidParticleID       = 802 , ///< Invalid Particle ID 
        // No Particle Property Service  
        InvalidParticlePSvc     = 803 , ///< No Particle Property Service  
        // No Convergency
        NoConvergency           = 810   ///< No Convergency is detected 
      } ;
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
      { return Error ( "LHCb::Particle* points to NULL" , InvalidParticle ) ; }
      const LHCb::ParticleProperty* prop = pp ( particle->particleID() ) ;
      if ( 0 == prop     )
      { return Error ( "Invalid LHCb::ParticleID" , InvalidParticleID ) ; }
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
    {
      // play a bit with extra-info
      if ( particle.hasInfo ( LHCb::Particle::Chi2OfParticleReFitter ) ) 
      { particle.eraseInfo ( LHCb::Particle::Chi2OfParticleReFitter )  ; }
      //
      StatusCode sc = fit ( &particle ) ; 
      if ( sc.isFailure() ) 
      { return Error ("reFit(): the error from fit()" , sc ) ; }
      //
      // in the case of success update the extra-info:
      if ( particle.hasInfo ( LHCb::Particle::Chi2OfMassConstrainedFit ) ) 
      { 
        particle.addInfo 
          (  LHCb::Particle::Chi2OfParticleReFitter ,
             particle.info ( LHCb::Particle::Chi2OfMassConstrainedFit , -1000 ) ) ;
      }    
      //
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the tool 
    virtual StatusCode initialize() 
    {
      StatusCode sc = GaudiTool::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      svc<IService> ( "LoKiSvc" , true ) ;
      return StatusCode::SUCCESS ;
    }
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
      : base_class      ( type, name , parent )
      , m_ppSvc         (  0 ) 
      , m_iterMax       ( 20 )
      , m_tolerance     ( 20 * Gaudi::Units::keV ) 
      , m_change_vertex ( true )
    {
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
    /// The default constructor is disabled 
    MassFitter() ;                       // The default constructor is disabled 
    /// The dcopy constructor is disabled 
    MassFitter ( const MassFitter& ) ;   //    The copy constructor is disabled 
    /// The assigmenent operator is disabled 
    MassFitter& operator=( const MassFitter& ) ; // The assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// get the particle property for the given particle ID 
    inline const LHCb::ParticleProperty* pp ( const LHCb::ParticleID& pid ) const 
    {
      // locate the service (if needed)
      if ( 0 == m_ppSvc )  
      { m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc" , true ) ; }  
      Assert ( 0 != m_ppSvc , "Unable to locate LHCb::ParticlePropertySvc" , 
               StatusCode ( InvalidParticlePSvc , true ) ) ;
      const LHCb::ParticleProperty* p = m_ppSvc -> find ( pid ) ;
      if ( 0 == p )
      {
        StatusCode sc = StatusCode ( InvalidParticleID , true ) ;
        Error 
          ( "Invalid LHCb::ParticleProperty for ID=" + 
            boost::lexical_cast<std::string> ( pid.pid() ) , sc ) ;
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
    /// pointer to the particle property service 
    mutable LHCb::IParticlePropertySvc* m_ppSvc ; // particle property service 
    /// maximal number of iterations 
    unsigned int                  m_iterMax   ; // maximal number of iterations 
    /// the tolerance
    double                        m_tolerance ; // the tolerance 
    /// flag to control the modification of end-vertex
    bool m_change_vertex ; // flag to control the modification of end-vertex
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi
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
  
  if ( 0 == particle ) 
  { return Error ( "LHCb::Particle* points to NULL" , InvalidParticle ) ; }
  
  // play a bit with extra-info
  if ( particle->hasInfo ( LHCb::Particle::Chi2OfMassConstrainedFit ) ) 
  { particle->eraseInfo ( LHCb::Particle::Chi2OfMassConstrainedFit )  ; }  
  
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
  
  // ==========================================================================
  // perform the iterations
  // ==========================================================================
  for ( unsigned int iter  = 1 ; iter <= m_iterMax ; ++iter ) 
  {
    // residual
    const double dmass2 = s_momentum1.M2() - mass * mass ;
    
    // fill the reduced projection matrix 
    s_D [ 0 ] = -2 * s_momentum1.Px () ;
    s_D [ 1 ] = -2 * s_momentum1.Py () ;
    s_D [ 2 ] = -2 * s_momentum1.Pz () ;
    s_D [ 3 ] =  2 * s_momentum1.E  () ;
    
    // evaluate V_D     = (D*V*D^T)-1
    const double v_D    = 1.0 / ROOT::Math::Similarity ( s_D , s_vpp ) ;
    
    // evaluate D*delta_alpha0 
    const double DAlpha0 = 
      2 *  s_momentum1.Dot ( particle->momentum() - s_momentum1 ) ;
    
    // evaluate lambda  = V_D * ( D * delta_alpha + d ) 
    const double lambda = v_D * (  DAlpha0 + dmass2 ) ;
    
    // product of D*lambda
    s_DL  = s_D ;
    s_DL *= lambda ;
    
    // D*V 
    s_dvp  = s_D * s_vpp ;
    s_dvpx = s_D * s_vpx ;
    
    // new momentum:
    
    // copy the Lorentz Vector of the particle momentum 
    //   into the linear algebra 4-vector  
    Gaudi::Math::geo2LA ( particle -> momentum() , s_momentum2 ) ;
    // calculate new momentum
    s_momentum2 -= s_dvp * lambda ;    
    Gaudi::Math::la2geo ( s_momentum2 , s_momentum1 ) ;
    
    // new position:
    
    // copy the Vertex Position of the particle into linear algebra 3-vector  
    Gaudi::Math::geo2LA ( particle->referencePoint() , s_position2 ) ;
    // calculate new position ;
    s_position2 -= s_dvpx * lambda ;
    Gaudi::Math::la2geo ( s_position2 , s_position1 ) ;
    
    // update chi2 value 
    s_chi2 = lambda * ( DAlpha0 + dmass2 ) ;
    
    // check the convergency: 
    if ( massOK ( s_momentum1 , mass  ) ) // CONVERGENCY ? 
    {
      
      Gaudi::Math::update ( s_vpp , s_dvp  ,          -v_D ) ;
      Gaudi::Math::update ( s_vxx , s_dvpx ,          -v_D ) ;
      Gaudi::Math::update ( s_vpx , s_dvp  , s_dvpx , -v_D ) ;
      
      // fill the particle
      particle -> setMomentum        ( s_momentum1 ) ;
      particle -> setReferencePoint  ( s_position1 ) ;
      particle -> setMomCovMatrix    ( s_vpp       ) ;
      particle -> setPosCovMatrix    ( s_vxx       ) ;
      particle -> setPosMomCovMatrix ( s_vpx       ) ; 
      //      
      
      // set chi2:
      chi2 = s_chi2 ;
      // play a bit with extra-info
      particle -> addInfo ( LHCb::Particle::Chi2OfMassConstrainedFit , chi2 ) ;
      //
      if ( m_change_vertex ) 
      {
        LHCb::VertexBase* vertex = particle -> endVertex () ;
        if ( 0 != vertex ) 
        {
          vertex -> setPosition  ( particle -> referencePoint () ) ;
          vertex -> setCovMatrix ( particle -> posCovMatrix   () ) ;
        }
        else { Warning ( "EndVertex points to NULL, ignore" ) ; }
      }
      // 
      counter ( "#iterations" ) += iter ; 
      //
      return StatusCode::SUCCESS ;                                // RETURN
    }
    // ========================================================================
  } // end of iterations
  // ==========================================================================
  // 
  // we are here, looks like there is no convergency
  counter ( "dmass" ) += s_momentum1.M() - mass ;
  counter ( "chi2"  ) += s_chi2 ;
  //
  StatusCode sc = StatusCode ( NoConvergency , true ) ;
  Error ( "No convergency for mass-constrained fit" , sc ).ignore() ;
  // 
  if ( msgLevel ( MSG::DEBUG ) ) 
  {
    MsgStream& log = debug() ;
    log << " The error in Mass-conatined fit: "                 << std::endl ; 
    log << "\tThe Particle    : "<< (*particle)                 << std::endl ;
    log << "\tThe 4-momentum  : "<< particle->momentum()        << std::endl ;
    log << "\tThe Matrix V_p  : "<< particle->momCovMatrix()    << std::endl ;
    log << "\tThe Matrix V_px : "<< particle->posMomCovMatrix() << std::endl ;
    log << "\tThe Matrix V_x  : "<< particle->posCovMatrix()    << endreq    ;
  }
  //
  return sc ; 
}
// ============================================================================
/// the factory ( needed for instantiation)
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,MassFitter)
// ============================================================================
// The END 
// ============================================================================
