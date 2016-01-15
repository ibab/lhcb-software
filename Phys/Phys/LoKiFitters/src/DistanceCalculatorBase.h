// $Id$
// ============================================================================
#ifndef LOKIFITTERS_DISTANCECALCULATORBASE_H 
#define LOKIFITTERS_DISTANCECALCULATORBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/VertexBase.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// TrackInterfaces
// ============================================================================
#include "TrackInterfaces/ITrackStateProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
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
// Local 
// ============================================================================
#include "ParticleClassificator.h"
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
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class DistanceCalculatorBase DistanceCalculatorBase.h
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
  class DistanceCalculatorBase : public LoKi::ParticleClassificator
  {
  public:
    // ========================================================================
    /// get the actual type of the line  
    typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line_ ;
    // ========================================================================
  public:
    // ========================================================================
    /// the error code
    enum {
      /// Invalid Particle 
      InvalidParticle         = 901 , // Invalid Particle 
      /// Invalid Vertex 
      InvalidVertex           = 902 , // Invalid Vertex 
      /// Invalid Track 
      InvalidTrack            = 903 , // Invalid Vertex 
      /// Invalid Data
      InvalidData             = 904 , // Invalid Data 
      /// Error in Matrix Inversion 
      ErrorInMatrixInversion  = 905 , // Error in Matrix Inversion
      /// Error from transporter 
      ErrorFromTransporter    = 906 , // Error from Particle Transporter
      /// No Convergency
      NoConvergency           = 910 , // No Convergency is detected 
      /// Not Implemented Yet 
      NotImplemented          = 920   // not yet implemented 
    } ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize 
    virtual StatusCode initialize () ;
    // ========================================================================
    /// finalize 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// construct the line trajectory from the particle  
    inline Line_  line   ( const LHCb::Particle& particle ) const ;
    /// construct the line trajectory from the track
    inline Line_  line   ( const LHCb::State&    state    ) const ;
    // ========================================================================
    /** find the impact parameter ("distance") vector 
     *  of the particle with respect to some vertex 
     *  @param particle (input) the particle 
     *  @param vertex   (input) the vertex 
     *  @param impact   (output) the evaluated impact parameter vector 
     */  
    inline void i_distance 
    ( const LHCb::Particle&   particle , 
      const LHCb::VertexBase& vertex   , 
      Gaudi::XYZVector&       impact   ) const ;
    // =========================================================================
    /** find the impact parameter ("distance") vector 
     *  of the particle with respect to some fixed point 
     *  @param particle (input) the particle 
     *  @param point    (input) the fixed point 
     *  @param impact   (output) the evaluated impact parameter vector 
     */  
    inline void i_distance 
    ( const LHCb::Particle&   particle , 
      const Gaudi::XYZPoint&  point    , 
      Gaudi::XYZVector&       impact   ) const ;
    // ========================================================================
    /** find the points of the closest approach for two  particles
     *  @param particel1 (input) the first particle 
     *  @param particle2 (input) the second particle 
     *  @param point1    (output) the point on the first trajectory 
     *  @param point2    (output) the point on the second trajectory
     */
    inline void i_distance 
    ( const LHCb::Particle&   particle1 ,  // the first particle 
      const LHCb::Particle&   particle2 ,  // the second particle 
      Gaudi::XYZPoint&        point1    ,  // the point on the first trajectory 
      Gaudi::XYZPoint&        point2    ) const ;  
    // ========================================================================
    /**  evaluate the  distance between two vertices 
     *   @param vx1  (input) the first vertex 
     *   @param vx2  (input) the second vertex 
     *   @param dist (output) the distance between vertices 
     *   @param chi2 (output,optional) the chi2 separation significance 
     */
    inline StatusCode  i_distance 
    ( const LHCb::VertexBase& vx1      , 
      const LHCb::VertexBase& vx2      , 
      double&                 dist     , 
      double*                 chi2 = 0 ) const ;
    // ========================================================================
    /**  evaluate the  distance between the vertex and the fixed point point 
     *   @param vertex (input) the vertex 
     *   @param point  (input) the fixed point
     *   @param dist   (output) the distance between vertices 
     *   @param chi2   (output,optional) the chi2 separation significance 
     */
    inline StatusCode i_distance 
    ( const LHCb::VertexBase& vertex   , 
      const Gaudi::XYZPoint&  point    , 
      double&                 dist     , 
      double*                 chi2 = 0 ) const ;
    // ========================================================================
    /* evalute the "projected" distance 
     * 
     *  \f$s=\frac{\left(\vec{\mathbf{v}}\vec{\mathbf{p}}
     *     \right)}{\left|\vec{\mathbf{p}}\right|}\f$
     *  where vector \f$\vec{\mathbf{v}}\f$ is a vector from 
     *  the primary to the secondary vertex: 
     *    \f$\vec{\mathbf{v}}=\vec{\mathbf{x}}_{d}-\vec{\mathbf{x}}_{pv}\f$,
     *
     * @param primary  (input)  the production vertex 
     * @param particle (input)  the particle 
     * @param decay    (input)  the decay particle 
     * @param return the projected distance
     * @return status code 
     */ 
    inline double i_distance 
    ( const LHCb::VertexBase& primary  , 
      const LHCb::Particle&   particle , 
      const LHCb::VertexBase& decay    ) const ;
    // ========================================================================
  protected: // track-related distances 
    // ========================================================================
    /** find the impact parameter ("distance") vector 
     *  of the particle with respect to some vertex 
     *  @param state    (input) the track state 
     *  @param vertex   (input) the vertex 
     *  @param impact   (output) the evaluated impact parameter vector 
     */  
    inline void i_distance 
    ( const LHCb::State&      state    , 
      const LHCb::VertexBase& vertex   , 
      Gaudi::XYZVector&       impact   ) const ;
    // =========================================================================
    /** find the impact parameter ("distance") vector 
     *  of the particle with respect to some fixed point 
     *  @param state    (input) the track state 
     *  @param point    (input) the fixed point  
     *  @param impact   (output) the evaluated impact parameter vector 
     */  
    inline void i_distance 
    ( const LHCb::State&      state    , 
      const Gaudi::XYZPoint&  point    , 
      Gaudi::XYZVector&       impact   ) const ;
    // =========================================================================
    /** find the points of the closest approach for two tracks 
     *  @param state1    (input) the first particle 
     *  @param state2    (input) the second particle 
     *  @param point1    (output) the point on the first trajectory 
     *  @param point2    (output) the point on the second trajectory
     */
    inline void i_distance 
    ( const LHCb::State&      state1    ,  // the first track
      const LHCb::State&      state2    ,  // the second track
      Gaudi::XYZPoint&        point1    ,  // the point on the first trajectory 
      Gaudi::XYZPoint&        point2    ) const ;  
    // ========================================================================
  protected:
    // ========================================================================
    /** check the validity of the particle
     *  @param p pointer to the particle 
     *  @return StatusCode 
     */
    inline StatusCode check ( const LHCb::Particle*   v ) const ;
    /** check the validity of the vertex 
     *  @param v pointer to the vertex
     *  @return StatusCode 
     */
    inline StatusCode check ( const LHCb::VertexBase* v ) const ;
    /** check the validity of the particles 
     *  @param p1 pointer to the first particle 
     *  @param p2 pointer to the second particle 
     *  @return StatusCode 
     */   
    inline StatusCode check 
    ( const LHCb::Particle*   p1 , 
      const LHCb::Particle*   p2 ) const ;
    /** check the validity of the particle and the vertex 
     *  @param p pointer to the particle 
     *  @param v pointer to the vertex
     *  @return StatusCode 
     */   
    inline StatusCode check 
    ( const LHCb::Particle*   p ,
      const LHCb::VertexBase* v ) const ;
    /** check the validity of the vertices 
     *  @param v1 pointer to the first vertex
     *  @param v2 pointer to the second vertex
     *  @return StatusCode 
     */   
    inline StatusCode check 
    ( const LHCb::VertexBase* v1 ,
      const LHCb::VertexBase* v2 ) const ;
    /** check the validity of the track 
     *  @param t pointer to the track 
     *  @return StatusCode 
     */
    inline StatusCode check 
    ( const LHCb::Track*   t ) const ;
    /** check the validity of the tracks 
     *  @param t1 pointer to the first  track 
     *  @param t2 pointer to the second track 
     *  @return StatusCode 
     */
    inline StatusCode check 
    ( const LHCb::Track*   t1 , 
      const LHCb::Track*   t2 ) const ;
    /** check the validity of track & vertex 
     *  @param t pointer to the track 
     *  @param v pointer to the vertex 
     *  @return StatusCode 
     */
    inline StatusCode check 
    ( const LHCb::Track*      t , 
      const LHCb::VertexBase* v ) const ;
    // ========================================================================
  protected: 
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
      return transporter() -> transport ( particle , newZ , transported ) ;
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
      return transporter() -> 
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
  protected:
    // ========================================================================
    /// get particle transporter 
    inline IParticleTransporter* transporter() const 
    {
      if ( 0 == m_transporter ) 
      { m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ; }
      return m_transporter ;
    }
    // ========================================================================
    /// get state provider 
    inline ITrackStateProvider* stateProvider() const 
    {
      if ( 0 == m_stateProvider ) 
      { m_stateProvider = tool<ITrackStateProvider> ( m_stateProviderName , this ) ; }
      return m_stateProvider ;
    }
    // ========================================================================
    /// get track extrapolator 
    inline ITrackExtrapolator* extrapolator() const 
    {
      if ( 0 == m_extrapolator ) 
      { m_extrapolator = tool<ITrackExtrapolator> ( m_extrapolatorName , this ) ; }
      return m_extrapolator ;
    }
    // ========================================================================
  protected: 
    // ========================================================================
    /// get the state from the track 
    inline const LHCb::State& state ( const LHCb::Track& t ) const 
    {
      const LHCb::State* s = t.stateAt( LHCb::State::ClosestToBeam ) ;
      if ( 0 != s ) { return *s ; }
      return t.firstState();
    }
    // ========================================================================
    /// get the state from the track 
    inline const LHCb::State& state ( const LHCb::Track& t , 
                                      const double       z ) const 
    { return t.closestState ( z ) ; }
    // ========================================================================
  protected: 
    // ========================================================================
    /** Standard constructor
     *  @param type tool type(?)
     *  @param name tool instance name 
     *  @param parent the pointer to the parent
     */
    DistanceCalculatorBase
    ( const std::string& type   ,    // tool type (?)
      const std::string& name   ,    // tool instance name 
      const IInterface*  parent ) ;  // the parent 
    // virtual and protected desctrustor 
    virtual ~DistanceCalculatorBase () ; // Destructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    DistanceCalculatorBase () ; // no default constructor 
    /// the copy    constructor is disabled 
    DistanceCalculatorBase ( const DistanceCalculatorBase&) ; // no copy 
    /// the assignement operator is disabled 
    DistanceCalculatorBase& operator=( const DistanceCalculatorBase&) ; 
    // ========================================================================
  private:
    // ========================================================================
    /// The name of particle transporter tool 
    std::string  m_transporterName ; // The name of particle transpoter tool    
    /// The particle transporter itself 
    mutable IParticleTransporter* m_transporter ; // The transporter itself
    // =======================================================================
    /// The name of track state provider tool 
    std::string  m_stateProviderName ; // The name od state provider 
    /// The state provider itself  
    mutable ITrackStateProvider* m_stateProvider ; // The state provider itself  
    // =======================================================================
    /// The name of track extrapolator tool  
    std::string  m_extrapolatorName ; // The name of track extrapolator
    /// The state provider itself  
    mutable ITrackExtrapolator* m_extrapolator ; // The extrapolator itself 
    // =======================================================================
  }; 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// check the validity of the particle
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check 
( const LHCb::Particle*    p ) const 
{
  if ( 0 == p ) 
  { return _Error ( "LHCb::Particle* points to NULL", InvalidParticle ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// check the validity of the vertex 
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check 
( const LHCb::VertexBase* v ) const 
{
  if ( 0 == v ) 
  { return _Error ( "LHCb::VertexBase* points to NULL", InvalidVertex ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// check the validity of the particles 
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check 
( const LHCb::Particle*   p1 , 
  const LHCb::Particle*   p2 ) const 
{
  if ( 0 == p1 || 0 == p2  ) 
  { return _Error ( "LHCb::Particle* points to NULL", InvalidParticle ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// check the validity of the particle and the vertex 
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check 
( const LHCb::Particle*   p ,
  const LHCb::VertexBase* v ) const 
{
  if ( 0 == p ) 
  { return _Error ( "LHCb::Particle*   points to NULL" , InvalidParticle ) ; }
  if ( 0 == v ) 
  { return _Error ( "LHCb::VertexBase* points to NULL" , InvalidVertex   ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// check the validity of the vertices 
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check 
( const LHCb::VertexBase* v1 , 
  const LHCb::VertexBase* v2 ) const 
{
  if ( 0 == v1 || 0 == v2 ) 
  { return _Error ( "LHCb::VertexBase* points to NULL", InvalidVertex ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
//  check the validity of the track 
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check 
( const LHCb::Track* t ) const 
{
  if ( 0 == t ) { return _Error ( "LHCb::Track* points to NULL", InvalidTrack ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
//  check the validity of the tracks 
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check
( const LHCb::Track* t1 , 
  const LHCb::Track* t2 ) const 
{
  if ( 0 == t1 || 0 == t2 ) 
  { return _Error ( "LHCb::Track* points to NULL", InvalidTrack ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
//  check the validity of track & vertex 
// ============================================================================
inline StatusCode LoKi::DistanceCalculatorBase::check
( const LHCb::Track*      t , 
  const LHCb::VertexBase* v ) const 
{
  if ( 0 == t ) 
  { return _Error ( "LHCb::Track*      points to NULL" , InvalidTrack     ) ; }
  if ( 0 == v ) 
  { return _Error ( "LHCb::VertexBase* points to NULL" , InvalidVertex   ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// construct the line trajector from the particle  
// ============================================================================
inline LoKi::DistanceCalculatorBase::Line_
LoKi::DistanceCalculatorBase::line ( const LHCb::Particle& p ) const 
{ return Line_( p.referencePoint() , p.momentum().Vect() ) ; }
// ============================================================================
// construct the line trajector from the tarck state 
// ============================================================================
inline LoKi::DistanceCalculatorBase::Line_
LoKi::DistanceCalculatorBase::line ( const LHCb::State&    s ) const 
{ return Line_
    ( Gaudi::XYZPoint  ( s.x () , s.y () , s.z() ) , 
      Gaudi::XYZVector ( s.tx() , s.ty() , 1.0   ) ) ; }
// ============================================================================
/*  find the impact parameter ("distance") vector 
 *  of the particle with respect to some vertex 
 */  
// ============================================================================
inline void 
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::Particle&   particle , 
  const LHCb::VertexBase& vertex   , 
  Gaudi::XYZVector&       impact   ) const 
{ 
  i_distance ( particle , vertex.position() , impact ) ; 
}
// ============================================================================
/*  find the impact parameter ("distance") vector 
 *  of the particle with respect to some fixed point 
 */  
// ============================================================================
inline void 
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::Particle&   particle , 
  const Gaudi::XYZPoint&  point    , 
  Gaudi::XYZVector&       impact   ) const 
{
  // (re)use the nice functions by Matt&Juan
  impact = Gaudi::Math::closestPoint ( point , line ( particle ) ) - point ;
}
// ============================================================================
// find the points of the closest approach for two  particles
// ============================================================================
inline void LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::Particle&   particle1 ,  // the first particle 
  const LHCb::Particle&   particle2 ,  // the second particle 
  Gaudi::XYZPoint&        point1    ,  // the point on the first trajectory 
  Gaudi::XYZPoint&        point2    ) const 
{
  double mu1 = 0 ;
  double mu2 = 0 ;
  const  Line_ line1 ( line ( particle1 ) ) ;
  const  Line_ line2 ( line ( particle2 ) ) ;
  
  // (re)use the nice functions by Matt&Juan
  Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
  
  point1 = line1 ( mu1 ) ; // the point on the first tarjectory
  point2 = line2 ( mu2 ) ; // the point on the second trajectory  
}
// ============================================================================
//  evaluate the  distance (and chi^2) between two vertices 
// ============================================================================
inline 
StatusCode 
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::VertexBase& vx1  , 
  const LHCb::VertexBase& vx2  , 
  double&                 dist , 
  double*                 chi2 ) const 
{
  //
  if ( &vx1 == &vx2 ) 
  {
    dist = 0 ;
    if ( 0 != chi2 ) { *chi2 = 0 ; }
    return _Warning("distance(v,v): the same vertex",StatusCode::SUCCESS ) ;  
    
  }
  // calculate the distance 
  const Gaudi::XYZVector delta ( vx1.position() - vx2.position() ) ;
  dist = delta.R() ;
  if ( 0 == chi2 ) { return StatusCode::SUCCESS ; } // RETURN
  // evaluate chi2: 
  Gaudi::SymMatrix3x3 cov ( vx1.covMatrix() + vx2.covMatrix() ) ;
  if ( !cov.Invert() ) 
  { return _Error ( "Error in matrix inversion" , ErrorInMatrixInversion ); }
  // evaluate the chi2 
  *chi2 = Gaudi::Math::Similarity ( delta , cov ) ; 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
//  evaluate the  distance (and chi^2) between 
// ============================================================================
inline 
StatusCode 
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::VertexBase& v    , 
  const Gaudi::XYZPoint&  p    , 
  double&                 dist ,
  double*                 chi2 ) const 
{
  // calculate the distance 
  const Gaudi::XYZVector delta ( v.position() - p ) ;
  dist = delta.R() ;
  if ( 0 == chi2 ) { return StatusCode::SUCCESS ; } // RETURN
  // evaluate chi2: 
  Gaudi::SymMatrix3x3 cov ( v.covMatrix() ) ;
  if ( !cov.Invert() ) 
  { return _Error ( "Error in matrix inversion" , ErrorInMatrixInversion ) ; }  
  // evaluate the chi2 
  *chi2 = Gaudi::Math::Similarity ( delta , cov ) ; 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// evalute the "projected" distance 
// ============================================================================
inline 
double 
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::VertexBase& primary  , 
  const LHCb::Particle&   particle , 
  const LHCb::VertexBase& decay    ) const 
{
  // decay position
  const Gaudi::XYZPoint&  vd = decay   . position () ;
  // origin position 
  const Gaudi::XYZPoint&  vp = primary . position () ;
  // the unit vector along the momentum 
  const Gaudi::XYZVector   p  = particle.momentum().Vect().Unit() ;
  //
  return ( vd  - vp ).Dot ( p ) ;
}
// ============================================================================
// track-related distances 
// ============================================================================
/* find the impact parameter ("distance") vector 
 *  of the particle with respect to some vertex 
 *  @param state    (input) the track state 
 *  @param vertex   (input) the vertex 
 *  @param impact   (output) the evaluated impact parameter vector 
 */  
// ============================================================================
inline void
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::State&      state    , 
  const LHCb::VertexBase& vertex   , 
  Gaudi::XYZVector&       impact   ) const 
{
  return i_distance ( state , vertex.position () , impact ) ;
}
// ============================================================================
/* find the impact parameter ("distance") vector 
 *  of the particle with respect to some fixed point 
 *  @param state    (input) the track state 
 *  @param point    (input) the fixed point  
 *  @param impact   (output) the evaluated impact parameter vector 
 */  
// ============================================================================
inline void
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::State&      state    , 
  const Gaudi::XYZPoint&  point    , 
  Gaudi::XYZVector&       impact   ) const 
{
  // (re)use the nice functions by Matt&Juan
  impact = Gaudi::Math::closestPoint ( point , line ( state ) ) - point ;
}
// ============================================================================
/*  find the points of the closest approach for two tracks 
 *  @param state1    (input) the first particle 
 *  @param state2    (input) the second particle 
 *  @param point1    (output) the point on the first trajectory 
 *  @param point2    (output) the point on the second trajectory
 */
// ============================================================================
inline void
LoKi::DistanceCalculatorBase::i_distance 
( const LHCb::State&      state1    ,  // the first track
  const LHCb::State&      state2    ,  // the second track
  Gaudi::XYZPoint&        point1    ,  // the point on the first trajectory 
  Gaudi::XYZPoint&        point2    ) const
{
  double mu1 = 0 ;
  double mu2 = 0 ;
  //
  const  Line_ line1 ( line ( state1 ) ) ;
  const  Line_ line2 ( line ( state2 ) ) ;
  
  // (re)use the nice functions by Matt&Juan
  Gaudi::Math::closestPointParams ( line1 , line2 , mu1 , mu2 ) ;
  
  point1 = line1 ( mu1 ) ; // the point on the first tarjectory
  point2 = line2 ( mu2 ) ; // the point on the second trajectory  
}  
// ============================================================================
// The END 
// ============================================================================
#endif // LOKIFITTERS_DISTANCECALCULATORBASE_H
// ============================================================================
