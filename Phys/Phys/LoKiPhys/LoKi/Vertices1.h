// $Id: Vertices1.h,v 1.7 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/11/27 12:01:32  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.5  2006/08/15 15:13:26  ibelyaev
//  update for new inheritance scheme Vertex<--VertexBase-->RecVertex
//
// Revision 1.4  2006/03/19 13:03:28  ibelyaev
//  minor update
//
// Revision 1.3  2006/02/23 21:14:09  ibelyaev
//   add new fuctors/predicates
//
// ============================================================================
#ifndef LOKI_VERTICES1_H 
#define LOKI_VERTICES1_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/VertexBase.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
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
    
    /** @class VertexDistance
     *  evaluator of the distance
     *  between vertex and the 'vertex'
     *  @see LoKi::Cuts::VVDIST
     *  @see LoKi::Cuts::VVD
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class VertexDistance : 
      public    LoKi::Function<const LHCb::VertexBase*> , 
      public    LoKi::Vertices::VertexHolder 
    {
    public:
      /// constructor 
      VertexDistance 
      ( const LHCb::VertexBase*  vertex ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Point3D& vertex ) ;      
      /// constructor 
      VertexDistance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// copy constructor 
      VertexDistance ( const VertexDistance& right ) ;
      /// destructor 
      virtual ~VertexDistance();
      /// clone method (mandatory!)
      virtual VertexDistance* clone() const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type         distance   ( argument v ) const ;
    private:
      /// default constructor is private 
      VertexDistance();
    };

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
    class VertexSignedDistance : 
      public    LoKi::Function<const LHCb::VertexBase*> 
    {
    public:
      /// constructor 
      VertexSignedDistance 
      ( const LHCb::VertexBase*  vertex ) ;
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Point3D& vertex ) ;      
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// copy constructor 
      VertexSignedDistance 
      ( const LoKi::Vertices::VertexDistance& right ) ;
      /// copy constructor 
      VertexSignedDistance 
      ( const VertexSignedDistance& right ) ;
      /// destructor 
      virtual ~VertexSignedDistance();
      /// clone method (mandatory!)
      virtual VertexSignedDistance* clone() const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type distance   ( argument v ) const ;
    private:
      /// default constructor is private 
      VertexSignedDistance();
    private:
      LoKi::Vertices::VertexDistance m_fun ;
    };
    
    /** @class VertexChi2Distance
     *  evaluator of the distance in chi2 units  
     *  between vertex and the 'vertex'
     *  @see LoKi::Cuts::VVDCHI2
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class VertexChi2Distance : 
      public    LoKi::Function<const LHCb::VertexBase*> , 
      public    LoKi::Vertices::VertexHolder 
    {
    public:
      /// constructor 
      VertexChi2Distance 
      ( const LHCb::VertexBase*  vertex ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Point3D& vertex ) ;      
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// copy constructor 
      VertexChi2Distance ( const VertexChi2Distance& right ) ;
      /// destructor 
      virtual ~VertexChi2Distance();
      /// clone method (mandatory!)
      virtual VertexChi2Distance* clone() const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type         chi2       ( argument v ) const ;
    private:
      /// default constructor is private 
      VertexChi2Distance();
    };
    
    /** @class MinVertexDistance
     *  class for evaluation of minimal GEOMETRY distance 
     *  between the vertex and other vertices 
     *  @see LoKi::Cuts::MINVVD
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-08
     */
    class MinVertexDistance :
      public LoKi::Function<const LHCb::VertexBase*> , 
      public LoKi::UniqueKeeper<LHCb::VertexBase>
    {
    public:
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
      ( const LHCb::RecVertices*   vs  ) ;
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
        : LoKi::Function<const LHCb::VertexBase*> () 
        , LoKi::UniqueKeeper<LHCb::VertexBase> ( first , last    )
        , m_fun                            ( LoKi::Point3D() )
      {}
      /// copy  constructor
      MinVertexDistance 
      ( const MinVertexDistance& right ) ;
      /// MANDATORY: destructor 
      virtual ~MinVertexDistance() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexDistance* clone() const 
      { return new MinVertexDistance(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default constructor is disabled 
      MinVertexDistance () ;
    private:
      LoKi::Vertices::VertexDistance m_fun  ;
    };


    /** @class MinVertexChi2Distance
     *  class for evaluation of minimal chi2 distance 
     *  between the vertex and other vertices 
     *  @see LoKi::Cuts::MINVVDCHI2 
     *  @author Vanya ELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-08
     */
    class MinVertexChi2Distance :
      public LoKi::Function<const LHCb::VertexBase*> , 
      public LoKi::UniqueKeeper<LHCb::VertexBase>
    {
    public:
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
        : LoKi::Function<const LHCb::Vertex*> () 
        , LoKi::UniqueKeeper<LHCb::Vertex> ( first , last    )
        , m_fun                            ( LoKi::Point3D() )
      {}
      /// copy  constructor
      MinVertexChi2Distance 
      ( const MinVertexChi2Distance& right ) ;
      /// MANDATORY: destructor 
      virtual ~MinVertexChi2Distance() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexChi2Distance* clone() const 
      { return new MinVertexChi2Distance(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default constructor is disabled 
      MinVertexChi2Distance () ;
    private:
      LoKi::Vertices::VertexChi2Distance m_fun  ;
    };

  }  // end of namespace LoKi::Vertices 
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTICES1_H
// ============================================================================
