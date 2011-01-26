// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES10_H 
#define LOKI_PARTICLES10_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/Particles9.h"
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
// Event 
// ============================================================================
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
 *  @date 2006-02-22 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class HasTracksFromPV 
     *
     *  The simple predicate which evaluates to 'true' for 
     *  the particle which *DIRECTLY* participates in to 
     *  the primary vertex reconstruction
     *  
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LHCb::RecVertex
     *  @see LoKi::Particles::HasTracks
     *
     *  @see LoKi::Cuts::PVTRACK
     *  @see LoKi::Cuts::TRACKFROMPV
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22
     */
    class GAUDI_API HasTracksFromPV : public LoKi::Particles::HasTracks 
    {
    public:
      // ======================================================================
      /// constructor from one primary vertex 
      HasTracksFromPV ( const LHCb::RecVertex*                pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksFromPV ( const LHCb::RecVertex::ConstVector&   pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LHCb::RecVertex::Container*     pvs ) ;
      /// constructor from one vertex 
      HasTracksFromPV ( const LHCb::VertexBase*               pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksFromPV ( const LHCb::VertexBase::ConstVector&  pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LoKi::PhysTypes::VRange&        pvs ) ;
      // ======================================================================
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksFromPV 
      ( VERTEX first , 
        VERTEX last  ) 
        : LoKi::Particles::HasTracks ( first , last ) 
      {}
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksFromPV 
      ( const LoKi::Keeper<VERTEX>& keeper ) 
        : LoKi::Particles::HasTracks ( keeper  ) 
      {}
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksFromPV 
      ( const LoKi::UniqueKeeper<VERTEX>& keeper ) 
        : LoKi::Particles::HasTracks ( keeper  ) 
      {}
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~HasTracksFromPV() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracksFromPV* clone() const 
      { return new HasTracksFromPV(*this); }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // The default costructor is disabled 
      HasTracksFromPV();
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class HasTracksInTreeFromPV 
     *
     *  The simple predicate which evaluates to 'true' for 
     *  the particle which has in the decay tree at least 
     *  one particle which participates into 
     *  the primary vertex reconstruction
     *  
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LHCb::PrimVertex
     *  @see LoKi::Particles::HasTracksFromPV 
     *
     *  @see LoKi::Cuts::PVTRACKINTREE
     *  @see LoKi::Cuts::TRACKFROMPVINTREE
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22
     */
    class GAUDI_API HasTracksInTreeFromPV : public HasTracksFromPV 
    {
    public:
      // ======================================================================
      /// constructor from one primary vertex 
      HasTracksInTreeFromPV 
      ( const LHCb::RecVertex*               pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::RecVertex::ConstVector&  pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::RecVertex::Container*    pvs ) ;
      /// constructor from one vertex 
      HasTracksInTreeFromPV 
      ( const LHCb::VertexBase*              pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::VertexBase::ConstVector& pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LoKi::PhysTypes::VRange&       pvs ) ;
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksInTreeFromPV 
      ( VERTEX first , 
        VERTEX last  ) 
        : LoKi::Particles::HasTracksFromPV ( first , last ) 
      {}
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksInTreeFromPV 
      ( const LoKi::Keeper<VERTEX>& keeper ) 
        : LoKi::Particles::HasTracksFromPV ( keeper ) 
      {}
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksInTreeFromPV 
      ( const LoKi::UniqueKeeper<VERTEX>& keeper ) 
        : LoKi::Particles::HasTracksFromPV ( keeper ) 
      {}
      /// MANDATORY: virtual destructor
      virtual ~HasTracksInTreeFromPV() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracksInTreeFromPV* clone() const 
      { return new HasTracksInTreeFromPV(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // The defualt costructor is disabled 
      HasTracksInTreeFromPV();
      // ======================================================================
    } ;
    // ========================================================================    
  } //                                               end of namespace Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES10_H
// ============================================================================
