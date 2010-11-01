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
    static const LHCb::Vertex*                   selectMin 
    ( const LHCb::Vertex::ConstVector&           vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================    
    static const LHCb::Vertex*                   selectMin 
    ( const LHCb::Vertex::Container*             vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMin 
    ( const LHCb::VertexBase::ConstVector&       vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMin 
    ( const LHCb::VertexBase::Container*         vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMin 
    ( const LoKi::Types::VRange&                 vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::RecVertex*                selectMin 
    ( const LHCb::RecVertex::ConstVector&        vertices , 
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
    static const LHCb::Vertex*                   selectMax 
    ( const LHCb::Vertex::ConstVector&           vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================    
    static const LHCb::Vertex*                   selectMax 
    ( const LHCb::Vertex::Container*             vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMax 
    ( const LHCb::VertexBase::ConstVector&       vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMax 
    ( const LHCb::VertexBase::Container*         vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMax 
    ( const LoKi::Types::VRange&                 vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::RecVertex*                selectMax 
    ( const LHCb::RecVertex::ConstVector&        vertices , 
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
