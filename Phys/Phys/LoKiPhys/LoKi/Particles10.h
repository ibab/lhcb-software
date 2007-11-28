// $Id: Particles10.h,v 1.7 2007-11-28 14:39:29 ibelyaev Exp $
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
namespace LHCb { class RecVertex ; }
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
     *  @see LHCb::PrimVertex
     *  @see LoKi::Particles::HasTracks
     *
     *  @see LoKi::Cuts::PVTRACK
     *  @see LoKi::Cuts::TRACKFROMPV
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22
     */
    class HasTracksFromPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      /// constructor from one primary vertex 
      HasTracksFromPV ( const LHCb::RecVertex*               pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksFromPV ( const LHCb::RecVertex::Vector&       pvs ) ;
      /// constructor from vector of primary vertices 
      HasTracksFromPV ( const LHCb::RecVertex::ConstVector&  pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LHCb::RecVertices*             pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LoKi::Keeper<LHCb::RecVertex>& pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LoKi::UniqueKeeper<LHCb::RecVertex>& pvs ) ;
      /// constructor from one vertex 
      HasTracksFromPV ( const LHCb::VertexBase*               pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksFromPV ( const LHCb::VertexBase::Vector&       pvs ) ;
      /// constructor from vector of primary vertices 
      HasTracksFromPV ( const LHCb::VertexBase::ConstVector&  pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LoKi::PhysTypes::VRange&        pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LoKi::Keeper<LHCb::VertexBase>& pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksFromPV ( const LoKi::UniqueKeeper<LHCb::VertexBase>& pvs ) ;
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksFromPV 
      ( VERTEX first , 
        VERTEX last  ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ()
        , m_cut () 
      { addVertices ( first , last ) ; }
      /// copy constructor 
      HasTracksFromPV ( const HasTracksFromPV& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasTracksFromPV() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracksFromPV* clone() const 
      { return new HasTracksFromPV(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /** add the vertex 
       *  @param vertex vertex to be added 
       *  @return the actual number of tracks 
       */
      size_t addVertex ( const LHCb::VertexBase* vertex ) ;
      /** add the vertex 
       *  @param vertex vertex to be added 
       *  @return the actual number of tracks 
       */
      size_t addVertex ( const LHCb::RecVertex*  vertex ) ;      
      /** add the sequence of vertices  
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       *  @return the actual number of tracks 
       */          
      template <class VERTEX>
      size_t addVertices 
      ( VERTEX first , 
        VERTEX last  ) 
      { 
        for ( ; first != last ; ++first ) { addVertex ( *first ) ; } 
        return size() ;
      } ;
    public:
      /// empty vector of tracks?
      bool   empty () const { return m_cut.empty () ; }
      /// the actual size of the vector of tracks 
      size_t size  () const { return m_cut.size  () ; }      
    private:
      // The defualt costructor is disabled 
      HasTracksFromPV();
    private:
      LoKi::Particles::HasTracks m_cut ;
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
    class HasTracksInTreeFromPV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public:
      /// constructor from one primary vertex 
      HasTracksInTreeFromPV 
      ( const LHCb::RecVertex*               pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::RecVertex::Vector&       pvs ) ;
      /// constructor from vector of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::RecVertex::ConstVector&  pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::RecVertices*             pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LoKi::Keeper<LHCb::RecVertex>& pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LoKi::UniqueKeeper<LHCb::RecVertex>& pvs ) ;
      /// constructor from one vertex 
      HasTracksInTreeFromPV 
      ( const LHCb::VertexBase*                   pv  ) ;
      /// constructor from vector of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::VertexBase::Vector&           pvs ) ;
      /// constructor from vector of primary vertices 
      HasTracksInTreeFromPV 
      ( const LHCb::VertexBase::ConstVector&      pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LoKi::PhysTypes::VRange&        pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LoKi::Keeper<LHCb::VertexBase>&     pvs ) ;
      /// constructor from container of primary vertices 
      HasTracksInTreeFromPV 
      ( const LoKi::UniqueKeeper<LHCb::VertexBase>& pvs ) ;
      /** templated constructor from sequence of vertices 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class VERTEX>
      HasTracksInTreeFromPV 
      ( VERTEX first , 
        VERTEX last  ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ()
        , m_cut ( first , last )
      {}
      /// copy constructor 
      HasTracksInTreeFromPV ( const HasTracksInTreeFromPV& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasTracksInTreeFromPV() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasTracksInTreeFromPV* clone() const 
      { return new HasTracksInTreeFromPV(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /** add the vertex 
       *  @param vertex vertex to be added 
       *  @return the actual number of tracks 
       */
      size_t addVertex ( const LHCb::VertexBase*     vertex ) 
      { return m_cut.addVertex ( vertex ) ; }
      /** add the vertex 
       *  @param vertex vertex to be added 
       *  @return the actual number of tracks 
       */
      size_t addVertex ( const LHCb::RecVertex* vertex ) 
      { return m_cut.addVertex ( vertex ) ; }
      /** add the sequence of vertices  
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       *  @return the actual number of tracks 
       */          
      template <class VERTEX>
      size_t addVertices 
      ( VERTEX first , 
        VERTEX last  ) 
      { return m_cut.addVertices( first , last ) ; } ;
    public:
      bool   empty () const { return m_cut.empty () ; }
      size_t size  () const { return m_cut.size  () ; }      
    private:
      // The defualt costructor is disabled 
      HasTracksInTreeFromPV();
    private:
      LoKi::Particles::HasTracksFromPV m_cut ;
    } ;
    // ========================================================================    
  } // end of namespace Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES10_H
// ============================================================================
