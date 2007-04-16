// $Id: VertexHolder.h,v 1.5 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/11/27 12:01:32  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.3  2006/08/15 15:13:25  ibelyaev
//  update for new inheritance scheme Vertex<--VertexBase-->RecVertex
//
// Revision 1.2  2006/02/19 21:49:12  ibelyaev
//  restructirisation + new funtions
//
// ============================================================================
#ifndef LOKI_VERTEXHOLDER_H 
#define LOKI_VERTEXHOLDER_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Kinematics.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/VertexBase.h"
// ============================================================================

// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Vertices::ImpParBase
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

namespace LoKi
{
  namespace Vertices 
  {
    /** @class VertexHolder 
     *  simple helper class to keep the 
     *  information about the vertices/points, 
     *  used by other tools 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-01-24
     */
    class VertexHolder : virtual public LoKi::AuxFunBase 
    {
    public:
      /// switch to distinguish different use cases 
      enum Case { _vertex , _point };
    public:
      /// constructor from the 3Dpoint
      VertexHolder ( const LoKi::Point3D&     point      ) ;
      /// constructor from Vertex 
      VertexHolder ( const LHCb::VertexBase*  vertex = 0 ) ;
      /// copy constructor 
      VertexHolder ( const VertexHolder&      right      ) ;
      /// destructor
      virtual ~VertexHolder();
    public:
      /// accessor to the 'case' 
      Case                     type   () const { return m_type   ; }
      /// accessor to the vertex 
      const LHCb::VertexBase*  vertex () const { return m_vertex ; }
      /// accessor to the point 
      const LoKi::Point3D& point  () const { return m_point  ; }
      /// change the vertex (and switch the case!)
      inline void setVertex ( const LHCb::VertexBase*  vx  ) const 
      { m_vertex = vx ;                m_type = _vertex ; }      
      /// change the vertex (and switch the case!)
      inline void setPoint  ( const LoKi::Point3D&     pnt ) const 
      { m_point = pnt ; m_vertex = 0 ; m_type = _point  ; }      
      /// change the vertex (and switch the case!)
      inline void setVertex ( const LoKi::Point3D&     pnt ) const 
      { setPoint  ( pnt ) ; }
      /// change the vertex (and switch the case!)
      inline void setPoint  ( const LHCb::VertexBase*  vx   ) const 
      { setVertex ( vx  ) ; }      
      /// return 'position'
      const LoKi::Point3D& position() const ;
      /// valid information 
      bool  valid() const 
      { return _point == type() ? true : 0 != m_vertex ; }
    private:
      mutable       Case               m_type   ;
      mutable const LHCb::VertexBase*  m_vertex ;
      mutable       LoKi::Point3D      m_point  ;
    };
    
  } // end of namespace LoKi::Vertices;
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTEXHOLDER_H
// ============================================================================
