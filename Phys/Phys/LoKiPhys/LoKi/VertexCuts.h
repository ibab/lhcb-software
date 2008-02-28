// $Id: VertexCuts.h,v 1.9 2008-02-28 13:59:30 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VERTEXCUTS_H 
#define LOKI_VERTEXCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/CoreCuts.h"
#include "LoKi/MoreFunctions.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/Vertices.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-16
 */
// ============================================================================
namespace LoKi 
{
  namespace Cuts 
  {   
    // ========================================================================
    /** @var VALL
     *  the trivial functor which always evaluates to "true"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Vertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::VertexBase*,bool>      VALL ( true ) ;
    // ========================================================================
    /** @var VTRUE
     *  the trivial functor which always evaluates to "true"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::VALL
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::VertexBase*,bool>      VTRUE = VALL ;
    // ========================================================================
    /** @var VNONE
     *  the trivial functor which always evaluates to "false"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Vertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::VertexBase*,bool>      VNONE ( false ) ;
    // ========================================================================
    /** @var VFALSE
     *  the trivial functor which always evaluates to "false"
     *  
     *  @see LoKi::BooleanConstant
     *  @see LHCb::Vertex
     *  @see LHCb::Cuts::VNONE 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::VertexBase*,bool>    VFALSE = VNONE ;
    // ========================================================================
    /** @var VONE
     *  the trivial functor which always evaluates to 1
     *  
     *  @see LoKi::Constant
     *  @see LHCb::Vertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::VertexBase*,double>         VONE ( 1  ) ;
    // ========================================================================
    /** @var VZERO
     *  the trivial functor which always evaluates to 0
     *  
     *  @see LoKi::Constant
     *  @see LHCb::Vertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Constant<const LHCb::VertexBase*,double>         VZERO ( 0 ) ;
    // ========================================================================
    /** @var VVALID
     *  the trivial functor which evaluayed the "validity" of the argument
     *  
     *  @see LoKi::Valid
     *  @see LHCb::Vertex
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    const LoKi::Valid<const LHCb::VertexBase*>                     VVALID ;
    // ========================================================================
    /** @var PRIMARY
     *     
     *  trivial predicate whcih evaluates to truw for primary vertices 
     *  it relies on th emethod LHCb::Vertex::isPrimary
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const bool primary = PRIMARY( v ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Vertices::IsPrimary
     *  @see LHCb::Vertex 
     *  @see LHCb::PrimVertex 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::IsPrimary                             PRIMARY ;
    // ========================================================================
    /** @var ISPRIMARY
     *     
     *  trivial predicate whcih evaluates to truw for primary vertices 
     *  it relies on th emethod LHCb::Vertex::isPrimary
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const bool primary = ISPRIMARY( v ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Vertices::IsPrimary
     *  @see LoKi::Cuts::PRIMARY
     *  @see LHCb::Vertex 
     *  @see LHCb::PrimVertex 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::IsPrimary                 ISPRIMARY = PRIMARY ;
    // ========================================================================
    /** @var TECHINIQUE
     *      
     *  evaluator of the "technique" used for the vertex 
     *
     *  It relies on the method LHCb::Vertex::technique
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const bool massFit = LHCb::Vertex::MassConstrained == TECHINIQUE( v ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Vertices::Technique
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */ 
    const LoKi::Vertices::Technique                       TECHNIQUE ;
    // ========================================================================
    /** @var VTECHINIQUE
     *      
     *  evaluator of the "technique" used for the vertex 
     *
     *  It relies on the method LHCb::Vertex::technique
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const bool massFit = LHCb::Vertex::MassConstrained == VTECHINIQUE( v ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Cuts::TECHNIQUE
     *  @see LoKi::Vertices::Technique
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */ 
    const LoKi::Vertices::Technique         VTECHNIQUE = TECHNIQUE ;
    // ========================================================================
    /** @var VCHI2
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::Vertex::chi2
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const double chi2 = VCHI2 ( v ) ;
     *
     *  @endcode
     * 
     *  @see LoKi::Vertices::VertexChi2
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */  
    const LoKi::Vertices::VertexChi2                            VCHI2 ;
    // ========================================================================
    /** @var VDOF
     *  evaluator of the number of degrees of freedom for the vertex 
     *  
     *  It relies on the method LHCb::Vertex::nDoF
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const double chi2 = VCHI2 ( v ) / VDOF( v ) ;
     *
     *  @endcode
     * 
     *  @see LoKi::Cuts::VCHI2
     *  @see LoKi::Vertices::VertexDoF
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::VertexDoF                             VDOF ;
    // ========================================================================
    /** @var VXNDOF
     *  evaluator of the number of degrees of freedom for the vertex 
     *  
     *  It relies on the method LHCb::Vertex::nDoF
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const double chi2 = VCHI2 ( v ) / VXNDOF( v ) ;
     *
     *  @endcode
     * 
     *  @see LoKi::Cuts::VCHI2
     *  @see LoKi::Cuts::VDOF
     *  @see LoKi::Vertices::VertexDoF
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::VertexDoF                    VXNDOF = VDOF ;
    // ========================================================================
    /** @typedef VPCHI2N 
     *  The evalutor of \f$\chi^2\f$-probability, tajing into account 
     *  the number of degrees of freedom.
     *
     *  @code
     *
     *    // get the vertex
     *    const LHCb:VertexBase* vertex = ... ;
     *   
     *    // create the fuctor:
     *    const int dof = ... ; /
     *    VPCHI2N fun ( dof ) ;
     *
     *    // use the functor
     *    const double probability = fun ( vertex ) ;
     *
     *  @endcode 
     *
     *  The GSL routine <b>gsl_cdf_chisq_Q</b> is used for evaluation
     *
     *  @see LoKi::Vertics::Chi2Prob 
     *  @see LoKi::Cuts::VPCHI2
     *  @see LHCb::VertexBase::chi2 
     *  @see LHCb::VertexBase::nDoF 
     *  @see LHCb::VertexBase
     *    
     *  @attention: The "unphysical" values mean: 
     *  <b>"get number of DoFs from the vertex itself"</b>
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-02-28
     */
    typedef LoKi::Vertices::Chi2Prob                   VPCHI2N ;
    // ========================================================================
    /** @var VPCHI2
     *  The evalutor of \f$\chi^2\f$-probability, taking into account 
     *  the number of degrees of freedom.
     *  DoFs number is extarcted from the vertex itself
     *
     *  @code
     *
     *    // get the vertex
     *    const LHCb:VertexBase* vertex = ... ;
     *   
     *    // use the functor
     *    const double probability = VPCHI2 ( vertex ) ;
     *
     *  @endcode 
     *
     *  The GSL routine <b>gsl_cdf_chisq_Q</b> is used for evaluation
     *
     *  @see LoKi::Vertics::Chi2Prob 
     *  @see LoKi::Cuts::VPCHI2N
     *  @see LHCb::VertexBase::chi2 
     *  @see LHCb::VertexBase::nDoF 
     *  @see LHCb::VertexBase
     *    
     *  @attention: The "unphysical" values mean: 
     *  <b>"get number of DoFs from the vertex itself"</b>
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-02-28
     */
    const LoKi::Vertices::Chi2Prob                   VPCHI2 ( -1) ;
    // ========================================================================
    /** @var VX 
     *  the trivial evaluator of X-position of the vertex 
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const double x = VX( v ) 
     *
     *  @endcode
     * 
     *  @see LoKi::Vertices::PositionX
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::PositionX                      VX ;
    // ========================================================================
    /** @var VY 
     *  the trivial evaluator of Y-position of the vertex 
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const double y = VY( v ) 
     *
     *  @endcode
     * 
     *  @see LoKi::Vertices::PositionY
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::PositionY                      VY ;
    // ========================================================================
    /** @var VZ 
     *  the trivial evaluator of Z-position of the vertex 
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = .. ;
     *  const double z = VZ( v ) 
     *
     *  @endcode
     * 
     *  @see LoKi::Vertices::PositionZ
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::PositionZ                      VZ ;
    // ========================================================================
    /** @typedef VHASINFO
     *  Trivial predicate which evaluates LHCb::Vertex::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Vertex::hasInfo
     *
     *  @code 
     * 
     *  const LHCb::VertexBase* v = ... ;
     *  VCut has14 = VHASINFO(15 ) ;
     * 
     *  const bool good = has15( v ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Vertex
     *  @see LoKi::Vertices::HasInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef LoKi::Vertices::HasInfo                         VHASINFO ;
    // ========================================================================
    /** @typedef HASVINFO
     *  Trivial predicate which evaluates LHCb::Vertex::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Vertex::hasInfo
     *
     *  @code 
     * 
     *  const LHCb::VertexBase* v = ... ;
     *  VCut has14 = HASVINFO(15 ) ;
     * 
     *  const bool good = has15( v ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Vertex
     *  @see LoKi::Vertices::HasInfo
     *  @see LoKi::Cuts VHASINFO
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef VHASINFO                                         HASVINFO ;
    // ========================================================================
    /** @typedef VINFO
     *  Trivial function which evaluates LHCb::Vertex::info
     *  
     *  It relies on the method LHCb::Vertex::info
     *
     *  @code 
     * 
     *  const LHCb::VertexBase* v = ... ;
     *
     *  const int index = ... ;
     *
     *  VFun info = VINFO( index , -1000 ) ;
     * 
     *  const double result = info( v ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Vertex
     *  @see LoKi::Vertices::Info
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef LoKi::Vertices::Info                                  VINFO ;
    // ========================================================================
    /** @var NTRACKS 
     *  The trivial function which evalautes to 
     *  number of outgoing particles
     *  It relies on the method LHCb::PrimVertex::tracks
     *  
     *  @code 
     * 
     *  const LHCb::VertexBase* v = ... ;
     *
     *  const bool good  = NTRACKS( v ) > 5 ; 
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::NumberOfTracks
     *  @see LHCb::PrimVertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::NumberOfTracks                        NTRACKS ;
    // ========================================================================
    /** @var NPRONGS
     *  The trivial function which evalautes to 
     *  number of outgoing particles 
     *  It relies on the method LHCb::Vertex::outgoingParticles
     *  
     *  @code 
     * 
     *  const LHCb::VertexBase* v = ... ;
     *
     *  const bool good  = NPRONGS( v ) == 3 ; 
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::NumberOfOutgoing
     *  @see LHCb::PrimVertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    const LoKi::Vertices::NumberOfOutgoing                        NPRONGS ;
    // ========================================================================
    /** @typedef VVDIST
     *  Evaluator of the distance between the vertex and another
     *  vertex or point
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = ... ;
     * 
     *  // some primary vertex 
     *  const LHCb::VertexBase*  pv  = ... ;
     *  // some point 
     *  const LoKi::Point3D& pnt = ... ;   
     *
     *  // create the functions 
     *  VFun d1 = VVDIST ( pv  ) ;
     *  VFun d2 = VVDIST ( pnt ) ;
     * 
     * // use them:
     *  const double dist1 = d1( v ) ;
     *  const double dist2 = d2( v ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Point3D
     *
     *  @see LoKi::Vertices::VertexDistance 
     *  @see LoKi::Cuts::VVD 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    typedef LoKi::Vertices::VertexDistance                            VVDIST ;
    // ========================================================================
    /** @typedef VVD
     *  Evaluator of the distance between the vertex and another
     *  vertex or point
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = ... ;
     * 
     *  // some primary vertex 
     *  const LHCb::VertexBase*  pv  = ... ;
     *  // some point 
     *  const LoKi::Point3D& pnt = ... ;   
     *
     *  // create the functions 
     *  VFun d1 = VVD ( pv  ) ;
     *  VFun d2 = VVD ( pnt ) ;
     * 
     * // use them:
     *  const double dist1 = d1( v ) ;
     *  const double dist2 = d2( v ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Point3D
     *
     *  @see LoKi::Cuts::VVDIST 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    typedef VVDIST                                                      VVD ;
    // ========================================================================
    /** @typedef VVDSIGN
     *  Evaluator of the signed distance between the vertex and another
     *  vertex or point
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = ... ;
     * 
     *  // some primary vertex 
     *  const LHCb::VertexBase*  pv  = ... ;
     *  // some point 
     *  const LoKi::Point3D& pnt = ... ;   
     *
     *  // create the functions 
     *  VFun d1 = VVDSIGN ( pv  ) ;
     *  VFun d2 = VVDSIGN ( pnt ) ;
     * 
     * // use them:
     *  const double dist1 = d1( v ) ;
     *  const double dist2 = d2( v ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Point3D
     *
     *  @see LoKi::Vertices::VertexSignedDistance
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    typedef LoKi::Vertices::VertexSignedDistance                     VVDSIGN ;
    // ========================================================================
    /** @typedef VVDCHI2
     *  evaluator of the distance in chi2 units  
     *  between vertex and the abother vertex or point
     *
     *  @code 
     *  
     *  const LHCb::VertexBase* v = ... ;
     * 
     *  // some primary vertex 
     *  const LHCb::VertexBase*  pv  = ... ;
     *  // some point 
     *  const LoKi::Point3D& pnt = ... ;   
     *
     *  // create the functions 
     *  VFun d1 = VVDCHI2 ( pv  ) ;
     *  VFun d2 = VVDCHI2 ( pnt ) ;
     * 
     * // use them:
     *  const double chi2_1 = d1( v ) ;
     *  const double chi2_2 = d2( v ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Point3D
     *
     *  @see LoKi::Vertices::VertexChi2Distance
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    typedef LoKi::Vertices::VertexChi2Distance                       VVDCHI2 ;
    // ========================================================================
    /** @typedef MINVVD
     *  class for evaluation of minimal GEOMETRY distance 
     *  between the vertex and set of other vertices 
     *
     *  @code 
     *  
     *  // get all primary vertices  
     *  const LHCb::PrimVertices* prims = ... ;
     * 
     *  const LHCb::VertexBase* v = ... ; 
     *
     *  // create the function
     *  VFun dist = MINVVD( prims ) ;
     *
     *  // use the function 
     *  const double distance = dist ( v ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Point3D
     *
     *  @see LoKi::Vertices::MinVertexDistance
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    typedef LoKi::Vertices::MinVertexDistance                         MINVVD ;
    // ========================================================================
    /** @typedef MINVVDCHI2 
     *  class for evaluation of minimal chi2 distance 
     *  between the vertex and set of other vertices 
     *
     *  @code 
     *  
     *  // get all primary vertices  
     *  const LHCb::PrimVertices* prims = ... ;
     * 
     *  const LHCb::VertexBase* v = ... ; 
     *
     *  // create the function
     *  VFun dist = MINVVDCHI2 ( prims ) ;
     *
     *  // use the function 
     *  const double chi2 = dist ( v ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Point3D
     *
     *  @see LoKi::Vertices::MinVertexChi2Distance
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    typedef LoKi::Vertices::MinVertexChi2Distance                 MINVVDCHI2 ;
    // ========================================================================
    /** @typedef VIP 
     *  class for evaluation of Impact Parameter 
     *  of the vertex with respect to the particle 
     *
     *
     *  @see LoKi::Vertices::ImpPar 
     *  @see LoKi::Particles::ImpPar
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edy
     *  @date 2006-11-25
     */
    typedef LoKi::Vertices::ImpPar VIP ;
    // ========================================================================
    /** @typedef VIPCHI2
     *  class for evaluation of Impact Parameter 
     *  of the vertex with respect to the particle 
     *
     *
     *  @see LoKi::Vertices::ImpParChi2 
     *  @see LoKi::Particles::ImpParChi2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edy
     *  @date 2006-11-25
     */
    typedef LoKi::Vertices::ImpParChi2 VIPCHI2 ;
    // ========================================================================
  }  // end of namespace LoKi::Cuts
} // end of the namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTEXCUTS_H
// ============================================================================
