// $Id: SelectVertex.h,v 1.1 2007-08-17 15:22:29 ibelyaev Exp $
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
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
// ============================================================================
namespace LoKi 
{
  /** @class LoKi::SelectVertex
   *  collection utilities for selection of primary vertex
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-17
   */
  class SelectVertex
  {
  public:
    // ========================================================================    
    static       LHCb::Vertex*                   selectMin 
    ( const LHCb::Vertex::Vector&                vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
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
    static const LHCb::Vertex*                   selectMin 
    ( const LoKi::Keeper<LHCb::Vertex>&          vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================    
    static const LHCb::Vertex*                   selectMin 
    ( const LoKi::UniqueKeeper<LHCb::Vertex>&    vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================    


    // ========================================================================
    static       LHCb::VertexBase*               selectMin 
    ( const LHCb::VertexBase::Vector&            vertices , 
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
    ( const LoKi::Keeper<LHCb::VertexBase>&      vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMin 
    ( const LoKi::UniqueKeeper<LHCb::VertexBase>&vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ========================================================================
    static const LHCb::VertexBase*               selectMin 
    ( const LoKi::Types::VRange&                 vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 

    
    // ======================================================================== 
    static       LHCb::RecVertex*                selectMin 
    ( const LHCb::RecVertex::Vector&             vertices , 
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
    static const LHCb::RecVertex*                selectMin 
    ( const LoKi::Keeper<LHCb::RecVertex>&       vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::RecVertex*                selectMin 
    ( const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices , 
      const LoKi::Types::VFunc&                  fun      ,
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    

    // ======================================================================== 
    static       LHCb::Vertex*                   selectMax 
    ( const LHCb::Vertex::Vector&                vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
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
    static const LHCb::Vertex*                   selectMax 
    ( const LoKi::Keeper<LHCb::Vertex>&          vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::Vertex*                   selectMax 
    ( const LoKi::UniqueKeeper<LHCb::Vertex>&    vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 

    // ======================================================================== 
    static       LHCb::VertexBase*               selectMax 
    ( const LHCb::VertexBase::Vector&            vertices , 
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
    ( const LoKi::Keeper<LHCb::VertexBase>&      vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::VertexBase*               selectMax 
    ( const LoKi::UniqueKeeper<LHCb::VertexBase>&vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::VertexBase*               selectMax 
    ( const LoKi::Types::VRange&                 vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 


    // ======================================================================== 
    static       LHCb::RecVertex*                selectMax 
    ( const LHCb::RecVertex::Vector&             vertices , 
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
    static const LHCb::RecVertex*                selectMax 
    ( const LoKi::Keeper<LHCb::RecVertex>&       vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
    // ======================================================================== 
    static const LHCb::RecVertex*                selectMax 
    ( const LoKi::UniqueKeeper<LHCb::RecVertex>& vertices , 
      const LoKi::Types::VFunc&                  fun      , 
      const LoKi::Types::VCuts&                  cut      ) ;
  }; 
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTEXSELECT_H
// ============================================================================
