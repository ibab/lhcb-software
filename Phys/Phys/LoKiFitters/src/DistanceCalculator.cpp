// $Id: DistanceCalculator.cpp,v 1.9 2010-04-25 17:57:01 graven Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculatorBase.h"
#include "KalmanFilter.h"
#include "FitterUtils.h"
/// ============================================================================
namespace LoKi 
{
  // ===========================================================================
  /** @class DistanceCalculator DistanceCalculator.h
   *
   *  It is the simplest implementation of the basic math, 
   *  needed for the real implementation 
   *  on the abstract interface IDistanceCalculator 
   *  Essentially it relies on many nice functions, 
   *  coded by Juan & Matt  
   *  
   *  @see IDistanceCalculator 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-03-05
   */
  class DistanceCalculator 
    : public extends1<LoKi::DistanceCalculatorBase, IDistanceCalculator>
  {
    // ========================================================================
    // the friend factory for instantiation 
    friend class ToolFactory<LoKi::DistanceCalculator> ;
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup ParticlePoint 
     *   Evaluation of the distance between the particle and the vertex  
     *  @{
     */
    // ========================================================================
    /** The method for the evaluation of the impact parameter  ("distance")
     *  vector of the particle with respect to some vertex.
     *  @param particle (input) pointer to the particle 
     *  @param vertex   (input) pointer to the vertex 
     *  @param imppar   (output) the value of impact parameter ("distance") 
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const LHCb::VertexBase* vertex   , 
      double&                 imppar   ) const 
    {
      StatusCode sc = check ( particle , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , *vertex , impact ) ;
      imppar = impact.R() ;
      return sc ;                                                    // RETURN 
    }
    // ========================================================================
    /** The method for the evaluation of the impact parameter  ("distance")
     *  vector of the particle with respect to some vertex.
     *  @param particle (input) pointer to the particle 
     *  @param vertex   (input) pointer to the vertex 
     *  @param imppar   (output) the value of impact parameter ("distance") 
     *  @param chi2     (output) the chi2 estimate for the separation
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const LHCb::VertexBase* vertex   , 
      double&                 imppar   , 
      double&                 chi2     ) const 
    {
      StatusCode sc = check ( particle , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , *vertex , impact , &chi2 ) ;
      imppar = impact.R() ;
      return sc ;                                                    // RETURN 
    }
    // ========================================================================
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup ParticlePoint 
     *   Evalaution of the distance between the particle and the fixed point 
     *  @{
     */
    // ========================================================================
    /** The method for the evaluation of the impact parameter ("distance")
     *  vector of the particle with respect to the fixed point 
     *  @param particle (input) pointer to the particle 
     *  @param point    (input) the fixed point  
     *  @param imppar   (output) the value of impact parameter ("distance") 
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const Gaudi::XYZPoint&  point    , 
      double&                 imppar   ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , point , impact ) ;
      imppar = impact.R() ;
      return sc ;                                                    // RETURN 
    }
    // ========================================================================    
    /** The method for the evaluation of the impact parameter ("distance")
     *  vector of the particle with respect to the fixed point 
     *  @param particle (input) pointer to the particle 
     *  @param point    (input) the fixed point  
     *  @param imppar   (output) the value of impact parameter ("distance") 
     *  @param chi2     (output) the chi2 estimate for the separation
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const Gaudi::XYZPoint&  point    , 
      double&                 imppar   , 
      double&                 chi2     ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , point , impact , &chi2 ) ;
      imppar = impact.R() ;
      return sc ;                                                    // RETURN 
    }
    // ========================================================================    
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup IPVector 
     *   Evalaution of the vector distance between the particle point/vertex
     *  @{
     */
    // ========================================================================
    /** The method for the evaluation of the impact parameter vector 
     *  vector of the particle with respect to the fixed point 
     *  @param particle (input) pointer to the particle 
     *  @param point    (input) the fixed point  
     *  @param impact   (output) the value of impact parameter vector
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const Gaudi::XYZPoint&  point    , 
      Gaudi::XYZVector&       impact   ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      sc = _distance ( *particle , point , impact ) ;
      return sc ;                                                    // RETURN 
    }
    // ========================================================================
    /** The method for the evaluation of the impact parameter vector 
     *  vector of the particle with respect to the vertex 
     *  @param particle (input) pointer to the particle 
     *  @param point    (input) the fixed point  
     *  @param impact   (output) the value of impact parameter vector 
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const LHCb::VertexBase* vertex   , 
      Gaudi::XYZVector&       impact   ) const 
    { 
      StatusCode sc = check ( particle , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      sc = _distance ( *particle , *vertex , impact ) ;
      return sc ;                                                    // RETURN 
    }
    // ========================================================================    
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup VertexVertex 
     *   Evalaution of the distance between two vertices  
     *  @{
     */
    // ========================================================================
    /** The trivial method for evaluation of the distance between two vertices 
     *  @param v1   (input) the pointr to the first vertex 
     *  @param v2   (input) the pointer to the second vertex 
     *  @param dist (output) the distance between two vertices 
     *  @param return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::VertexBase*  v1   , 
      const LHCb::VertexBase*  v2   , 
      double&                  dist ) const 
    {
      StatusCode sc = check ( v1 , v2 ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the real calculations 
      return i_distance ( *v1 , *v2 , dist ) ;                        // RETURN
    }
    // ========================================================================
    /** The method for evaluation of the distance between two vertices and the 
     *  corresponding \f$\chi^2\f$ for the separation significance.
     *  @param v1   (input) the pointr to the first vertex 
     *  @param v2   (input) the pointer to the second vertex 
     *  @param dist (output) the distance between two vertices 
     *  @param return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::VertexBase*  v1   , 
      const LHCb::VertexBase*  v2   , 
      double&                  dist , 
      double&                  chi2 ) const      
    {  
      StatusCode sc = check ( v1 , v2 ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the real calculations 
      return i_distance ( *v1 , *v2 , dist , &chi2 ) ;               // RETURN 
    }
    // ========================================================================
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup VertexPoint
     *   The set of the methods for evaluation of the various distances
     *   between the vertex and th efixed point 
     *  @{
     */
    // ========================================================================
    /** The trivial method for evaluation of the distance between the vertex 
     *  and some "fixed" point
     *  @param v   (input) the pointr to the first vertex 
     *  @param p   (input) the fixed point  
     *  @param dist (output) the distance between two vertices 
     *  @param return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::VertexBase*  v    , 
      const Gaudi::XYZPoint&   p    , 
      double&                  dist ) const      
    {
      StatusCode sc = check ( v ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the real calculations 
      return i_distance ( *v , p , dist ) ;                           // RETURN 
    }
    // ========================================================================
    /** The method for evaluation of the distance between the vertices 
     *  and some fixed point and the corresponding \f$\chi^2\f$ for 
     *  the separation significance.
     *  @param v   (input)   the pointer to the first vertex 
     *  @param p   (input)   the fixed point 
     *  @param dist (output) the distance between two vertices 
     *  @param chi2 (output) the chi2 of the separation significance
     *  @param return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::VertexBase*  v    , 
      const Gaudi::XYZPoint&   p    , 
      double&                  dist , 
      double&                  chi2 ) const 
    {
      StatusCode sc = check ( v ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the real calculations 
      return i_distance ( *v , p , dist , &chi2 ) ;                   // RETURN
    }
    // ========================================================================
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup ParticleParticle
     *   The set of the methods for evaluation of the various distances
     *   between two particles 
     *  @{
     */
    // ========================================================================
    /** The method for evaluation of the scalar distance between two particles, 
     *  aka "distance of the closest approach". 
     *  @param p1 (input) the pointer to the first particle 
     *  @param p2 (input) the pointer to the second particle 
     *  @param dist (output) the shortest distance between two trajectories  
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle* p1   , 
      const LHCb::Particle* p2   , 
      double&               dist ) const 
    {
      StatusCode sc = check ( p1 , p2 ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the real calculations 
      return _distance ( *p1 , *p2 , dist ) ;                         // RETURN  
    }
    // ========================================================================
    /** The method for evaluation of the scalar distance between two particles, 
     *  aka "distance of the closest approach" and also its 
     *   \f$\chi^2\f$ for separation significance 
     *  @param p1 (input) the pointer to the first particle 
     *  @param p2 (input) the pointer to the second particle 
     *  @param dist (output) the shortest diostance between trajectories   
     *  @param chi2 (output) chi2-estimate for the separation significance
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle* p1   , 
      const LHCb::Particle* p2   ,
      double&               dist , 
      double&               chi2 ) const 
    {
      StatusCode sc = check ( p1 , p2 ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the real calculations 
      return _distance ( *p1 , *p2 , dist , &chi2 ) ;                 // RETURN  
    }
    // ========================================================================
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup OtherDistances
     *   The set of the methods for evaluation of "other" distances 
     *  @{
     */
    // ========================================================================
    /** The method for evaluation of the "path"-distance 
     *  between the decay vertex of the particle and the vertex.
     * 
     *  The path-distance, 
     *   is defined as the value of the scalar parameter \f$s\f$
     *  from the vector equation: 
     *
     *   \f[  \vec{\mathbf{v}}_{decay} = \vec{\mathbf{v}}_{production} + 
     *           \frac{\vec{\mathbf{p}}}{\left|\vec{\mathbf{p}}\right|}s \f]
     *  
     *  @param   particle (input) the pointer to the particle 
     *  @param   primary  (input) the pointer to the production vertex
     *  @param   path     (output) the "path-distance"
     *  @param   error    (output) the estimate of the uncertanti in 
     *                the projected distance
     *  @param   chi2 (output) the overall chi2 the procedure, 
     *                which is the measure of the consistency
     *  @return  status code 
     */
    virtual StatusCode pathDistance 
    ( const LHCb::Particle*   particle , 
      const LHCb::VertexBase* primary  , 
      double&                 path     ,
      double&                 error    , 
      double&                 chi2     ) const 
    {
      // check the input data 
      StatusCode sc = check ( particle , primary ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // check the end-vertex
      const LHCb::VertexBase* decay = particle->endVertex() ;
      sc = check ( decay ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the real evaluation:
      return _distance ( *primary  , *particle , *decay , path , error , chi2 ) ;
    }
    // ========================================================================
    /** Calculate the projected distance 
     *
     *  \f$s=\frac{\left(\vec{\mathbf{v}}\vec{\mathbf{p}}
     *     \right)}{\left|\vec{\mathbf{p}}\right|}\f$
     *  where vector \f$\vec{\mathbf{v}}\f$ i a vector from 
     *  the primary to the secondary vertex: 
     *    \f$\vec{\mathbf{v}}=\vec{\mathbf{x}}_{d}-\vec{\mathbf{x}}_{pv}\f$,
     *
     * @param particle (input)  the pointer to the particle 
     * @param primary  (input)  the pointer to the production vertex 
     * @param dist     (output) the projected distance
     * @return status code 
     */
    virtual StatusCode projectedDistance   
    ( const LHCb::Particle*   particle , 
      const LHCb::VertexBase* primary  , 
      double&                 dist     ) const      
    {
      // check the input data 
      StatusCode sc = check ( particle , primary ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // check the end-vertex
      const LHCb::VertexBase* decay = particle->endVertex() ;
      sc = check ( decay ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the real evaluation:
      return _distance ( *primary  , *particle , *decay , dist , 0 ) ;
    }
    // ========================================================================
    /** Calculate the projected distance 
     *
     *  \f$s=\frac{\left(\vec{\mathbf{v}}\vec{\mathbf{p}}
     *     \right)}{\left|\vec{\mathbf{p}}\right|}\f$
     *  where vector \f$\vec{\mathbf{v}}\f$ i a vector from 
     *  the primary to the secondary vertex: 
     *    \f$\vec{\mathbf{v}}=\vec{\mathbf{x}}_{d}-\vec{\mathbf{x}}_{pv}\f$,
     *  and its error 
     *
     * @param particle (input)  the pointer to the particle 
     * @param primary  (input)  the pointer to the production vertex 
     * @param dist     (output) the projected distance
     * @param error    (output) the estimate of the error in the distance 
     * @return status code 
     */
    virtual StatusCode projectedDistance   
    ( const LHCb::Particle*   particle , 
      const LHCb::VertexBase* primary  , 
      double&                 dist     , 
      double&                 error    ) const 
    {
      // check the input data 
      StatusCode sc = check ( particle , primary ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // check the end-vertex
      const LHCb::VertexBase* decay = particle->endVertex() ;
      sc = check ( decay ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the real evaluation:
      return _distance ( *primary  , *particle , *decay , dist , &error ) ;
    }
    // ========================================================================
    /// @}
    // ========================================================================
  protected: 
    // ========================================================================
    /** Standard constructor
     *  @param type tool type(?)
     *  @param name tool instance name 
     *  @param parent the pointer to the parent
     */
    DistanceCalculator
    ( const std::string& type   ,    // tool type (?)
      const std::string& name   ,    // tool instance name 
      const IInterface*  parent ) ;  // the parent 
    // virtual and protected desctrustor 
    virtual ~DistanceCalculator () ; // Destructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    DistanceCalculator () ; // no default constructor 
    /// the copy    constructor is disabled 
    DistanceCalculator ( const DistanceCalculator&) ; // no copy 
    /// the assignement operator is disabled 
    DistanceCalculator& operator=( const DistanceCalculator&) ; // no assignement
    // ========================================================================
  private:
    // ========================================================================
    /** transport the particle to a certain Z position
     *  @param particle (input) the particle to be transported
     *  @param newZ     (input) new position 
     *  @param transported (output) the transported particle
     *  @return status code 
     */
    inline StatusCode transport 
    ( const LHCb::Particle* particle    , 
      const double          newZ        , 
      LHCb::Particle&       transported ) const 
    {
      if ( 0 == m_transporter ) 
      { m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ; }
      return m_transporter -> transport ( particle , newZ , transported ) ;
    }  
    // ========================================================================
    /** transport the particle to a certain Z position
     *  @param particle (input) the particle to be transported
     *  @param z        (input) new position 
     *  @param transported (output) the transported particle
     *  @return status code 
     */
    inline StatusCode transport 
    ( const LHCb::Particle*  particle    , 
      const Gaudi::XYZPoint& z           , 
      LHCb::Particle&        transported ) const 
    { return transport ( particle , z.Z() , transported ) ; }
    // ========================================================================
    /** transport the particle to a certain Z position
     *  @param particle (input) the particle to be transported
     *  @param newZ     (input) new position 
     *  @param transported (output) the transported particle
     *  @return status code 
     */
    inline StatusCode transportAndProject 
    ( const LHCb::Particle* particle    , 
      const double          newZ        , 
      LHCb::Particle&       transported ) const 
    {
      if ( 0 == m_transporter ) 
      { m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ; }
      return m_transporter -> 
        transportAndProject ( particle , newZ , transported ) ;
    }  
    // ========================================================================
    /** transport the particle to a certain Z position
     *  @param particle (input) the particle to be transported
     *  @param z        (input) new position 
     *  @param transported (output) the transported particle
     *  @return status code 
     */
    inline StatusCode transportAndProject 
    ( const LHCb::Particle*  particle    , 
      const Gaudi::XYZPoint& z           , 
      LHCb::Particle&        transported ) const 
    { return transportAndProject ( particle , z.Z() , transported ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /** The method for the evaluation of the impact parameter ("distance") 
     *  vector of the particle with respect to some vertex. 
     *  @param particle (input) pointer to the particle 
     *  @param vertex   (input) pointer to the vertex 
     *  @param imppar   (output) the impact parameter ("distance") vector 
     *  @param chi2     (output,optional) the chi2 of the inmpact parameter
     *  @return status code 
     */
    StatusCode _distance 
    ( const LHCb::Particle&   particle     ,
      const LHCb::VertexBase& vertex       , 
      Gaudi::XYZVector&       imppar       , 
      double*                 chi2     = 0 ) const ;
    // ========================================================================
    /** The method for the evaluation of the impact parameter ("distance") 
     *  vector of the particle with respect to the fixed vertex  
     *  @param particle (input) the particle 
     *  @param point    (input) the fixed point
     *  @param imppar   (output) the impact parameter ("distance") vector 
     *  @param chi2     (output,optional) the chi2 of the inmpact parameter
     *  @return status code 
     */
    StatusCode _distance 
    ( const LHCb::Particle&   particle     ,
      const Gaudi::XYZPoint&  point        , 
      Gaudi::XYZVector&       imppar       , 
      double*                 chi2     = 0 ) const;
    // ========================================================================
    /** The method for evaluation of the scalar distance between two particles, 
     *  aka "distance of the closest approach" and also its 
     *   \f$\chi^2\f$ for separation significance 
     *  @param p1 (input) the first particle 
     *  @param p2 (input) the second particle 
     *  @param dist (output) the shortest distance between trajectories   
     *  @param chi2 (output,optional) chi2-estimate for the separation significance
     *  @return status code 
     */
    StatusCode _distance 
    ( const LHCb::Particle& p1       , 
      const LHCb::Particle& p2       ,
      double&               dist     , 
      double*               chi2 = 0 ) const ;
    // ========================================================================
    /** the method for the evaluation of "path"-distance
     *  @param primary  (input) the production(primary) vertex 
     *  @param particle (input) the particle 
     *  @param decay    (input) the decay vertex of the particle 
     *  @param path     (output) the path-distance  
     *  @param error    (output) the error inpath distance 
     *  @param chi2     (output) the chi2 of the procedure 
     *  @return status code 
     */
    StatusCode _distance 
    ( const LHCb::VertexBase& primary  ,  
      const LHCb::Particle&   particle , 
      const LHCb::VertexBase& decay    ,  
      double&                 path     ,
      double&                 error    ,
      double&                 chi2     ) const ;  
    // ========================================================================
    /** Calculate the projected distance 
     *
     *  \f$s=\frac{\left(\vec{\mathbf{v}}\vec{\mathbf{p}}
     *     \right)}{\left|\vec{\mathbf{p}}\right|}\f$
     *  where vector \f$\vec{\mathbf{v}}\f$ is a vector from 
     *  the primary to the secondary vertex: 
     *    \f$\vec{\mathbf{v}}=\vec{\mathbf{x}}_{d}-\vec{\mathbf{x}}_{pv}\f$,
     *  and its error 
     *
     * The simplest way to evaluate the error it to considner formally 
     * the problem as a constrained minimization with the constraint:
     *  
     * \f$ H = \left(\vec{\mathbf{v}}\vec{\mathbf{p}}\right) - 
     *    \mathbf{s}\left|\vec{\mathbf{p}}\right| = 0 
     * \f$
     * Of course there is no need to perform the actual minimiation 
     * The solution is known in advance!), but formalizm is easy to reuse 
     * for evaluation of \f$\mathbf{C_s}\f$
     *  
     * @param primary  (input)  the production vertex 
     * @param particle (input)  the particle 
     * @param decay    (input)  the decay particle 
     * @param dist     (output) the projected distance
     * @param error    (output) the estimate of the error in the distance 
     * @return status code 
     */
    StatusCode _distance   
    ( const LHCb::VertexBase& primary      , 
      const LHCb::Particle&   particle     , 
      const LHCb::VertexBase& decay        , 
      double&                 dist         , 
      double*                 error    = 0 ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the maximal number of iterations 
    unsigned int m_nIter_max  ; // the maximal number of iterations 
    /// the convergency criterion for impact parameter evaluations 
    double       m_deltaZ     ; // the convergency criterion for ip-evaluations
    /// the convergency criterion for delta(chi2) 
    double       m_delta_chi2 ; // the convergency criterion for delta(chi2) 
    // the convergency criteri for delta(path)
    double       m_delta_path ; // the convergency criterion for delta(path)
    /// The name of particle transpoter tool 
    std::string  m_transporterName ; // The name of particle transpoter tool    
    /// The particle transporter itself 
    mutable IParticleTransporter* m_transporter ; // The transporter itself
    /// some static particle 
    mutable LHCb::Particle        m_particle1   ; // some particle 
    /// another static particle 
    mutable LHCb::Particle        m_particle2   ; // some particle 
    /// Kalman filter object: 
    mutable LoKi::KalmanFilter::Entry   m_entry   ; // Kalman filter object
    /// Kalman filter objects: 
    mutable LoKi::KalmanFilter::Entries m_entries ; // Kalman filter objects
    /// distance/path fitter
    mutable LoKi::Fitters::Fitter1      m_fitter  ; // distance/path fitter
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/*  Standard constructor
 *  @param type tool type(?)
 *  @param name tool instance name 
 *  @param parent the pointer to the parent
 */
// ============================================================================
LoKi::DistanceCalculator::DistanceCalculator
( const std::string& type   , // tool type (?)
  const std::string& name   , // tool instance name 
  const IInterface*  parent ) // the parent 
  : base_class ( type , name , parent ) 
  /// the maximal number of iterations 
  , m_nIter_max ( 10 ) // the maximal number of iterations 
  /// the convergency criterion for ip-evaluation
  , m_deltaZ     ( 2 * Gaudi::Units::micrometer ) // the criteria for ip-evaluation
  /// the convergency criterion for delta(chi2) 
  , m_delta_chi2 ( 0.05 ) 
  /// the convergency criterion for delta(path) 
  , m_delta_path ( 2 * Gaudi::Units::micrometer )
  /// The name of particle transpoter tool 
  , m_transporterName ( "ParticleTransporter:PUBLIC" ) // The name of particle transpoter tool
  /// The tarnsported tool itself 
  , m_transporter ( 0 ) // The transporter tool itself 
  /// the local storages of particles
  , m_particle1 ( LHCb::ParticleID ( 511 ) ) 
  , m_particle2 ( LHCb::ParticleID ( 511 ) )
{
  //
  declareProperty 
    ( "MaxIterations"    , m_nIter_max           , 
      "The maximal number of iterations "        ) ;
  declareProperty 
    (  "ToleranceInZ"    , m_deltaZ              , 
       "The allowed tolerance in Z (for transporters)" ) ;
  declareProperty 
    ( "DeltaChi2"        , m_delta_chi2          , 
      "Delta chi2  (convergency criterion)"      ) ;
  declareProperty 
    ( "DeltaPath"        , m_delta_path          , 
      "Delta c*tau (convergency criterion)"      ) ; 
  declareProperty 
    ( "Transporter"      , m_transporterName     , 
      "The Particle Transporter tool to be used" );
}
// ============================================================================
// virtual and protected desctrustor 
// ============================================================================
LoKi::DistanceCalculator::~DistanceCalculator(){}
// ============================================================================
/*  The method for the evaluation of the impact parameter ("distance")
 *  vector of the particle with respect to some vertex. 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const LHCb::VertexBase& vertex   , 
  Gaudi::XYZVector&       impact   , 
  double*                 chi2     ) const 
{
  
  using namespace Gaudi::Math::Operators ;
  
  // make the fast evaluation:
  i_distance ( particle , vertex , impact ) ;
  
  const double Vz = vertex.position().Z() ;
  
  const LHCb::Particle* good  = &particle ;
  
  double deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
  
  // make the iterations (if needed) 
  unsigned int nIter = 0 ;
  for ( unsigned int iIter = 0 ; 
        deltaZ >= m_deltaZ && iIter < m_nIter_max ; ++iIter ) 
  { 
    // transport the particle into the new impact parameter point.
    StatusCode sc = transportAndProject 
      ( good                         ,   // what to be transported 
        vertex.position() + impact   ,   // new Z 
        m_particle1                  ) ; // the result
    if ( sc.isFailure() ) 
    { _Warning ( "Error from ParticleTransporter(ip_1), ignore" , sc ) ; }
    else 
    { good = &m_particle1 ; } // the properly transported particle 
    
    // make the new evaluation of impact parameter
    i_distance ( *good , vertex , impact ) ;
    
    // check the delta_z 
    deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
    
    ++nIter ;
  } /// the end of iteration loop 
  
  // check for  the convergency
  if ( deltaZ >= m_deltaZ )
  { _Warning ( "There is no convergency-I", NoConvergency ) ; }
  
  // evaluate chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = -1.e+10 ;
    // transport the particle into the new impact parameter point.
    if ( good != &m_particle1 ) 
    {
      // not yet transported? 
      StatusCode sc = transportAndProject 
        ( good                         ,   // what to be transported 
          vertex.position() + impact   ,   // new Z 
          m_particle1                  ) ; // the result 
      if ( sc.isFailure() ) 
      { _Warning ( "Error from ParticleTransporter(chi2_1), ignore" , sc ) ; }
      else 
      { good = &m_particle1 ; } // the properly transported particle    
    }
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::load ( *good , m_entry ) ;
    if ( sc.isFailure() ) 
    { return _Warning("distance(I): KalmanFilter::load failed", sc ) ; }
    // get the "the previus" Kalman Filter estimate == vertex
    Gaudi::SymMatrix3x3 ci = vertex.covMatrix() ; // the gain matrix 
    if ( !ci.Invert() ) 
    { return _Warning ( "distance(I): unable to calculate the gain matrix" ) ; }
    // make one step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( m_entry , vertex.position() , ci , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Warning ( "distance(I): error from Kalman Filter step" , sc ) ; }
    // get the chi2 
    *chi2 = m_entry.m_chi2 ;
    // ========================================================================
  }
  //
  return StatusCode::SUCCESS ;                                 // RETURN 
}
// ============================================================================
/*  The method for the evaluation of the impact parameter ("distance")
 *  vector of the particle with respect to  the fixed point 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const Gaudi::XYZPoint&  point    , 
  Gaudi::XYZVector&       impact   , 
  double*                 chi2     ) const 
{ 
  using namespace Gaudi::Math::Operators ;
  
  // make the fast evaluation:
  i_distance ( particle , point , impact ) ;
  
  const double Vz = point.Z() ;
  
  const LHCb::Particle* good  = &particle ;
  
  double deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
  
  // make the iterations here (if needed) 
  unsigned int nIter = 0 ;
  for ( unsigned int iIter = 0 ; 
        deltaZ >= m_deltaZ && iIter < m_nIter_max ; ++iIter ) 
  {
    // transport the particle into the new impact parameter point.
    StatusCode sc = transportAndProject 
      ( good             ,   // what to be transported 
        point + impact   ,   // new Z 
        m_particle1      ) ; // the result 
    if ( sc.isFailure() ) 
    { _Warning ( "Error from ParticleTransporter(ip_2), ignore" , sc ) ; }
    else 
    { good = &m_particle1 ; } // the properly transported particle 
    
    // the transported particle 
    good = &m_particle1 ;
    
    // make the new evaluation of impact parameter
    i_distance ( *good , point , impact ) ;
    
    // check the delta_z 
    deltaZ = ::fabs ( Vz + impact.Z() - good->referencePoint().Z() ) ;
    
    ++nIter ;
  } // the end of iteration loop 
  
  // check for  the convergency
  if ( deltaZ >= m_deltaZ )
  { _Warning ( "There is no convergency-II", NoConvergency ) ; }
  
  // evaluate the chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    *chi2 = -1.e+10 ;
    // transport the particle into the new impact parameter point.
    if ( good != &m_particle1 ) 
    {
      // not yet transported? 
      StatusCode sc = transportAndProject 
        ( good                         ,   // what to be transported 
          point + impact               ,   // new Z 
          m_particle1                  ) ; // the result 
      if ( sc.isFailure() ) 
      { _Warning ( "Error from ParticleTransporter(chi2_2), ignore" , sc ) ; }
      else 
      { good = &m_particle1 ; } // the properly transported particle 
    }
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::load ( *good , m_entry ) ;
    if ( sc.isFailure() ) 
    { return _Error("distance(II): error from KalmanFilter::load" , sc ) ; }
    // here the evaluations of chi2 is just trivial:
    *chi2 = Gaudi::Math::Similarity ( m_entry.m_vxi , impact ) ;
  }
  //
  return StatusCode::SUCCESS ;                                 // RETURN 
}
// ============================================================================
/* The method for evaluation of the scalar distance between two particles, 
 * aka "distance of the closest approach" and also its 
 * chi^2-for separation significance 
 */
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance
( const LHCb::Particle& p1   , 
  const LHCb::Particle& p2   ,
  double&               dist , 
  double*               chi2 ) const 
{

  using namespace Gaudi::Math::Operators ;
  
  Gaudi::XYZPoint point1 ;
  Gaudi::XYZPoint point2 ;
  // make the evaluation of the distance:
  i_distance ( p1 , p2 , point1 , point2 ) ;
  
  const LHCb::Particle* good1 = &p1 ;
  const LHCb::Particle* good2 = &p2 ;
  
  double dz1 = ::fabs ( point1.Z() - good1 -> referencePoint().Z() ) ;
  double dz2 = ::fabs ( point2.Z() - good2 -> referencePoint().Z() ) ;
  double dz  = std::max ( dz1 , dz2 ) ;
  
  // make iterations here (if needed)
  unsigned int nIter = 0 ;
  for ( unsigned int iIter = 0 ; 
        dz >= m_deltaZ && iIter <= m_nIter_max ; ++iIter ) 
  {
    // transport the first particle into new positions:
    StatusCode sc = transport 
      ( good1       ,    // what to transport 
        point1      ,    // where to transport 
        m_particle1 )  ; // destination 
    if ( sc.isFailure() ) 
    { _Warning ( "distance(III):Error from ParticleTransporter, ignore" , sc ) ; }
    else { good1 = &m_particle1 ; } // the properly transported particles:
    
    // transport the second particle into new positions:
    sc = transport 
      ( good2       ,    // what to transport 
        point2      ,    // where to transport 
        m_particle2 )  ; // destination 
    if ( sc.isFailure() ) 
    { _Warning ( "distance(III):Error from ParticleTransporter, ignore" , sc ) ; }
    else { good2 = &m_particle2 ; } // the properly transported particles:
    
    // make new (improved) evaluation of the distance:
    i_distance ( *good1 , *good2 , point1 , point2 ) ;
    
    // check delta_Z 
    dz1 = ::fabs ( point1.Z () - good1 -> referencePoint().Z() ) ;
    dz2 = ::fabs ( point2.Z () - good2 -> referencePoint().Z() ) ;
    dz  = std::max ( dz1 , dz2 ) ;
    
    ++nIter ;
  } /// end of iterations 
  
  // check for  the convergency
  if ( dz >= m_deltaZ )
  { _Warning ( "There is no convergency-III", NoConvergency ) ; }
  
  // evaluate the distance 
  dist = ( point1 - point2 ) . R () ;
  
  // evaluate chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = 1.e+10 ;    
    // prepare the Kalman Filter machinery
    m_entries.resize(2) ;
    LoKi::KalmanFilter::Entries::iterator first  = m_entries.begin() ;
    LoKi::KalmanFilter::Entries::iterator second = first + 1         ;   
    // load:
    StatusCode sc = LoKi::KalmanFilter::load ( *good1 , *first  ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::load(1)" , sc ) ; }
    // load : 
    sc =            LoKi::KalmanFilter::load ( *good2 , *second ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::load(2)" , sc ) ; }
    //
    // make the special step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( *first  , *second , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::step(2)" , sc ) ; }
    //
    // get the final chi2 
    *chi2 = second->m_chi2 ;
    // ========================================================================
  }
  //
  return StatusCode::SUCCESS ;                                 // RETURN
}
// ============================================================================
// the method for the evaluation of "path"-distance
// ============================================================================
StatusCode LoKi::DistanceCalculator::_distance 
( const LHCb::VertexBase& primary  , // the production/primary vertex  
  const LHCb::Particle&   particle , // the particle 
  const LHCb::VertexBase& decay    , // the decay vertex   
  double&                 path     , // the estimate of path-distance
  double&                 error    , // the error in  path-distance 
  double&                 chi2     ) const 
{
  
  // reset the initial values 
  error = -1.e+10 * Gaudi::Units::mm ;
  chi2  = -1.e+10                    ;
  
  // propagate particle into its own decay vertex:
  StatusCode sc = transport ( &particle , decay.position() , m_particle1 ) ;
  if ( sc.isFailure() ) 
  { return _Error ( "distance(IV):Error from IParticleTransporter" , sc ) ; }
  
  // the properly transported particle
  const LHCb::Particle* good = &m_particle1 ;
  
  // get the first estimate of the path parameter
  sc = LoKi::Fitters::path0 ( primary , *good , decay , path ) ;
  if ( sc.isFailure() ) 
  {
    _Warning ( "distance(IV):Error code from LoKi::Fitters::path0" , sc ) ;
    path = 0 ;
  }
  
  // get the expansion point:
  Gaudi::LorentzVector momentum   = good     -> momentum       () ;
  Gaudi::XYZPoint      decvertex  = good     -> referencePoint () ;
  Gaudi::XYZPoint      primvertex = primary  .  position       () ;
  
  // make iterations here (if needed)
  for ( unsigned int iIter = 1 ; iIter <= m_nIter_max ; ++iIter ) 
  {
    const double o_chi2 = chi2 ;
    const double o_path = path ;
    
    // make one step of the fit:
    m_fitter.m_var = path ; 
    sc = LoKi::Fitters::path_step 
      ( primary , *good , momentum , decvertex , primvertex , m_fitter ) ;
    if ( sc.isFailure() ) 
    {
      // reset  to the initial expansion point and reiterate 
      _Warning ( "distance(IV): error from path_step" , sc ) ;
      momentum   = good     -> momentum       () ;
      decvertex  = good     -> referencePoint () ;
      primvertex = primary  .  position       () ;
      m_fitter.m_var = 0  ; 
      continue  ;                                                 // CONTINUE
    }
    
    // get the parameters from the fitter
    path  = m_fitter.m_var  ;
    chi2  = m_fitter.m_chi2 ;
    
    // check for the convergency: 
    //    - by the change in either chi2 *OR* in the proper lifetime  
    if ( ::fabs ( chi2  - o_chi2 ) < m_delta_chi2 || 
         ::fabs ( path  - o_path ) < m_delta_path ) 
    {
      // get the error in "path" 
      error = ::sqrt ( m_fitter.m_Vvar ) ;
      //
      return StatusCode::SUCCESS ;                                   // RETURN 
    }  
  }
  
  _Warning ( "There is no convergency-IV" , NoConvergency ) ;
  // get the error in "path" 
  error = ::sqrt ( m_fitter.m_Vvar ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================ 
/* Calculate the projected distance 
 * @param primary  (input)  the production vertex 
 * @param particle (input)  the particle 
 * @param decay    (input)  the decay particle 
 * @param dist     (output) the projected distance
 * @param error    (output) the estimate of the error in the distance 
 * @return status code 
 */
// ============================================================================ 
StatusCode LoKi::DistanceCalculator::_distance   
( const LHCb::VertexBase& primary  ,         // the production vertex 
  const LHCb::Particle&   particle ,         // the particle  
  const LHCb::VertexBase& decay    ,         // the production vertex 
  double&                 dist     ,         // the distance 
  double*                 error    ) const   // the error in distance 
{
  
  using namespace Gaudi::Math::Operators ;
  
  // propagate the particle into its own decay vertex:
  StatusCode sc = transport ( &particle , decay.position() , m_particle1 ) ;
  if ( sc.isFailure() ) 
  { return _Error ( "distance(V):Error from IParticleTransporter" , sc ) ; }
  
  const LHCb::Particle* good = &m_particle1 ; // the properly transported particle
  
  // evaluate the projected distance 
  dist = i_distance ( primary  , *good , decay ) ;
  // evaluate the error? 
  if ( 0 == error ) { return StatusCode::SUCCESS ; }         // RETURN 
  //
  const Gaudi::XYZPoint& vd   = decay    . position() ;
  const Gaudi::XYZPoint& vpv  = primary  . position() ;
  const Gaudi::XYZVector  p   = good     -> momentum() . Vect () ;
  //
  //  H = (vd-vp)*p - s* |p|
  //
  // get dH/d(p4)
  Gaudi::Vector4 dHdp4 ;
  const Gaudi::XYZVector delta = vd - vpv - dist * p.Unit() ;
  dHdp4 ( 0 ) = delta.X () ; // dH/d(px) 
  dHdp4 ( 1 ) = delta.Y () ; // dH/d(py)
  dHdp4 ( 2 ) = delta.Z () ; // dH/d(pz) 
  dHdp4 ( 3 ) = 0          ; // dH/d(E) 
  //
  // get dH/d(vd) 
  const Gaudi::XYZVector& dHdvd =  p ; // dH/d(vd) 
  // get dH/d(vp) 
  // const Gaudi::XYZVector  dHdvp = -p ; // dH/d(vp) 
  //
  // V_D = ( D^T * V * D ) ^{-1}
  const double V_D = 1.0 /
    ( ROOT ::Math::Similarity ( dHdp4 , good ->    momCovMatrix ()     ) + 
      2 * ROOT::Math::Dot     ( dHdp4 , good -> posMomCovMatrix () * p ) + 
      Gaudi::Math::Similarity ( dHdvd , decay    .    covMatrix ()     ) + 
      Gaudi::Math::Similarity ( dHdvd , primary  .    covMatrix ()     ) );
  
  // v_s = ( E^T * V_D * E ) ^ {-1}
  //  where E = d(H)/d(s) = -|p|  
  const double V_s = 1.0 / ( V_D * p.Mag2() ) ;
  //
  if ( 0 <= V_s ) { *error = ::sqrt ( V_s ) ; }
  else 
  {
    *error = -1 ;
    _Warning ( "distance(V):The negative covarinace, return error=-1" ) ;    
  }
  //
  return StatusCode::SUCCESS ;                                       // RETURN
}
// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,DistanceCalculator) ;
// ============================================================================
// The END 
// ============================================================================
