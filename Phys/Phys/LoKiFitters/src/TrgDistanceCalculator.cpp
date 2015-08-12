// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKenrel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
/// KalmanFilter
// ============================================================================
#include "KalmanFilter/VertexFit.h"
#include "KalmanFilter/VertexFitWithTracks.h"
// ============================================================================
// local
// ============================================================================
#include "DistanceCalculatorBase.h"
#include "FitterUtils.h"
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
 *  @date   2006-04-12
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
  /** @class TrgDistanceCalculator
   *
   *  It is the fast "trigger-oriented" implementation of the basic math, 
   *  needed for the real implementation 
   *  on the abstract interface IDistanceCalculator 
   *  Essentially it relies on many nice functions, 
   *  coded by Juan & Matt  
   *  
   *  @see IDistanceCalculator 
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
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-03-05
   */
  class TrgDistanceCalculator 
    : public extends1<LoKi::DistanceCalculatorBase,IDistanceCalculator>
  {
    // ========================================================================
    // the friend factory for instantiation 
    friend class ToolFactory<LoKi::TrgDistanceCalculator> ;
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
      double&                 imppar   , 
      const bool              allow    ) const 
    {
      StatusCode sc = check ( particle , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , *vertex , impact , allow ) ;
      imppar = impact.R() ;
      return sc ;                                                     // RETURN 
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
      double&                 chi2     , 
      const bool              allow    ) const 
    {
      StatusCode sc = check ( particle , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , *vertex , impact , allow , &chi2 ) ;
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
      double&                 imppar   , 
      const bool              allow    ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , point , impact , allow ) ;
      imppar = impact.R() ;
      return sc ;                                                     // RETURN 
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
      double&                 chi2     , 
      const bool              allow    ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                         // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , point , impact , allow , &chi2 ) ;
      imppar = impact.R() ;
      return sc ;                                                    // RETURN 
    }
    // ========================================================================    
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup IPVector
     *   Evalaution of the vector distance between the particle and point/vertex 
     *  @{
     */
    // ========================================================================
    /** The method for the evaluation of the vector impact parameter
     *  vector of the particle with respect to the fixed point 
     *  @param particle (input) pointer to the particle 
     *  @param point    (input) the fixed point  
     *  @param impact   (output) the impact parameter vector 
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const Gaudi::XYZPoint&  point    , 
      Gaudi::XYZVector&       impact   , 
      const bool              allow    ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the proper evaluations 
      sc = _distance ( *particle , point , impact , allow ) ;
      return sc ;                                                     // RETURN 
    }
    // ========================================================================    
    /** The method for the evaluation of the vector impact parameter 
     *  vector of the particle with respect to the vertex 
     *  @param particle (input) pointer to the particle 
     *  @param point    (input) the fixed point  
     *  @param impact   (output) the impact parameter vector 
     *  @return status code 
     */
    virtual StatusCode distance 
    ( const LHCb::Particle*   particle ,
      const LHCb::VertexBase* vertex   , 
      Gaudi::XYZVector&       impact   , 
      const bool              allow    ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      // make the proper evaluations 
      sc = _distance ( *particle , *vertex , impact , allow ) ;
      return sc ;                                                     // RETURN 
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
      if ( sc.isFailure() ) { return sc ; }               // RETURN 
      //
      if ( v1 == v2 ) 
      {
        dist = 0 ;
        return _Warning("distance(v,v): the same vertex",StatusCode::SUCCESS ) ; 
      }
      // make the real calculations 
      return i_distance ( *v1 , *v2 , dist ) ;
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
      //
      if ( v1 == v2 ) 
      {
        dist = 0 ;
        chi2 = 0 ;
        return _Warning("distance(v,v): the same vertex",StatusCode::SUCCESS ) ; 
      }
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
    ( const LHCb::Particle* p1    , 
      const LHCb::Particle* p2    , 
      double&               dist  , 
      const bool            allow ) const 
    {
      StatusCode sc = check ( p1 , p2 ) ;
      if ( sc.isFailure() ) { return sc ; }                    // RETURN 
      //
      if ( p1 == p2 ) 
      {
        dist = 0 ;
        return _Warning("distance(p,p): the same particle",StatusCode::SUCCESS ) ; 
      }
      // make the real calculations 
      return _distance ( *p1 , *p2 , dist , allow ) ;         // RETURN  
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
    ( const LHCb::Particle* p1    , 
      const LHCb::Particle* p2    ,
      double&               dist  , 
      double&               chi2  , 
      const bool            allow ) const 
    { 
      StatusCode sc = check ( p1 , p2 ) ;
      if ( sc.isFailure() ) { return sc ; }                      // RETURN 
      //
      if ( p1 == p2 ) 
      {
        dist = 0 ;
        chi2 = 0 ;
        return _Warning("distance(p,p): the same particle",StatusCode::SUCCESS ) ; 
      }
      // make the real calculations 
      return _distance ( *p1 , *p2 , dist , allow , &chi2 ) ;    // RETURN 
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
     *  @param   error    (output) the estimate of the uncertanty in 
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
      if ( sc.isFailure() ) { return sc ; }                     // RETURN 
      // check the end-vertex
      const LHCb::VertexBase* decay = particle->endVertex() ;
      sc = check ( decay ) ;
      if ( sc.isFailure() ) { return sc ; }                     // RETURN 
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
      if ( sc.isFailure() ) { return sc ; }                     // RETURN 
      // check the end-vertex
      const LHCb::VertexBase* decay = particle->endVertex() ;
      sc = check ( decay ) ;
      if ( sc.isFailure() ) { return sc ; }                     // RETURN 
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
      if ( sc.isFailure() ) { return sc ; }                     // RETURN 
      // check the end-vertex
      const LHCb::VertexBase* decay = particle->endVertex() ;
      sc = check ( decay ) ;
      if ( sc.isFailure() ) { return sc ; }                     // RETURN 
      // make the real evaluation:
      return _distance ( *primary  , *particle , *decay , dist , &error ) ;
    }
    // ========================================================================
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup  TrackVertex  
     *  Evaluation of the distance between the track and vertex 
     *  @{
     */
    // ========================================================================    
    /** evaluate the impact parameter of the track with respect to the vertex 
     *  @param track (input)   the track 
     *  @param vertex (input)  the vertex 
     *  @param imppar (output) the value of impact parameter
     */
    virtual StatusCode distance 
    ( const LHCb::Track*      track    ,
      const LHCb::VertexBase* vertex   , 
      double&                 imppar   ) const 
    {
      StatusCode sc = check ( track , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }
      Gaudi::XYZVector impact ;
      sc = _distance ( *track , *vertex , impact ) ;
      imppar = impact.R() ;
      return sc ;
    }
    // ========================================================================    
    /** evaluate the impact parameter of the track with respect to the vertex 
     *  @param track (input)   the track 
     *  @param vertex (input)  the vertex 
     *  @param imppar (output) the value of impact parameter
     *  @param chi2   (output) chi2 of impact parameter 
     */
    virtual StatusCode distance 
    ( const LHCb::Track*      track    ,
      const LHCb::VertexBase* vertex   , 
      double&                 imppar   , 
      double&                 chi2     ) const 
    {
      StatusCode sc = check ( track , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }
      Gaudi::XYZVector impact ;
      sc = _distance ( *track , *vertex , impact , &chi2 ) ;
      imppar = impact.R() ;
      return sc ;
    }
    /** evaluate the impact parameter of the track with respect to the vertex 
     *  @param track (input)   the track 
     *  @param vertex (input)  the vertex 
     *  @param imppar (output) the value of impact parameter
     */
    virtual StatusCode distance 
    ( const LHCb::Track*      track    ,
      const LHCb::VertexBase* vertex   , 
      Gaudi::XYZVector&       impact   ) const 
    {
      StatusCode sc = check ( track , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }
      return _distance ( *track , *vertex , impact ) ;
    }
    // ========================================================================    
    /// @}
    // ========================================================================
  public:
    // ========================================================================
    /** @defgroup  TrackTrack
     *  Evaluation of the distance between the tracks
     *  @{
     */
    // ========================================================================    
    /** evaluate the distance between two tracks
     *  @param track1 (input)  the first track 
     *  @param track2 (input)  the second track  
     *  @param doca   (output) the value of distance 
     */
    virtual StatusCode distance 
    ( const LHCb::Track*      track1 ,
      const LHCb::Track*      track2 ,
      double&                 doca   ) const 
    {
      StatusCode sc = check ( track1 , track2 ) ;
      if ( sc.isFailure() ) { return sc ; }
      //
      if ( track1 == track2 ) 
      {
        doca = 0 ;
        return _Warning("distance(t,t): the same track",StatusCode::SUCCESS ) ; 
      }
      //
      return _distance ( *track1 , *track2 , doca ) ;
    }
    // ========================================================================    
    /** evaluate the distance between two tracks
     *  @param track1 (input)  the first track 
     *  @param track2 (input)  the second track  
     *  @param doca   (output) the value of distance 
     *  @param chi2   (output) the chi2 of distance 
     */
    virtual StatusCode distance 
    ( const LHCb::Track*      track1 ,
      const LHCb::Track*      track2 ,
      double&                 doca   , 
      double&                 chi2   ) const 
    {
      StatusCode sc = check ( track1 , track2 ) ;
      if ( sc.isFailure() ) { return sc ; }
      //
      if ( track1 == track2 ) 
      {
        doca = 0 ;
        chi2 = 0 ;
        return _Warning("distance(t,t): the same track",StatusCode::SUCCESS ) ; 
      }
      //
      return _distance ( *track1 , *track2 , doca , &chi2 ) ;
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
    TrgDistanceCalculator
    ( const std::string& type   ,    // tool type (?)
      const std::string& name   ,    // tool instance name 
      const IInterface*  parent ) ;  // the parent 
    // virtual and protected desctrustor 
    virtual ~TrgDistanceCalculator (){}  // Destructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    TrgDistanceCalculator () ; // no default constructor 
    /// the copy    constructor is disabled 
    TrgDistanceCalculator ( const TrgDistanceCalculator&) ; // no copy 
    /// the assignement operator is disabled 
    TrgDistanceCalculator& operator=( const TrgDistanceCalculator&) ;
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
    ( const LHCb::Particle&   particle         ,
      const LHCb::VertexBase& vertex           , 
      Gaudi::XYZVector&       imppar           , 
      const bool              allow            ,
      double*                 chi2     = 0     ) const ;
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
      const bool              allow        , 
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
      const bool            allow    , 
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
    /** The method for the evaluation of the impact parameter ("distance") 
     *  vector of the particle with respect to some vertex. 
     *  @param track    (input)  the track 
     *  @param vertex   (input)  the vertex 
     *  @param imppar   (output) the impact parameter ("distance") vector 
     *  @param chi2     (output,optional) the chi2 of distance 
     *  @return status code 
     */
    StatusCode _distance 
    ( const LHCb::Track&      track        ,
      const LHCb::VertexBase& vertex       , 
      Gaudi::XYZVector&       imppar       , 
      double*                 chi2     = 0 ) const ;
    // ========================================================================
    /** The method for the evaluation of the distance between two tracks 
     *  @param track1  (input)  the first tarck 
     *  @param track2  (input)  the first tarck 
     *  @param doca    (output) the distance 
     *  @param chi2    (output,optional) the chi2 of distance 
     *  @return status code 
     */
    StatusCode _distance 
    ( const LHCb::Track&      track1       ,
      const LHCb::Track&      track2       ,
      double&                 doca         ,
      double*                 chi2     = 0 ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// Kalman filter object: 
    mutable LoKi::KalmanFilter::Entry   m_entry   ; // Kalman filter object
    /// Kalman filter objects: 
    mutable LoKi::KalmanFilter::Entries m_entries ; // Kalman filter objects
    /// distance/path fitter
    mutable LoKi::Fitters::Fitter1      m_fitter  ; // distance/path fitter
    // ========================================================================
  private:  // tracks without momentum information 
    // ========================================================================
    /// Kalman filter object: 
    mutable LoKi::KalmanFilter::TrEntry4   m_entry4   ; // Kalman filter object
    /// Kalman filter objects: 
    mutable LoKi::KalmanFilter::TrEntries4 m_entries4 ; // Kalman filter objects
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
LoKi::TrgDistanceCalculator::TrgDistanceCalculator
( const std::string& type   , // tool type (?)
  const std::string& name   , // tool instance name 
  const IInterface*  parent ) // the parent 
  : base_class ( type , name , parent ) 
{
  StatusCode sc = setProperty ( "MaxPrints" , 0 ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'MaxPrints'"    , sc ) ;
}
// ============================================================================
/*  The method for the evaluation of the impact parameter ("distance")
 *  vector of the particle with respect to some vertex. 
 */
// ============================================================================
StatusCode LoKi::TrgDistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const LHCb::VertexBase& vertex   , 
  Gaudi::XYZVector&       impact   , 
  const bool              allow    , 
  double*                 chi2     ) const 
{
  // 
  if ( allow )
  {
    // the first particle
    LoKi::KalmanFilter::ParticleType type1 = particleType_ ( particle ) ;
    switch ( type1 ) 
    {
    case LoKi::KalmanFilter::GammaLikeParticle   : ;
    case LoKi::KalmanFilter::DiGammaLikeParticle :
      {
        impact =  Gaudi::XYZVector() ;
        if ( 0 != chi2 ) { *chi2 = 0 ; }
        if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "IP->(DI)GAMMA" ) ; }
        return StatusCode::SUCCESS ;                                    // RETURN 
      }
    case LoKi::KalmanFilter::ShortLivedParticle : 
      {
        const LHCb::VertexBase* v1 = particle.endVertex() ;
        if ( 0 == v1 ) { break ; }  
        if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "IP->VD" ) ; }
        // make the real calculations 
        double dist = 0 ;
        StatusCode sc = i_distance ( *v1 , vertex , dist , chi2 ) ; // RETURN 
        impact = v1->position() - vertex.position () ;
        return sc ;
      } 
    default: ;
    }    
  }
  //
  // start normal processing 
  //
  using namespace Gaudi::Math::Operators ;
  //
  // make the fast evaluation:
  i_distance ( particle , vertex , impact ) ;
  
  // evaluate chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = -1.e+10 ;
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::loadAsFlying ( particle , m_entry ) ;
    if ( sc.isFailure() ) 
    { return _Error("_distance(I): error from KalmanFilter::load", sc ) ; }
    // get the "the previus" Kalman Filter estimate == vertex
    Gaudi::SymMatrix3x3 ci = vertex.covMatrix() ; // the gain matrix 
    if ( !ci.Invert() ) 
    { return _Error ( "_distance(I): unable to calculate the gain matrix" ) ; }
    // make one step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( m_entry , vertex.position() , ci , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "_distance(I): error from Kalman Filter step" , sc ) ; }
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
StatusCode LoKi::TrgDistanceCalculator::_distance
( const LHCb::Particle&   particle ,
  const Gaudi::XYZPoint&  point    , 
  Gaudi::XYZVector&       impact   , 
  const bool              allow    , 
  double*                 chi2     ) const 
{ 
  //
  //
  if ( allow )
  {
    // the first particle
    LoKi::KalmanFilter::ParticleType type1 = particleType_ ( particle ) ;
    switch ( type1 ) 
    {
    case LoKi::KalmanFilter::GammaLikeParticle   : ;
    case LoKi::KalmanFilter::DiGammaLikeParticle :
      {
        impact =  Gaudi::XYZVector() ;
        if ( 0 != chi2 ) { *chi2 = 0 ; }
        if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "IP->(DI)GAMMA" ) ; }
        return StatusCode::SUCCESS ;                                    // RETURN 
      }
    case LoKi::KalmanFilter::ShortLivedParticle : 
      {
        const LHCb::VertexBase* v1 = particle.endVertex() ;
        if ( 0 == v1 ) { break ; }  
        if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "IP->VD" ) ; }
        // make the real calculations 
        double dist   = 0 ;
        StatusCode sc = i_distance ( *v1 , point , dist , chi2 ) ;
        impact        = v1->position() - point ;
        return sc ;
      } 
    default: ;
    }    
  }
  //
  using namespace Gaudi::Math::Operators ;
  
  // make the fast evaluation:
  i_distance ( particle , point , impact ) ;
  
  // evaluate the chi2 (if needed) 
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = -1.e+10 ;
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::loadAsFlying ( particle , m_entry ) ;
    if ( sc.isFailure() ) 
    { return _Error("_distance(II): error from KalmanFilter::load" , sc ) ; }
    // here the evaluations of chi2 is just trivial:
    *chi2 = Gaudi::Math::Similarity ( m_entry.m_vxi , impact ) ;
    // ========================================================================
  }  
  //
  return StatusCode::SUCCESS ;                                 // RETURN 
}
// ============================================================================
/* The method for evaluation of the scalar distance between two particles, 
 * aka "distance of the closest approach" and also its 
 *  chi^2-for separation significance 
 */
// ============================================================================
StatusCode LoKi::TrgDistanceCalculator::_distance
( const LHCb::Particle& p1    , 
  const LHCb::Particle& p2    ,
  double&               dist  , 
  const bool            allow , 
  double*               chi2  ) const 
{
  //
  if ( &p1 == &p2 ) 
  {
    dist = 0 ;
    if ( 0 != chi2 ) { *chi2 = 0 ; }
    return _Warning("distance(p,p): the same particle",StatusCode::SUCCESS ) ;  
  }
  //
  if ( allow )
  {
    // the first particle
    LoKi::KalmanFilter::ParticleType type1 = particleType_ ( p1 ) ;
    switch ( type1 ) 
    {
    case LoKi::KalmanFilter::GammaLikeParticle   : ;
    case LoKi::KalmanFilter::DiGammaLikeParticle :
      {
        dist = 0 ;
        if ( 0 != chi2 ) { *chi2 = 0 ; }
        if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "DOCA->(DI)GAMMA" ) ; }
        return StatusCode::SUCCESS ;                                    // RETURN 
      }
    case LoKi::KalmanFilter::ShortLivedParticle : 
      {
        const LHCb::VertexBase* v1 = p1.endVertex() ;
        if ( 0 == v1 ) { break ; }  
        if ( printStat() && msgLevel ( MSG::INFO ) ) { ++counter ( "DOCA->IP" ) ; }
        // make the proper evaluations 
        Gaudi::XYZVector impact ;
        StatusCode sc  = _distance ( p2 , *v1 , impact , allow , chi2 ) ;
        dist           = impact.R() ;
        return sc  ;                                                    // RETURN 
      } 
    default: ;
    }    
    // the second particle
    LoKi::KalmanFilter::ParticleType type2 = particleType_ ( p2 ) ;
    switch ( type2 ) 
    {
    case LoKi::KalmanFilter::GammaLikeParticle   : ;
    case LoKi::KalmanFilter::DiGammaLikeParticle :
      {
        dist = 0 ;
        if ( 0 != chi2 ) { *chi2 = 0 ; }
        if ( printStat() && msgLevel( MSG::INFO ) ) { ++counter ( "DOCA->(DI)GAMMA" ) ; }
        return StatusCode::SUCCESS ;                                    // RETURN 
      }
    case LoKi::KalmanFilter::ShortLivedParticle :
      {
        const LHCb::VertexBase* v2 = p2.endVertex() ;
        if ( 0 == v2 ) { break ; }  
        if ( printStat() && msgLevel ( MSG::INFO ) ) { ++counter ( "DOCA->IP" ) ; }
        // make the proper evaluations 
        Gaudi::XYZVector impact ;
        StatusCode sc  = _distance ( p1 , *v2 , impact , allow , chi2 ) ;
        dist           = impact.R() ;
        return sc ;                                                    // RETURN 
      }
    default: ;
    }
  }
  //
  // start normal processing  
  //
  using namespace Gaudi::Math::Operators ;
  
  Gaudi::XYZPoint point1 ;
  Gaudi::XYZPoint point2 ;
  //
  // make the evaluation of the distance:
  //
  i_distance ( p1 , p2 , point1 , point2 ) ;
  //
  // evaluate the distance 
  //
  dist = ( point1 - point2 ) . R () ;
  //
  // evaluate chi2 (if needed) 
  //
  if ( 0 != chi2 ) 
  {
    // =======================================================================
    *chi2 = 1.e+10 ;
    //
    // prepare the Kalman Filter machinery
    //
    m_entries.resize(2) ;
    LoKi::KalmanFilter::Entries::iterator first  = m_entries.begin() ;
    LoKi::KalmanFilter::Entries::iterator second = first + 1         ;
    //
    // load as long-lived particles: 
    //
    StatusCode sc = LoKi::KalmanFilter::load 
      ( p1 , LoKi::KalmanFilter::LongLivedParticle , *first  ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::load(1)" , sc ) ; }
    //
    sc            = LoKi::KalmanFilter::load 
      ( p2 , LoKi::KalmanFilter::LongLivedParticle , *second ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::load(2)" , sc ) ; }
    //
    // make the special step of Kalman filter 
    //
    sc = LoKi::KalmanFilter::step ( *first  , *second , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(III): error from KalmanFilter::step(2)" , sc ) ; }
    //
    // get the final chi2 
    //
    *chi2 = second->m_chi2 ;
    // ========================================================================
  }
  //
  return StatusCode::SUCCESS ;                                 // RETURN
}
// ========================================================================
// the method for the evaluation of "path"-distance
// ========================================================================
StatusCode LoKi::TrgDistanceCalculator::_distance 
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
  
  // get the first estimate of the path parameter
  StatusCode sc = LoKi::Fitters::path0 ( primary , particle , decay , path ) ;
  if ( sc.isFailure() ) 
  {
    _Warning ( "Error code from LoKi::Fitters::path0" , sc ) ;
    path = 0 ;
  }
  
  // get the expansion point:
  Gaudi::LorentzVector momentum   = particle .  momentum       () ;
  Gaudi::XYZPoint      decvertex  = particle .  referencePoint () ;
  Gaudi::XYZPoint      primvertex = primary  .  position       () ;
  m_fitter.m_var = path ;
  
  // make one step of the fit:
  sc = LoKi::Fitters::path_step 
    ( primary , particle , momentum , decvertex , primvertex , m_fitter ) ;
  if ( sc.isFailure() ) 
  { return _Error ( "_pathDistance(): error from path_step" , sc ) ; }
  
  // get the parameters from the fitter
  path  = m_fitter.m_var  ;
  chi2  = m_fitter.m_chi2 ;
  
  // get the error in "path" 
  error = ::sqrt ( m_fitter.m_Vvar ) ;
  //
  return StatusCode::SUCCESS ;                                   // RETURN 
  //
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
StatusCode LoKi::TrgDistanceCalculator::_distance   
( const LHCb::VertexBase& primary  ,         // the production vertex 
  const LHCb::Particle&   particle ,         // the particle  
  const LHCb::VertexBase& decay    ,         // the production vertex 
  double&                 dist     ,         // the distance 
  double*                 error    ) const   // the error in distance 
{
  
  using namespace Gaudi::Math::Operators ;
  
  // evaluate the projected distance 
  dist = i_distance ( primary  , particle , decay ) ;
  
  // evaluate the error? 
  if ( 0 == error ) { return StatusCode::SUCCESS ; }         // RETURN 
  //
  const Gaudi::XYZPoint& vd   = decay    . position() ;
  const Gaudi::XYZPoint& vpv  = primary  . position() ;
  const Gaudi::XYZVector  p   = particle . momentum() . Vect () ;
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
    ( ROOT ::Math::Similarity ( dHdp4 , particle .    momCovMatrix ()     ) + 
      2 * ROOT::Math::Dot     ( dHdp4 , particle . posMomCovMatrix () * p ) + 
      Gaudi::Math::Similarity ( dHdvd , decay    .       covMatrix ()     ) + 
      Gaudi::Math::Similarity ( dHdvd , primary  .       covMatrix ()     ) );
  
  // v_s = ( E^T * V_D * E ) ^ {-1}
  //  where E = d(H)/d(s) = -|p|  
  const double V_s = 1.0 / ( V_D * p.Mag2() ) ;
  //
  if ( 0 <= V_s ) { *error = ::sqrt ( V_s ) ; }
  else 
  {
    *error = -1 ;
    return _Error ( "The negative covariance, return error=-1" ) ;   // RETURN 
  }
  //
  return StatusCode::SUCCESS ;                                       // RETURN
}
// ============================================================================
/*  The method for the evaluation of the impact parameter ("distance") 
 *  vector of the particle with respect to some vertex. 
 *  @param tarck    (input)  the tarck 
 *  @param vertex   (input)  the vertex 
 *  @param imppar   (output) the impact parameter ("distance") vector 
 *  @param chi2     (output,optional) the chi2 of the impact parameter
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::TrgDistanceCalculator::_distance   
( const LHCb::Track&      track  ,
  const LHCb::VertexBase& vertex , 
  Gaudi::XYZVector&       imppar , 
  double*                 chi2   ) const 
{
  //
  // get the closest state 
  // 
  const LHCb::State& s = state ( track , vertex.position().Z() ) ;
  //
  // get the distance 
  // 
  i_distance ( s , vertex , imppar ) ;
  //
  // evaluate chi2 (if needed) 
  //
  if ( 0 != chi2 ) 
  {
    // ========================================================================
    *chi2 = -1.e+10 ;
    // prepare the Kalman Filter machinery 
    StatusCode sc = LoKi::KalmanFilter::load ( &s , m_entry4 ) ;
    if ( sc.isFailure() ) 
    { return _Error("_distance(IV): error from KalmanFilter::load", sc ) ; }
    // get the "the previus" Kalman Filter estimate == vertex
    Gaudi::SymMatrix3x3 ci = vertex.covMatrix() ; // the gain matrix 
    if ( !ci.Invert() ) 
    { return _Error ( "_distance(IV): unable to calculate the gain matrix" ) ; }
    // make one step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( m_entry4 , vertex.position() , ci , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "_distance(IV): error from Kalman Filter step" , sc ) ; }
    // get the chi2 
    *chi2 = m_entry4.m_chi2 ;
    // ========================================================================
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  The method for the evaluation of the distance between two tracks 
 *  @param track1  (input)  the first tarck 
 *  @param track2  (input)  the first tarck 
 *  @param doca    (output) the distance 
 *  @param chi2    (output,optional) the chi2 of distance 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::TrgDistanceCalculator::_distance   
( const LHCb::Track&      track1 ,
  const LHCb::Track&      track2 ,
  double&                 doca   ,
  double*                 chi2   ) const 
{
  //
  if ( &track1 == &track2 ) 
  {
    doca = 0 ;
    if ( 0 != chi2 ) { *chi2 = 0 ; }
    return _Warning("distance(t,t): the same track",StatusCode::SUCCESS ) ;  
  }
  //
  Gaudi::XYZPoint point1 ;
  Gaudi::XYZPoint point2 ;
  //
  const LHCb::State& s1 = state ( track1 );
  const LHCb::State& s2 = state ( track2 );
  // make the evaluation of the distance:
  i_distance ( s1 , s2 , point1 , point2 ) ;
  //
  // evaluate the distance 
  //
  doca = ( point1 - point2 ) . R () ;
  //
  // evaluate chi2 (if needed) 
  //
  if ( 0 != chi2 ) 
  {
    // =======================================================================
    *chi2 = 1.e+10 ;
    // prepare the Kalman Filter machinery
    m_entries4.resize ( 2 ) ;
    LoKi::KalmanFilter::TrEntries4::iterator first  = m_entries4.begin() ;
    LoKi::KalmanFilter::TrEntries4::iterator second = first + 1          ;   
    //
    StatusCode sc = LoKi::KalmanFilter::load ( &s1 , *first  ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "_distance(V): error from KalmanFilter::load(1)" , sc ) ; }
    //
    sc =            LoKi::KalmanFilter::load ( &s2 , *second ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "_distance(V): error from KalmanFilter::load(2)" , sc ) ; }
    //
    // make the special step of Kalman filter 
    sc = LoKi::KalmanFilter::step ( *first  , *second , 0 ) ;
    if ( sc.isFailure() ) 
    { return _Error ( "distance(V): error from KalmanFilter::step(2)" , sc ) ; }
    //
    // get the final chi2 
    *chi2 = second->m_chi2 ;
    // ========================================================================
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,TrgDistanceCalculator)
// ============================================================================
// The END 
// ============================================================================
