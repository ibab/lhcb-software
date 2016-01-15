#ifndef DISTANCECALCULATOR_H 
#define DISTANCECALCULATOR_H 1
// $Id: DistanceCalculator.cpp 199656 2016-01-14 10:50:07Z ibelyaev $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// KalmanFilter 
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
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-03-05
 *  
 *                    $Revision: 199656 $
 *  Last modification $Date: 2016-01-14 11:50:07 +0100 (Thu, 14 Jan 2016) $
 *                 by $Author: ibelyaev $
 * 
 */
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
      double&                 imppar   , 
      const bool              allow    ) const 
    {
      StatusCode sc = check ( particle , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , *vertex , impact , allow ) ;
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
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      Gaudi::XYZVector impact ;
      sc = _distance ( *particle , point , impact , allow ) ;
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
      double&                 chi2     , 
      const bool              allow    ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
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
      Gaudi::XYZVector&       impact   ,
      const bool              allow    ) const 
    { 
      StatusCode sc = check ( particle ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      sc = _distance ( *particle , point , impact , allow ) ;
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
      Gaudi::XYZVector&       impact   , 
      const bool              allow    ) const 
    { 
      StatusCode sc = check ( particle , vertex ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN 
      // make the proper evaluations 
      sc = _distance ( *particle , *vertex , impact , allow ) ;
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
      //
      if ( v1 == v2 ) 
      {
        dist = 0 ;
        return _Warning( "distance(v,v): the same vertex",StatusCode::SUCCESS,0 ) ;  
        
      }
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
      //
      if ( v1 == v2 ) 
      {
        dist = 0 ;
        chi2 = 0 ;
        return _Warning("distance(v,v): the same vertex",StatusCode::SUCCESS,0);  
        
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
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      //
      if ( p1 == p2 ) 
      {
        dist = 0 ;
        return _Warning("distance(p,p): the same particle",StatusCode::SUCCESS,0);  
        
      }
      // make the real calculations 
      return _distance ( *p1 , *p2 , dist , allow ) ;                // RETURN  
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
      if ( sc.isFailure() ) { return sc ; }                           // RETURN 
      //
      if ( p1 == p2 ) 
      {
        dist = 0 ;
        chi2 = 0 ;
        return _Warning("distance(p,p): the same particle",StatusCode::SUCCESS,0);  
        
      }
      // make the real calculations 
      return _distance ( *p1 , *p2 , dist , allow , &chi2 ) ;         // RETURN  
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
        return _Warning("distance(t,t): the same track",StatusCode::SUCCESS,0);
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
        return _Warning("distance(t,t): the same track",StatusCode::SUCCESS,0);
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
  protected:
    // ========================================================================
    /// use fast (==no transportation, no iteration) algorithms?
    bool    fastalgo() const { return m_fastalgo ; }
    /// use fast (==no transportation, no iteration) algorithms?
    void setFastAlgo ( const bool value ) { m_fastalgo = value ; }
    // ========================================================================
  private:
    // ========================================================================
    /// get the state from the track at certain Z  
    inline StatusCode stateAtZ  
    ( LHCb::State&       s ,  
      const LHCb::Track& t , 
      const double       z ) const 
    {
      //
      const double tolerance = 0.35 * m_deltaZ ;
      //
      // For long tracks in vicinity of origin point, use trajectory approximation.
      // According to Wouter it should be much more CPU efficient.
      if ( t.checkType ( LHCb::Track::Long ) && 
           m_region.first <= z &&  z <= m_region.second ) 
      {
        StatusCode sc = stateProvider()->stateFromTrajectory ( s , t , z ) ;
        if ( sc.isSuccess() ) { return sc ; }
      }
      //
      StatusCode sc = stateProvider() -> state ( s , t , z , 0.9 * tolerance ) ;
      if ( sc.isSuccess() && std::fabs ( s.z() - z ) < tolerance ) { return sc ; }
      //
      _Warning ( "stateAtZ: unable to get the proper state with provider"     , sc ) ;
      //
      sc = extrapolator()->propagate ( t , z , s ) ;
      if ( sc.isSuccess() && std::fabs ( s.z() - z ) < tolerance ) { return sc ; }
      //
      _Warning ( "stateAtZ: unable to get the proper state with extrapolator" , sc ) ;
      //
      s = t.closestState ( z ) ;
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /* The method for the evaluation of the impact parameter ("distance") 
     *  vector of the particle with respect to some vertex. 
     *  @param particle (input) pointer to the particle 
     *  @param vertex   (input) pointer to the vertex 
     *  @param imppar   (output) the impact parameter ("distance") vector 
     *  @param chi2     (output,optional) the chi2 of the inmpact parameter
     *  @return status code 
     */
    // ========================================================================
    StatusCode _distance
    ( const LHCb::Particle&   particle     ,
      const LHCb::VertexBase& vertex       , 
      Gaudi::XYZVector&       impact       , 
      const bool              allow        , 
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
      const  bool             allow        , 
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
  private: // properties:
    // ========================================================================
    /// the maximal number of iterations 
    unsigned int m_nIter_max  ; // the maximal number of iterations 
    /// the convergency criterion for impact parameter evaluations 
    double       m_deltaZ     ; // the convergency criterion for ip-evaluations
    /// the convergency criterion for delta(chi2) 
    double       m_delta_chi2 ; // the convergency criterion for delta(chi2) 
    // the convergency criterion for delta(path)
    double       m_delta_path ; // the convergency criterion for delta(path)
    // the ragion where Trajectory" approximation is good for long tracks 
    std::pair<double, double>  m_region ;
    // use fast (no transportation, no iteration) algorithms?
    bool         m_fastalgo   ;
    // ========================================================================
  private: /// some local objects 
    // ========================================================================
    mutable LHCb::Particle                m_particle1 ;  // some particle 
    /// another static particle 
    mutable LHCb::Particle                m_particle2 ; // some particle 
    /// Kalman filter object: 
    mutable LoKi::KalmanFilter::Entry     m_entry     ; // Kalman filter object
    /// Kalman filter objects: 
    mutable LoKi::KalmanFilter::Entries   m_entries   ; // Kalman filter objects
    /// distance/path fitter
    mutable LoKi::Fitters::Fitter1        m_fitter    ; // distance/path fitter
    // ========================================================================
    /// track state to bve used 
    mutable LHCb::State                   m_state1 ; // track state to bve used 
    mutable LHCb::State                   m_state2 ; // track state to bve used 
    // ========================================================================
    // tracks without momentum information 
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
//                                                                      The END 
// ============================================================================
#endif // DISTANCECALCULATOR_H
// ============================================================================
