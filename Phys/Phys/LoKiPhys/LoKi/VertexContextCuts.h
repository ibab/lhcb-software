// $Id: VertexContextCuts.h,v 1.1 2008-03-30 13:43:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VERTEXCONTEXTCUTS_H 
#define LOKI_VERTEXCONTEXTCUTS_H 1
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysSources.h"
#include "LoKi/Vertices5.h"
// ============================================================================
/** @file
 *  The file with the collection of "context-dependent" functors 
 *x
 *  Essentially all these functord depends on "event-data" and 
 *  get the nesessary "context-dependent" data from Algorithm Context Service
 *
 *  The basic ingredients here:
 *   - LoKi Service 
 *   - Algorithm Context Service
 *   - PhysDesktop
 *   - LoKi::getPhysDesktop
 *   - Gaudi::Utils::getDValgorithm
 *
 *  @attention There are no direct needs to use all these "Context" 
 *             functors inside the native LoKi-based C++ code, 
 *             for all of them, there are more efficient, transparent, 
 *             clear and safe analogues...
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-01-29
 */
// ============================================================================
namespace LoKi 
{
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef VMINVDSOURCE 
     *  The generic "vertex"-functor which evaluates the minimal 
     *  3D-distance between the vertex and the vertices form the list.
     *  The vertices are extracted from the "Source"
     * 
     *  @code 
     *  
     *   const VFun fun = VMINVDSOURCE( VSOURCEDV(PRIMARY) ) ;
     *  
     *   const LHCb::VertexBase* vertex = ... ;
     *  
     *   const double minDistance = fun ( vertex ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::MinVertexDistanceWithSource::Source 
     *  @see LoKi::Cuts::VMINVDDV 
     *  @see LoKi::Cuts::VMINVDTES
     *  @see LoKi::Vertices::MinVertexDistanceDV 
     *  @see LoKi::Vertices::MinVertexDistanceTES
     *  @see LoKi::Vertices::MinVertexDistanceWithSource
     *  @see LoKi::Vertices::MinVertexDistance
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-01-29
     */
    typedef LoKi::Vertices::MinVertexDistanceWithSource         VMINVDSOURCE ;
    // =======================================================================
    /** @typedef VMINVDDV
     *  The "vertex"-functor which evaluates the minimal 
     *  3D-distance between the vertex and the vertices form the list.
     *  The vertices are extracted from desktop 
     * 
     *  @code 
     *  
     *   const VFun fun = VMINVDDV ( PRIMARY ) ;
     *  
     *   const LHCb::VertexBase* vertex = ... ;
     *  
     *   const double minDistance = fun ( vertex ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::MinVertexDistanceDV 
     *  @see LoKi::Vertices::MinVertexDistanceWithSource
     *  @see LoKi::Vertices::MinVertexDistance
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-01-29
     */    
    typedef LoKi::Vertices::MinVertexDistanceDV                     VMINVDDV ;
    // =======================================================================
    /** @typedef VMINVDTES
     *  The "vertex"-functor which evaluates the minimal 
     *  3D-distance between the vertex and the vertices form the list.
     *  The vertices are extracted from TES 
     * 
     *  @code 
     *  
     *   const VFun fun = VMINVDTES ( "some TES location" , PRIMARY ) ;
     *  
     *   const LHCb::VertexBase* vertex = ... ;
     *  
     *   const double minDistance = fun ( vertex ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::MinVertexDistanceTES
     *  @see LoKi::Vertices::MinVertexDistanceWithSource
     *  @see LoKi::Vertices::MinVertexDistance
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-01-29
     */    
    typedef LoKi::Vertices::MinVertexDistanceTES                   VMINVDTES ;
    // =======================================================================
    /** @typedef VMINVDCHI2SOURCE 
     *  The generic "vertex"-functor which evaluates the minimal 
     *  chi2-distance between the vertex and the vertices form the list.
     *  The vertices are extracted from the "Source"
     * 
     *  @code 
     *  
     *   const VFun fun = VMINVDCHI2SOURCE( VSOURCEDV(PRIMARY) ) ;
     *  
     *   const LHCb::VertexBase* vertex = ... ;
     *  
     *   const double minChi2 = fun ( vertex ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::MinVertexChi2DistanceWithSource::Source 
     *  @see LoKi::Cuts::VMINVDCHI2DV 
     *  @see LoKi::Cuts::VMINVDCHI2TES
     *  @see LoKi::Vertices::MinVertexChi2DistanceDV 
     *  @see LoKi::Vertices::MinVertexChi2DistanceTES
     *  @see LoKi::Vertices::MinVertexChi2DistanceWithSource
     *  @see LoKi::Vertices::MinVertexChi2Distance
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-01-29
     */
    typedef LoKi::Vertices::MinVertexChi2DistanceWithSource VMINVDCHI2SOURCE ;
    // =======================================================================
    /** @typedef VMINVDCHI2DV
     *  The "vertex"-functor which evaluates the minimal 
     *  chi2-distance between the vertex and the vertices form the list.
     *  The vertices are extracted from desktop 
     * 
     *  @code 
     *  
     *   const VFun fun = VMINVDCHI2DV ( PRIMARY ) ;
     *  
     *   const LHCb::VertexBase* vertex = ... ;
     *  
     *   const double minChi2 = fun ( vertex ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::MinVertexChi2DistanceDV 
     *  @see LoKi::Vertices::MinVertexChi2DistanceWithSource
     *  @see LoKi::Vertices::MinVertexChi2Distance
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-01-29
     */    
    typedef LoKi::Vertices::MinVertexChi2DistanceDV             VMINVDCHI2DV ;
    // =======================================================================
    /** @typedef VMINVDCHI2TES
     *  The "vertex"-functor which evaluates the minimal 
     *  chi2-distance between the vertex and the vertices form the list.
     *  The vertices are extracted from TES 
     * 
     *  @code 
     *  
     *   const VFun fun = VMINVDCHI2TES ( "some TES location" , PRIMARY ) ;
     *  
     *   const LHCb::VertexBase* vertex = ... ;
     *  
     *   const double minChi2 = fun ( vertex ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Vertices::MinVertexChi2DistanceTES
     *  @see LoKi::Vertices::MinVertexChi2DistanceWithSource
     *  @see LoKi::Vertices::MinVertexChi2Distance
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-01-29
     */    
    typedef LoKi::Vertices::MinVertexChi2DistanceTES          VMINVDCHI2TES ;
    // =======================================================================
  } // end of namespace LoKi::Cuts 
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTEXCONTEXTCUTS_H
// ============================================================================
