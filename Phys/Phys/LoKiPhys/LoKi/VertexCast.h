// $Id: VertexCast.h,v 1.4 2007-07-23 17:35:49 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VERTEXCAST_H 
#define LOKI_VERTEXCAST_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/VertexBase.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
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
 *  @date 2006-02-16 
 */
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::Vertices
   *  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */  
  namespace Vertices 
  {
    // ========================================================================
    /** helper casting function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-08-15 
     */
    inline LHCb::Vertex* 
    base2vertex 
    ( LHCb::VertexBase* base ) 
    {
      return 0 == base ? (LHCb::Vertex*) 0 : 
        dynamic_cast<LHCb::Vertex*>( base ) ;
    }
    // ========================================================================
    /** helper casting function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-08-15 
     */
    inline const LHCb::Vertex* 
    base2vertex 
    ( const LHCb::VertexBase* base ) 
    {
      return 0 == base ? (const LHCb::Vertex*) 0 : 
        dynamic_cast<const LHCb::Vertex*>( base ) ;
    }
    // ========================================================================
    /** helper casting function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-08-15 
     */
    inline LHCb::RecVertex* 
    base2rec
    ( LHCb::VertexBase* base ) 
    {
      return 0 == base ? (LHCb::RecVertex*) 0 : 
        dynamic_cast<LHCb::RecVertex*>( base ) ;
    }
    // ========================================================================
    /** helper casting function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-08-15 
     */
    inline const LHCb::RecVertex* 
    base2rec 
    ( const LHCb::VertexBase* base ) 
    {
      return 0 == base ? (LHCb::RecVertex*) 0 : 
        dynamic_cast<const LHCb::RecVertex*>( base ) ;
    }
    // ========================================================================
  } // end of namespace Vertices
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTEXCAST_H
// ============================================================================
