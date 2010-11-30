// $Id$
// ============================================================================
#ifndef LOKI_VERTEXSELECT_H 
#define LOKI_VERTEXSELECT_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
/** @file LoKi/SelectVertex.h
 *
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-08-17
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class LoKi::SelectVertex
   *  collection utilities for selection of primary vertex
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-17
   */
  class GAUDI_API SelectVertex
  {
  public:
    // ========================================================================    
    static const LHCb::VertexBase*               selectMin 
    ( const LoKi::Types::VRange&                 vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::Vertex*                   selectMin 
    ( const LHCb::Vertex::Container*             vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ; 
    // ======================================================================== 
    static const LHCb::RecVertex*                selectMin 
    ( const LHCb::RecVertex::Container*          vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::RecVertex*                selectMin 
    ( const LHCb::RecVertex::Container*          vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
  public:
    // ========================================================================    
    static const LHCb::VertexBase*               selectMax 
    ( const LoKi::Types::VRange&                 vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::Vertex*                   selectMax 
    ( const LHCb::Vertex::Container*             vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::RecVertex*                selectMax
    ( const LHCb::RecVertex::Container*          vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
  }; 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_VERTEXSELECT_H
// ============================================================================
