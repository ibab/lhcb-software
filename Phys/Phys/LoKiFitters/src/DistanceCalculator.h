// $Id: DistanceCalculator.h,v 1.1 2008-03-10 18:24:43 ibelyaev Exp $
// ============================================================================
#ifndef LOKIFITTERS_DISTANCECALCULATOR_H 
#define LOKIFITTERS_DISTANCECALCULATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// Local 
// ============================================================================
#include "DistanceCalculatorBase.h"
#include "KalmanFilter.h"
#include "FitterUtils.h"
// ============================================================================
namespace LoKi 
{
  /** @class DistanceCalculator DistanceCalculator.h
   *
   *  It is the simplest implementation of the basic math, 
   *  needed for the real implementation 
   *  on the abstract interface IDistanceCalculator 
   *  Essentially it relies on many nice functions, coded 
   *  
   *  
   *  @see IDistanceCalculator 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-03-05
   */
  class DistanceCalculator 
    : public virtual IDistanceCalculator 
    , public LoKi::DistanceCalculatorBase  
  {
    // the friend factory for instantiation 
    friend class ToolFactory<LoKi::DistanceCalculator> ;
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
      double&                 imppar   ) const ;
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
      double&                 chi2     ) const ;
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
      double&                 imppar   ) const ;    
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
      double&                 chi2     ) const ;    
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
      double&                  dist ) const ;
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
      double&                  chi2 ) const ;
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
      double&                  dist ) const ;
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
      double&                  chi2 ) const ;
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
      double&               dist ) const ;
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
      double&               chi2 ) const ;
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
     *  @param   errror   (output) the estimate of the uncertanti in 
     *                the projected distance
     *  @param   chi2 (output) the overall chi2 the procedure, 
     *                which is the measure of the consistency
     *  @return  status code 
     */
    virtual StatusCode pathDistance 
    ( const LHCb::Particle*   particle , 
      const LHCb::VertexBase* vertex   , 
      double&                 path     ,
      double&                 errror   , 
      double&                 chi2     ) const ;
    // ========================================================================
    /** Calculate the projected distance 
     *
     *  \f$s=\frac{\left(\vec{\mathbf{v}}\vec{\mathbf{p}}
     *     \right)}{\left|\vec{\mathbf{p}}\right|}\f,$
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
      double&                 error    ) const ;
    // ========================================================================
    /// @}
    // ========================================================================
  protected: 
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
  private:
    /// the default constructor is disabled 
    DistanceCalculator () ; // no default constructor 
    /// the copy    constructor is disabled 
    DistanceCalculator ( const DistanceCalculator&) ; // no copy 
    /// the assignement operator is disabled 
    DistanceCalculator& operator=( const DistanceCalculator&) ; // no assignement
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
     *  @param newZ     (input) new position 
     *  @param transported (output) the transported particle
     *  @return status code 
     */
    inline StatusCode transport 
    ( const LHCb::Particle*  particle    , 
      const Gaudi::XYZPoint& z           , 
      LHCb::Particle&        transported ) const 
    { return transport ( particle , z.Z() , transported ) ; }
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
  private:
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
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKIFITTERS_DISTANCECALCULATOR_H
// ============================================================================
