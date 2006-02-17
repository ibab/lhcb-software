// $Id: VertexHolder.h,v 1.1.1.1 2006-02-17 19:17:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include "Event/Vertex.h"
// ============================================================================
class IGeomDispCalculator ;
// ============================================================================

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
      VertexHolder ( const LoKi::Point3D& point      ) ;
      /// constructor from Vertex 
      VertexHolder ( const LHCb::Vertex*  vertex = 0 ) ;
      /// copy constructor 
      VertexHolder ( const VertexHolder&  right      ) ;
      /// destructor
      virtual ~VertexHolder();
    public:
      /// accessor to the 'case' 
      Case                 type   () const { return m_type   ; }
      /// accessor to the vertex 
      const LHCb::Vertex*  vertex () const { return m_vertex ; }
      /// accessor to the point 
      const LoKi::Point3D& point  () const { return m_point  ; }
      /// change the vertex (and switch the case!)
      void setVertex ( const LHCb::Vertex*  vx  ) const 
      { m_vertex = vx ;                m_type = _vertex ; }      
      /// change the vertex (and switch the case!)
      void setPoint  ( const LoKi::Point3D& pnt ) const 
      { m_point = pnt ; m_vertex = 0 ; m_type = _point  ; }      
      /// return 'position'
      const LoKi::Point3D& position() const ;
      /// valid information 
      bool  valid() const 
      { return _point == type() ? true : 0 != m_vertex ; }
    private:
      mutable       Case           m_type   ;
      mutable const LHCb::Vertex*  m_vertex ;
      mutable       LoKi::Point3D  m_point  ;
    };
    
    class ImpactParamTool : 
      virtual public LoKi::AuxFunBase 
    {    
    public:
      /// accessor to the tool itself 
      inline IGeomDispCalculator*  tool     () const { return m_tool ; }
    protected:
      /// constructor 
      ImpactParamTool 
      ( IGeomDispCalculator*   tool  ) ;
      /// copy constructor 
      ImpactParamTool     
      ( const ImpactParamTool& tool  ) ;
      /// virtual destructor 
      virtual ~ImpactParamTool();
      /// assignement 
      ImpactParamTool& 
      operator=( const ImpactParamTool& right ) ;
    private:
      /// default constructor is private 
      ImpactParamTool();
    private:
      IGeomDispCalculator* m_tool;
    } ;

    
    /** @class ImpParBase
     *  helper FunB class for evaluation 
     *  impact parameters, distances, and so on.
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ImpParBase : 
      public virtual LoKi::Vertices::ImpactParamTool , 
      public virtual LoKi::Vertices::VertexHolder    
    {
    public:
      /// constructor 
      ImpParBase 
      ( const LHCb::Vertex*    vertex ,   
        IGeomDispCalculator*   tool   );
      /// constructor 
      ImpParBase 
      ( const LoKi::Point3D&   point  ,   
        IGeomDispCalculator*   tool   );
      /// constructor 
      ImpParBase 
      ( const VertexHolder&    holder ,   
        IGeomDispCalculator*   tool   );
      /// constructor 
      ImpParBase 
      ( const LHCb::Vertex*    vertex ,   
        const ImpactParamTool& tool   );
      /// constructor 
      ImpParBase 
      ( const LoKi::Point3D&   point  ,   
        const ImpactParamTool& tool   );
      /// constructor 
      ImpParBase 
      ( const VertexHolder&    holder ,   
        const ImpactParamTool& tool   );
      /// constructor 
      ImpParBase 
      ( const LoKi::Point3D&   point  ,   
        const ImpParBase&      tool   );
      /// constructor 
      ImpParBase 
      ( const LHCb::Vertex*    vertex ,   
        const ImpParBase&      tool   );
      /// constructor 
      ImpParBase 
      ( const VertexHolder&    holder ,   
        const ImpParBase&      tool   );
      /// copy constructor 
      ImpParBase 
      ( const ImpParBase&      base   );
      /// the destructor is virtual 
      virtual ~ImpParBase () ;
    private:
      /// default constructor is private 
      ImpParBase();
    };

  }; // end of namespace LoKi::Vertices;
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTEXHOLDER_H
// ============================================================================
