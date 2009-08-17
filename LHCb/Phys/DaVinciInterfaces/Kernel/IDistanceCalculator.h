// $Id: IDistanceCalculator.h,v 1.3 2009-08-17 08:04:22 jpalac Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_IDISTANCECALCULATOR_H 
#define DAVINCIKERNEL_IDISTANCECALCULATOR_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
// ============================================================================
// Forward declarations
// ============================================================================
namespace LHCb 
{
  class Particle   ;
  class VertexBase ;
}
// =============================================================================
/** @class IDistanceCalculator Kernel/IDistanceCalculator.h
 *
 *  The abstract Interface for "Distance Calculator", the tool which 
 *  is able to perform calculation of the various "distances"
 *
 *  Currenly it allows to evaluate "distances" for follwowing pairs of objects:
 *     - the particle and the vertex      ("impact parameter")
 *     - the particle and the fixed point ("impact parameter")
 *     - the particle and the particle    ("the distance of closest approach")
 *     - the vertex and the vertex        ("vertex separation")
 *     - the vertex and the fixed point   ("vertex separation"
 *     - the "path-distance"      form the production vertex to decay vertex 
 *     - the "projected-distance" form the production vertex to decay vertex 
 *  
 *  @author Vanya BELYAEV Ivan.Belyave@nikhef.nl
 *  @date   2008-03-05
 */
class GAUDI_API IDistanceCalculator : virtual public IAlgTool 
{
public:
  // ==========================================================================
  /// interface machinery
  DeclareInterfaceID(IDistanceCalculator, 2, 0);
  // ==========================================================================
public:
  // ==========================================================================
  /** @defgroup ParticleVertex Methods to evaluate distances between "particle" and "vertex"
   *
   *  @{
   */
  // ==========================================================================
  /** The method for the evaluation of the impact parameter ("distance")
   *  vector of the particle with respect to some vertex. 
   * 
   *  The impact parameter is dedined as the length of the 
   *  vector form the vertex to the nearest point on the particle trajectory:
   *  \f[  \vec{i} = \vec{\mathbf{v}}_{p} - \vec{\mathbf{v}}_{0} - 
   *               \vec{\mathbf{p}}\frac{\left(\vec{\mathbf{v}}_{p}-
   *                \vec{\mathbf{v}}_{0}\right)\vec{\mathbf{p}}}
   *                {\vec{\mathbf{p}}^2}, \f]
   *  where the particle trakjectory is parameterized as
   *  \f$\vec{\mathbf{v}}\left(t\right)=\vec{\mathbf{v}}_{p}+\vec{\mathbf{p}}t\f$, 
   *  and \f$\vec{\mathbf{v}}_{0}\f$ stands for the position of the vertex
   *
   *
   *  @code
   *
   *   // get the tool 
   *   const IGeomDispCalculator* tool = ... ; 
   *
   *   const LHCb::Particle*   particle = ... ;
   *   const LHCb::VertexBase* primary  = ... ;
   *  
   *   double impact = -1 * Gaudi::Units::km ; 
   * 
   *   // use the tool to evaluate the impact parameter vector 
   *   StatusCode sc = tool -> distance ( particle , vertex , impact ) ;
   *
   *  @endcode 
   *
   *  @param particle (input) pointer to the particle 
   *  @param vertex   (input) pointer to the vertex 
   *  @param imppar   (output) the value of impact parameter ("distance") 
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::Particle*   particle ,
    const LHCb::VertexBase* vertex   , 
    double&                 imppar   ) const = 0 ;
  // ==========================================================================
  /** The method for the evaluation of the impact parameter ("distance")
   *  of the particle with respect to some vertex and its \f$\chi^2\f$-significance
   * 
   *  The impact parameter is dedined as the length of the 
   *  vector form the vertex to the nearest point on the particle trajectory:
   *  \f[  \vec{i} = \vec{\mathbf{v}}_{p} - \vec{\mathbf{v}}_{0} - 
   *               \vec{\mathbf{p}}\frac{\left(\vec{\mathbf{v}}_{p}-
   *                \vec{\mathbf{v}}_{0}\right)\vec{\mathbf{p}}}
   *                {\vec{\mathbf{p}}^2}, \f]
   *  where the particle trakjectory is parameterized as
   *  \f$\vec{\mathbf{v}}\left(t\right)=\vec{\mathbf{v}}_{p}+\vec{\mathbf{p}}t\f$, 
   *  and \f$\vec{\mathbf{v}}_{0}\f$ stands for the position of the vertex
   *
   *  @code
   *
   *   // get the tool 
   *   const IGeomDispCalculator* tool = ... ; 
   *
   *   const LHCb::Particle*   particle = ... ;
   *   const LHCb::VertexBase* primary  = ... ;
   *  
   *   double impact = -1 * Gaudi::Units::km ; 
   *   double chi2   = -1000 ;
   * 
   *   // use the tool to evaluate the impact parameter vector 
   *   StatusCode sc = tool -> distance ( particle , vertex , impact , chi2 ) ;
   *  
   *   // evaluate "pseudo-significance":
   *   const double significance = sqrt ( chi2 ) ;  // ATTENTION!!!
   *
   *  @endcode 
   *
   *  @param particle (input)  pointer to the particle 
   *  @param vertex   (input)  pointer to the vertex 
   *  @param imppar   (output) the value of impact parameter ("distance") 
   *  @param chi2     (output) the chi2-value for the impact parameter 
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::Particle*   particle ,
    const LHCb::VertexBase* vertex   , 
    double&                 imppar   , 
    double&                 chi2     ) const = 0 ;
  // ==========================================================================
  /// @}
  // ==========================================================================
public:
  // ==========================================================================
  /** @defgroup ParticlePoint Methods to evaluate distances between "particle" and "fixed point"
   *
   *  @{
   */
  // ==========================================================================
  /** the basic method for the evaluation of the impact parameter ("distance")
   *  vector of the particle with respect to some vertex. 
   * 
   *  The impact parameter is defined as the length of the vector form the fixed point 
   *  to the nearest point on the particle trajectory:
   *
   *  \f[  \vec{i} = \vec{\mathbf{v}}_{p} - \vec{\mathbf{v}}_{0} - 
   *               \vec{\mathbf{p}}\frac{\left(\vec{\mathbf{v}}_{p}-
   *                \vec{\mathbf{v}}_{0}\right)\vec{\mathbf{p}}}
   *                {\vec{\mathbf{p}}^2}, \f]
   *  where the particle trakjectory is parameterized as
   *  \f$\vec{\mathbf{v}}\left(t\right)=\vec{\mathbf{v}}_{p}+\vec{\mathbf{p}}t\f$, 
   *  and \f$\vec{\mathbf{v}}_{0}\f$ stands for the position of the fixed point.
   *
   *
   *  @code
   *
   *   // get the tool 
   *   const IGeomDispCalculator* tool = ... ; 
   *
   *   const LHCb::Particle*   particle = ... ;
   *   const Gaudi::XYZPoint&  point    = ... ;
   *  
   *   double impact = -1 * Gaudi::Units::km ; 
   * 
   *   // use the tool to evaluate the impact parameter vector 
   *   StatusCode sc = tool -> distance ( particle , point , impact ) ;
   *
   *  @endcode 
   *
   *  @param particle (input) pointer to the particle 
   *  @param point    (input) the fixed point 
   *  @param imppar   (output) the value of impact parameter ("distance") 
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::Particle*   particle ,
    const Gaudi::XYZPoint&  point    , 
    double&                 imppar   ) const = 0 ;
  // ==========================================================================
  /** the basic method for the evaluation of the impact parameter ("distance")
   *  of the particle with respect to some vertex and its \f$\chi^2\f$-significance
   * 
   *  The impact parameter is defined as the length of the vector form the fixed point 
   *  to the nearest point on the particle trajectory:
   *
   *  \f[  \vec{i} = \vec{\mathbf{v}}_{p} - \vec{\mathbf{v}}_{0} - 
   *               \vec{\mathbf{p}}\frac{\left(\vec{\mathbf{v}}_{p}-
   *                \vec{\mathbf{v}}_{0}\right)\vec{\mathbf{p}}}
   *                {\vec{\mathbf{p}}^2}, \f]
   *  where the particle trakjectory is parameterized as
   *  \f$\vec{\mathbf{v}}\left(t\right)=\vec{\mathbf{v}}_{p}+\vec{\mathbf{p}}t\f$, 
   *  and \f$\vec{\mathbf{v}}_{0}\f$ stands for the position of the fixed point.
   *
   *  @code
   *
   *   // get the tool 
   *   const IGeomDispCalculator* tool = ... ; 
   *
   *   const LHCb::Particle*   particle = ... ;
   *   const Gaudi::XYZPoint&  point    = ... ;
   *  
   *   double impact = -1 * Gaudi::Units::km ; 
   *   double chi2   = -1000 ;
   * 
   *   // use the tool to evaluate the impact parameter vector 
   *   StatusCode sc = tool -> distance ( particle , point , impact , chi2 ) ;
   *  
   *   // evaluate "pseudo-significance":
   *   const double significance = sqrt ( chi2 ) ;  // ATTENTION!!!
   *
   *  @endcode 
   *
   *  @param particle (input)  pointer to the particle 
   *  @param point    (input) the fixed point 
   *  @param imppar   (output) the value of impact parameter ("distance") 
   *  @param chi2     (output) the chi2-value for the impact parameter 
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::Particle*   particle ,
    const Gaudi::XYZPoint&  point    , 
    double&                 imppar   , 
    double&                 chi2     ) const = 0 ;
  // ==========================================================================
  /// @}
  // ==========================================================================
public:
  // ==========================================================================
  /** @defgroup VertexVertex Methods to evaluate distances between tho "vertices"
   *
   *  @{
   */
  // ==========================================================================
  /** The trivial method for evaluation of the distance between two vertices 
   * 
   *  The distance is trivially defined as the length of the vector 
   *  \f$\vec{\mathbf{d}}\f$:
   *
   *  \f[ \vec{\mathbf{d}} = \vec{\mathbf{v}}_2 - \vec{\mathbf{v}}_1,\f] 
   *  where  \f$\vec{\mathbf{v}}_1\f$ and \f$\vec{\mathbf{v}}_1\f$ are positions
   *  of the first and the second vertices 
   *
   *  @code 
   *
   *  // get the tool 
   *  const IGeomDispCalculator* tool = ... ; 
   *
   *  const LHCb::VertexBase* primary   = ... ;
   *  const LHCb::VertexBase* secondary = ... ; 
   *
   *   double dist = -1 * Gaudi::Units::km ; 
   *
   *   // use the tool to evaluate the distance
   *   StatusCode sc = tool -> distance ( primary , secondary , dist ) ;
   *
   *  @endcode
   *
   *  @param v1   (input) the pointr to the first vertex 
   *  @param v2   (input) the pointer to the second vertex 
   *  @param dist (output) the distance between two vertices 
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::VertexBase*  v1   , 
    const LHCb::VertexBase*  v2   , 
    double&                  dist ) const = 0 ;
  // ==========================================================================  
  /** The method for evaluation of the distance between two vertices and the 
   *  corresponding \f$\chi^2\f$ for the separation significance.
   *
   *  The distance is trivially defined as the length of the vector 
   *  \f$\vec{\mathbf{d}}\f$:
   *
   *  \f[ \vec{\mathbf{d}} = \vec{\mathbf{v}}_2 - \vec{\mathbf{v}}_1,\f] 
   *  where  \f$\vec{\mathbf{v}}_1\f$ and \f$\vec{\mathbf{v}}_1\f$ are positions
   *  of the first and the second vertices 
   *
   *  @code 
   *
   *  // get the tool 
   *  const IGeomDispCalculator* tool = ... ; 
   *
   *  const LHCb::VertexBase* primary   = ... ;
   *  const LHCb::VertexBase* secondary = ... ; 
   *
   *   double dist = -1 * Gaudi::Units::km ; 
   *   double chi2 = -1000 ; 
   *
   *   // use the tool to evaluate the distance
   *   StatusCode sc = tool -> distance ( primary , secondary , dist , chi2 ) ;
   *
   *  @endcode
   *
   *  @param v1   (input) the pointr to the first vertex 
   *  @param v2   (input) the pointer to the second vertex 
   *  @param dist (output) the distance between two vertices 
   *  @param chi2 (output) the chi2 of the separation significance
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::VertexBase*  v1   , 
    const LHCb::VertexBase*  v2   , 
    double&                  dist , 
    double&                  chi2 ) const = 0 ;
  // ==========================================================================
  /// @}
  // ==========================================================================
public:
  // ==========================================================================
  /** @defgroup VertexPoint Methods to evaluate distances between a vertex and a fixed point 
   *
   *  @{
   */
  // ==========================================================================
  /** The trivial method for evaluation of the distance between the vertex 
   *  and some "fixed" point
   * 
   *  The distance is trivially defined as the length of the vector 
   *  \f$\vec{\mathbf{d}}\f$:
   *
   *  \f[ \vec{\mathbf{d}} = \vec{\mathbf{v}} - \vec{\mathbf{p}},\f] 
   *  where  \f$\vec{\mathbf{v}}\f$ and \f$\vec{\mathbf{v}}\f$ are positions
   *  of the veretx and the fixed point 
   *
   *  @code 
   *
   *  // get the tool 
   *  const IGeomDispCalculator* tool = ... ; 
   *
   *  const LHCb::VertexBase* secondary = ... ; 
   *  const Gaudi::XYZPoint&  primary   = ... ;
   *
   *   double dist = -1 * Gaudi::Units::km ; 
   *
   *   // use the tool to evaluate the distance
   *   StatusCode sc = tool -> distance ( secondary , primary , dist ) ;
   *
   *  @endcode
   *
   *  @param v   (input) the pointr to the first vertex 
   *  @param p   (input) the fixed point  
   *  @param dist (output) the distance between two vertices 
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::VertexBase*  v    , 
    const Gaudi::XYZPoint&   p    , 
    double&                  dist ) const = 0 ;
  // ==========================================================================  
  /** The method for evaluation of the distance between the vertices 
   *  and soem fixed point and the corresponding \f$\chi^2\f$ for 
   *  the separation significance.
   * 
   *  The distance is trivially defined as the length of the vector 
   *  \f$\vec{\mathbf{d}}\f$:
   *
   *  \f[ \vec{\mathbf{d}} = \vec{\mathbf{v}} - \vec{\mathbf{p}},\f] 
   *  where  \f$\vec{\mathbf{v}}\f$ and \f$\vec{\mathbf{v}}\f$ are positions
   *  of the veretx and the fixed point 
   *
   *  @code 
   *
   *  // get the tool 
   *  const IGeomDispCalculator* tool = ... ; 
   *
   *  const LHCb::VertexBase* secondary = ... ; 
   *  const Gaudi::XYZPoint&  primary   = ... ;
   *
   *   double dist = -1 * Gaudi::Units::km ; 
   *   double chi2 = -1000 ; 
   *
   *   // use the tool to evaluate the distance
   *   StatusCode sc = tool -> distance ( secondary , primary , dist , chi2 ) ;
   *
   *  @endcode
   *
   *  @param v   (input)   the pointer to the first vertex 
   *  @param p   (input)   the fixed point 
   *  @param dist (output) the distance between two vertices 
   *  @param chi2 (output) the chi2 of the separation significance
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::VertexBase*  v    , 
    const Gaudi::XYZPoint&   p    , 
    double&                  dist , 
    double&                  chi2 ) const = 0 ;
  // ==========================================================================  
  /// @}
  // ==========================================================================
public:
  // ==========================================================================
  /** @defgroup ParticleParticle Methods to evaluate distances between two particles 
   *
   *  @{
   */
  // ==========================================================================
  /** The method for evaluation of the scalar distance between two particles, 
   *  aka "distance of the closest approach". 
   *
   *  The distance is defined as the the shortest legth vector between 
   *  the point on the first trajectory and the point on the second 
   *  trajectory
   *  
   *  @code
   * 
   *  // get the tool 
   *  const IGeomDispCalculator* tool = ... ; 
   *
   *  const LHCb::Particle* p1 = ... ;
   *  const LHCb::Particle* p2 = ... ;
   *  
   *  double dist ; 
   *  
   *  // use the tool:
   *  StatusCode sc = tool -> distance ( p1 , p2 , dist ) ;
   * 
   *  @endcode 
   *
   *  @param p1 (input) the pointer to the first particle 
   *  @param p2 (input) the pointer to the second particle 
   *  @param dist (output) the shortest distance between two trajectories  
   *  @return status code 
   */
  virtual StatusCode distance 
  ( const LHCb::Particle* p1   , 
    const LHCb::Particle* p2   , 
    double&               dist ) const = 0 ;
  // ==========================================================================
  /** The method for evaluation of the scalar distance between two particles, 
   *  aka "distance of the closest approach" and also its 
   *   \f$\chi^2\f$ for separation significance 
   *
   *  The distance is defined as the the shortest legth vector between 
   *  the point on the first trajectory and the point on the second 
   *  trajectory
   *  
   *  @code
   * 
   *  // get the tool 
   *  const IGeomDispCalculator* tool = ... ; 
   *
   *  const LHCb::Particle* p1 = ... ;
   *  const LHCb::Particle* p2 = ... ;
   *  
   *  double dist = -1 * Gaudi::Units::km ; 
   *  double chi2 = -1000 ;
   *  
   *  // use the tool:
   *  StatusCode sc = tool -> distance ( p1 , p2 , dist , chi2 ) ;
   * 
   *  @endcode 
   *
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
    double&               chi2 ) const = 0 ;
  // ==========================================================================
  /// @} 
  // ==========================================================================
public:
  // ==========================================================================
  /** @defgroup OtherDistances Methods to evaluate of some other "distances"
   *
   *  @{
   */
  // ==========================================================================
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
   *  Clearly this vector-equation has the solution only in the 
   *  sence of \f$\chi^2\f$ solution, and the parameter \f$s\f$ is 
   *  closely related to the particle proper time \f$c\tau\f$.
   *
   *  @code
   * 
   *  // get the tool 
   *  const IGeomDispCalculator* tool = ... ; 
   *
   *  const LHCb::Particle*   particle = ... ;
   *  const LHCb::VertexBase* primary  = ... ; 
   *  
   *  double dist = -1 * Gaudi::Units::km ; 
   *  double err  = -1 * Gaudi::Units::km ; 
   *  double chi2 = -1000 ;
   *  
   *  // use the tool:
   *  StatusCode sc = tool -> pathDistance 
   *           ( particle , 
   *             primary  , dist , err , chi2 ) ;
   * 
   *  @endcode 
   * 
   *  @param   particle (input) the pointer to the particle 
   *  @param   vertex   (input) the pointer to the production vertex
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
    double&                 chi2     ) const = 0 ;
  // ==========================================================================
  /** Calculate the projected distance 
   *
   *  \f$s=\frac{\left(\vec{\mathbf{v}}\vec{\mathbf{p}}
   *     \right)}{\left|\vec{\mathbf{p}}\right|}\f$,
   *  where vector \f$\vec{\mathbf{v}}\f$ is a vector from 
   *  the primary to the secondary vertex: 
   *    \f$\vec{\mathbf{v}}=\vec{\mathbf{x}}_{d}-\vec{\mathbf{x}}_{pv}\f$,
   *
   * @param[in]  particle the pointer to the particle 
   * @param[in]  primary  the pointer to the production vertex 
   * @param[out] dist     the projected distance
   * @return status code 
   */
  virtual StatusCode projectedDistance   
  ( const LHCb::Particle*   particle , 
    const LHCb::VertexBase* primary  , 
    double&                 dist     ) const = 0 ;
  // ==========================================================================
  /** Calculate the projected distance 
   *
   *  \f$s=\frac{\left(\vec{\mathbf{v}}\vec{\mathbf{p}}
   *     \right)}{\left|\vec{\mathbf{p}}\right|}\f$,
   *  where vector \f$\vec{\mathbf{v}}\f$ is a vector from 
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
    double&                 error    ) const = 0 ;
  // ==========================================================================
  /// @}
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual and protected desctructor 
  virtual ~IDistanceCalculator() ;         // virtual and protected desctrustor 
  // ==========================================================================  
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IDISTANCECALCULATOR_H
// ============================================================================
