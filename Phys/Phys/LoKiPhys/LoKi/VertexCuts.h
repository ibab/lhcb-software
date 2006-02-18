// $Id: VertexCuts.h,v 1.2 2006-02-18 18:09:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
//  $Log: not supported by cvs2svn $
//  Revision 1.1.1.1  2006/02/17 19:17:26  ibelyaev
//  New package: "Phys"-part of restructurized LoKi project  
// 
// ============================================================================
#ifndef LOKI_VERTEXCUTS_H 
#define LOKI_VERTEXCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/CoreCuts.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/Vertices.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
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
    const LoKi::BooleanConstant<const LHCb::Vertex*>      VALL ( true ) ;
    // ========================================================================
    
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
    const LoKi::BooleanConstant<const LHCb::Vertex*>      VTRUE = VALL ;
    // ========================================================================
    
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
    const LoKi::BooleanConstant<const LHCb::Vertex*>      VNONE ( false ) ;
    // ========================================================================

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
    const LoKi::BooleanConstant<const LHCb::Vertex*>    VFALSE = VNONE ;
    // ========================================================================
    
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
    const LoKi::Constant<const LHCb::Vertex*>             VONE ( 1  ) ;
    // ========================================================================
    
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
    const LoKi::Constant<const LHCb::Vertex*>             VZERO ( 0 ) ;
    // ========================================================================

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
    const LoKi::Valid<const LHCb::Vertex*>                     VVALID ;
    // ========================================================================
    
    // ========================================================================
    /** @var PRIMARY
     *     
     *  trivial predicate whcih evaluates to truw for primary vertices 
     *  it relies on th emethod LHCb::Vertex::isPrimary
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
     *  const bool primary = PRIMARY( v ) ;
     *
     *  @endcode
     *
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
    
    // ========================================================================
    /** @var ISPRIMARY
     *     
     *  trivial predicate whcih evaluates to truw for primary vertices 
     *  it relies on th emethod LHCb::Vertex::isPrimary
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
     *  const bool primary = ISPRIMARY( v ) ;
     *
     *  @endcode
     *
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
    
    // ========================================================================
    /** @var TECHINIQUE
     *      
     *  evaluator of the "technique" used for the vertex 
     *
     *  It relies on the method LHCb::Vertex::technique
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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

    // ========================================================================
    /** @var VTECHINIQUE
     *      
     *  evaluator of the "technique" used for the vertex 
     *
     *  It relies on the method LHCb::Vertex::technique
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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
    
    // ========================================================================
    /** @var VCHI2
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::Vertex::chi2
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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
    
    // ========================================================================
    /** @var VDOF
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::Vertex::nDoF
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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

    // ========================================================================
    /** @var VXNDOF
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::Vertex::nDoF
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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
    
    // ========================================================================
    /** @var VX 
     *  the trivial evaluator of X-position of the vertex 
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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
    
    // ========================================================================
    /** @var VY 
     *  the trivial evaluator of Y-position of the vertex 
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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
    
    // ========================================================================
    /** @var VZ 
     *  the trivial evaluator of Z-position of the vertex 
     *
     *  @code 
     *  
     *  const LHCb::Vertex* v = .. ;
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
    
    // ========================================================================
    /** Trivial predicate which evaluates LHCb::Vertex::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Vertex::hasInfo
     *
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
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
    
    // ========================================================================
    /** Trivial predicate which evaluates LHCb::Vertex::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Vertex::hasInfo
     *
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
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
   
    // ========================================================================
    /** Trivial function which evaluates LHCb::Vertex::info
     *  
     *  It relies on the method LHCb::Vertex::info
     *
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
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

    // ========================================================================
    /** @var NTRACKS 
     *  The trivial function which evalautes to 
     *  number of outgoing particles
     *  It relies on the method LHCb::PrimVertex::tracks
     *  
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
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

    // ========================================================================
    /** @var NPRONGS
     *  The trivial function which evalautes to 
     *  number of outgoing particles 
     *  It relies on the method LHCb::Vertex::outgoingParticles
     *  
     *  @code 
     * 
     *  const LHCb::Vertex* v = ... ;
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


    
  } ; // end of namespace LoKi::Cuts
}; // end of the namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTEXCUTS_H
// ============================================================================
