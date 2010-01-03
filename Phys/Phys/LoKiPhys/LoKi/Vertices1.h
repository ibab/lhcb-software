// $Id: Vertices1.h,v 1.15 2010-01-03 08:55:09 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VERTICES1_H 
#define LOKI_VERTICES1_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/VertexHolder.h"
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
 *  @date 2006-02-16 
 */
// ============================================================================
namespace LoKi 
{
  namespace  Vertices 
  {
    // ========================================================================
    /** @class VertexDistance
     *  evaluator of the distance
     *  between vertex and the 'vertex'
     *  @see LoKi::Cuts::VVDIST
     *  @see LoKi::Cuts::VVD
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class VertexDistance 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function 
      , public LoKi::Vertices::VertexHolder 
    {
    public:
      // ======================================================================
      /// constructor 
      VertexDistance 
      ( const LHCb::VertexBase*  vertex ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Point3D& vertex ) ;      
      /// constructor 
      VertexDistance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// destructor 
      virtual ~VertexDistance(){}
      /// clone method (mandatory!)
      virtual VertexDistance* clone() const 
      { return new VertexDistance(*this) ; }
      /** the only one essential method 
       *  @param v pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const 
      { return distance( v ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "VVDIST" ; }
      /** the only one essential method 
       *  @param v pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type         distance   ( argument v ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      VertexDistance();
      // ======================================================================
    };
    // ========================================================================
    /** @class VertexSignedDistance
     *  evaluator of the distance
     *  between vertex and the 'vertex', signed 
     *  according (Zv-Z0)
     *  
     *  The actual evaluation is done by 
     *   LoKi::Vertices::VertexDistance 
     *  
     *  @see LoKi::Vertices::VertexDistance 
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::VVDSIGN
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class VertexSignedDistance : public LoKi::Vertices::VertexDistance 
    {
    public:
      // ======================================================================
      /// constructor 
      VertexSignedDistance 
      ( const LHCb::VertexBase*  vertex ) 
        : LoKi::Vertices::VertexDistance( vertex ) {}
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Point3D& vertex )       
        : LoKi::Vertices::VertexDistance( vertex  ) {}
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Vertices::VertexHolder& base ) 
        : LoKi::Vertices::VertexDistance( base   ) {}
      /// destructor 
      virtual ~VertexSignedDistance() {}
      /// clone method (mandatory!)
      virtual VertexSignedDistance* clone() const 
      { return new VertexSignedDistance ( *this ) ; }
      /** the only one essential method 
       *  @param v pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const 
      { return signedDistance ( v ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "VVDSIGN" ; }
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type signedDistance   ( argument v ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// default constructor is private 
      VertexSignedDistance();
      // ======================================================================
    };
    // ========================================================================
    /** @class VertexDeltaZ 
     *  Trivial functor to evaluate the delta-Z with respect to "the vertex"
     *  @see LoKi::Cuts::VDZ 
     *  @author Vanya BELYAEV Ivan.Belyaev@inkhef.nl
     *  @date 2009-03-11
     */
    class VertexDeltaZ : public LoKi::Vertices::VertexDistance 
    {
    public:
      // ======================================================================
      /// constructor 
      VertexDeltaZ 
      ( const LHCb::VertexBase*  vertex ) 
        : LoKi::Vertices::VertexDistance( vertex ) {}
      /// constructor 
      VertexDeltaZ 
      ( const LoKi::Point3D& vertex )       
        : LoKi::Vertices::VertexDistance( vertex  ) {}
      /// constructor 
      VertexDeltaZ  
      ( const LoKi::Vertices::VertexHolder& base ) 
        : LoKi::Vertices::VertexDistance( base   ) {}
      /// destructor 
      virtual ~VertexDeltaZ() {}
      /// clone method (mandatory!)
      virtual VertexDeltaZ* clone() const 
      { return new VertexDeltaZ ( *this ) ; }
      /** the only one essential method 
       *  @param v pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const 
      { return deltaZ ( v ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "VDZ" ; }
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type deltaZ( argument v ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// default constructor is private 
      VertexDeltaZ();                         // default constructor is private 
      // ======================================================================
    };
    // ========================================================================
    /** @class VertexAbsDeltaZ 
     *  Trivial functor to evaluate the abs(deltaZ) with respect to "the vertex"
     *  @see LoKi::Cuts::VABSDZ 
     *  @author Vanya BELYAEV Ivan.Belyaev@inkhef.nl
     *  @date 2009-03-11
     */
    class VertexAbsDeltaZ : public LoKi::Vertices::VertexDeltaZ
    {
    public:
      // ======================================================================
      /// constructor 
      VertexAbsDeltaZ 
      ( const LHCb::VertexBase*  vertex ) 
        : LoKi::Vertices::VertexDeltaZ ( vertex ) {}
      /// constructor 
      VertexAbsDeltaZ 
      ( const LoKi::Point3D& vertex )       
        : LoKi::Vertices::VertexDeltaZ ( vertex  ) {}
      /// constructor 
      VertexAbsDeltaZ  
      ( const LoKi::Vertices::VertexHolder& base ) 
        : LoKi::Vertices::VertexDeltaZ ( base   ) {}
      /// destructor 
      virtual ~VertexAbsDeltaZ() {}
      /// clone method (mandatory!)
      virtual VertexAbsDeltaZ* clone() const 
      { return new VertexAbsDeltaZ ( *this ) ; }
      /** the only one essential method 
       *  @param v pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const 
      { return absDeltaZ ( v ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "VABSDZ" ; }
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type absDeltaZ ( argument v ) const ;
      // ======================================================================
    private :
      // ======================================================================
      /// default constructor is private 
      VertexAbsDeltaZ();                      // default constructor is private 
      // ======================================================================
    };
    // ========================================================================    
    /** @class VertexChi2Distance
     *  evaluator of the distance in chi2 units  
     *  between vertex and the 'vertex'
     *  @see LoKi::Cuts::VVDCHI2
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class VertexChi2Distance : public LoKi::Vertices::VertexDistance 
    {
    public:
      // ======================================================================
      /// constructor 
      VertexChi2Distance 
      ( const LHCb::VertexBase*  vertex ) 
        : LoKi::Vertices::VertexDistance ( vertex ) {}      
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Point3D& vertex ) 
        : LoKi::Vertices::VertexDistance ( vertex ) {}
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Vertices::VertexHolder& base ) 
        : LoKi::Vertices::VertexDistance ( base ) {}
      /// destructor 
      virtual ~VertexChi2Distance() {} ;
      /// clone method (mandatory!)
      virtual VertexChi2Distance* clone() const 
      { return new VertexChi2Distance ( *this ) ; }
      /** the only one essential method 
       *  @param v pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const 
      { return chi2 ( v ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "VVDCHI2" ; }
      /** the only one essential method 
       *  @param v pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type         chi2       ( argument v ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      VertexChi2Distance();
      // ======================================================================
    };
    // ========================================================================    
    /** @class MinVertexDistance
     *  class for evaluation of minimal GEOMETRY distance 
     *  between the vertex and other vertices 
     *  @see LoKi::Cuts::MINVVD
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-08
     */
    class MinVertexDistance : public LoKi::Vertices::VertexDistance
    {
    public:
      // ======================================================================
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LHCb::VertexBase::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      MinVertexDistance 
      ( const LHCb::VertexBase::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LHCb::Vertex::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      MinVertexDistance 
      ( const LHCb::Vertex::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      MinVertexDistance 
      ( const SmartRefVector<LHCb::Vertex>& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LHCb::RecVertex::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LHCb::RecVertex::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LHCb::RecVertex::Container*   vs  ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LoKi::PhysTypes::VRange& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LoKi::Keeper<LHCb::VertexBase>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LoKi::UniqueKeeper<LHCb::VertexBase>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LoKi::Keeper<LHCb::Vertex>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LoKi::UniqueKeeper<LHCb::Vertex>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LoKi::Keeper<LHCb::RecVertex>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexDistance 
      ( const LoKi::UniqueKeeper<LHCb::RecVertex>& vs ) ;      
      /** templated constructor from arbitrary sequence 
       *  of vertices and helper object 
       *  @param first begin iterator of arbitrary sequence of primary vertices
       *  @param last  end iterator of arbitrary sequence of primary vertices
       */
      template <class VERTEX>
      MinVertexDistance 
      ( VERTEX                          first , 
        VERTEX                          last  )
        : LoKi::Vertices::VertexDistance ( LoKi::Point3D() ) 
        , m_keeper ( first , last )
      {}
      /// MANDATORY: destructor 
      virtual ~MinVertexDistance() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexDistance* clone() const 
      { return new MinVertexDistance(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const 
      { return minvd ( v ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "MINVVD" ; }
      // ======================================================================
    public:
      // ======================================================================
      result_type minvd ( argument v ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const LoKi::Keeper<LHCb::VertexBase>& keeper   () const { return m_keeper ; }
      const LoKi::Keeper<LHCb::VertexBase>& vertices () const { return m_keeper ; }
      operator const LoKi::Keeper<LHCb::VertexBase>& () const { return m_keeper ; }
      LoKi::Keeper<LHCb::VertexBase>::const_iterator begin () const 
      { return m_keeper.begin () ; }
      LoKi::Keeper<LHCb::VertexBase>::const_iterator end   () const 
      { return m_keeper.end   () ; }      
      bool   empty () const { return m_keeper.empty () ; }
      size_t size  () const { return m_keeper.size  () ; }
      // ======================================================================      
    protected:
      // ======================================================================      
      void clear () const { m_keeper.clear() ; }
      // ======================================================================
      template <class VERTEX>
      size_t addObjects ( VERTEX first , VERTEX last ) const
      { return m_keeper.addObjects ( first , last ) ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// default constructor is disabled 
      MinVertexDistance () ;
      // ======================================================================
    private:
      // ======================================================================
      /// keeper for the vertices 
      mutable LoKi::Keeper<LHCb::VertexBase> m_keeper ;
      // ======================================================================
    };
    // ========================================================================
    /** @class MinVertexChi2Distance
     *  class for evaluation of minimal chi2 distance 
     *  between the vertex and other vertices 
     *  @see LoKi::Cuts::MINVVDCHI2 
     *  @author Vanya ELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-08
     */
    class MinVertexChi2Distance : public LoKi::Vertices::VertexChi2Distance 
    {
    public:
      // ======================================================================
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::VertexBase::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::VertexBase::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::Vertex::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::Vertex::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      MinVertexChi2Distance 
      ( const SmartRefVector<LHCb::Vertex>& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::RecVertex::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::RecVertex::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::RecVertices*   vs  ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::PhysTypes::VRange& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::Keeper<LHCb::VertexBase>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::UniqueKeeper<LHCb::VertexBase>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::Keeper<LHCb::Vertex>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::UniqueKeeper<LHCb::Vertex>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::Keeper<LHCb::RecVertex>& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::UniqueKeeper<LHCb::RecVertex>& vs ) ;      
      /** templated constructor from arbitrary sequence 
       *  of vertices and helper object 
       *  @param first begin iterator of arbitrary sequence of primary vertices
       *  @param last  end iterator of arbitrary sequence of primary vertices
       */
      template <class VERTEX>
      MinVertexChi2Distance 
      ( VERTEX                          first , 
        VERTEX                          last  )
        : LoKi::Vertices::VertexChi2Distance ( LoKi::Point3D() )
        , m_keeper ( first , last    )
      {}
      /// MANDATORY: destructor 
      virtual ~MinVertexChi2Distance() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexChi2Distance* clone() const 
      { return new MinVertexChi2Distance ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const 
      { return minvdchi2 ( v ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "MINVVDCHI2" ; }
      // ======================================================================
    public:
      // ======================================================================
      result_type minvdchi2 ( argument v ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const LoKi::Keeper<LHCb::VertexBase>& keeper   () const { return m_keeper ; }
      const LoKi::Keeper<LHCb::VertexBase>& vertices () const { return m_keeper ; }
      operator const LoKi::Keeper<LHCb::VertexBase>& () const { return m_keeper ; }
      LoKi::Keeper<LHCb::VertexBase>::const_iterator begin () const 
      { return m_keeper.begin () ; }
      LoKi::Keeper<LHCb::VertexBase>::const_iterator end   () const 
      { return m_keeper.end   () ; }      
      bool   empty () const { return m_keeper.empty () ; }
      size_t size  () const { return m_keeper.size  () ; }
      // ======================================================================      
    protected:
      // ======================================================================      
      void clear () const { m_keeper.clear() ; }
      // ======================================================================
      template <class VERTEX>
      size_t addObjects ( VERTEX first , VERTEX last ) const
      { return m_keeper.addObjects ( first , last ) ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// default constructor is disabled 
      MinVertexChi2Distance () ;
      // ======================================================================
    private:
      // ======================================================================
      /// keeper for the vertices 
      mutable LoKi::Keeper<LHCb::VertexBase> m_keeper ;
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Vertices 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTICES1_H
// ============================================================================
