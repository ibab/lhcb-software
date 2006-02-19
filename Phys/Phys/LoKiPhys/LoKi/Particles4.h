// $Id: Particles4.h,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PARTICLES4_H 
#define LOKI_PARTICLES4_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/ImpParBase.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-19 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    /** @class ImpPar
     *  class for evaluation of impact parameter of 
     *  particle with respect to vertex
     *  The tool IGeomDispCalculator is used 
     *
     *  @see IGeomDispCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::IP
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ImpPar : 
      public    LoKi::Function<const LHCb::Particle*> , 
      public    LoKi::Vertices::ImpParBase 
    {
    public:
      /// constructor 
      ImpPar 
      ( const LHCb::Vertex*                     vertex ,   
        const LoKi::Vertices::ImpactParamTool&  tool   ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Point3D&                     point  ,   
        const LoKi::Vertices::ImpactParamTool&   tool   ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::ImpactParamTool&   tool   ,
        const LHCb::Vertex*                      vertex ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::ImpactParamTool&   tool   , 
        const LoKi::Point3D&                     point  ) ;
      /// constructor 
      ImpPar ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// copy constructor 
      ImpPar ( const LoKi::Particles::ImpPar&    right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpPar(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual ImpPar* clone() const { return new ImpPar( *this ) ; }        
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const { return ip( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluator 
      result_type ip ( argument p ) const ;
    private:
      // default constructor is private 
      ImpPar();
    };
    
    /** @class ImpParChi2
     *  class for evaluation of impact parameter chi2 
     *  of particle with respect to vertex
     *  The tool IGeomDispCalculator is used 
     *
     *  @see IGeomDispCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::IPCHI2
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ImpParChi2 : 
      public    LoKi::Function<const LHCb::Particle*> , 
      public    LoKi::Vertices::ImpParBase 
    {
    public:
      /// constructor 
      ImpParChi2 
      ( const LHCb::Vertex*                    vertex ,   
        const LoKi::Vertices::ImpactParamTool& tool   ) ;
      /// constructor 
      ImpParChi2 
      ( const LoKi::Point3D&                   point  ,   
        const LoKi::Vertices::ImpactParamTool& tool   ) ;
      /// constructor 
      ImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   ,
        const LHCb::Vertex*                    vertex ) ;
      /// constructor 
      ImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   , 
        const LoKi::Point3D&                   point  ) ;
      /// constructor 
      ImpParChi2 ( const LoKi::Vertices::ImpParBase&  tool  ) ;
      /// copy constructor 
      ImpParChi2 ( const LoKi::Particles::ImpParChi2& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpParChi2(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ImpParChi2* clone() const {return new ImpParChi2(*this) ;} ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const { return chi2( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluator 
      result_type chi2 ( argument p ) const ;
    private:
      // default constructor is private 
      ImpParChi2();
    };
    
    /** @class MinImpPar
     *  class for evaluation of minimal valeu of 
     *  impact parameter of 
     *  particle with respect to seevral vertices 
     *
     *  The tool IGeomDispCalculator is used 
     *
     *  @see IGeomDispCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LoKi::Vertices::ImpPar 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::MINIP
     *  @see LoKi::Cuts::IPMIN
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class MinImpPar : 
      public    LoKi::Function<const LHCb::Particle*> 
    {
    public:
      /// constructor from vertices and the tool
      MinImpPar
      ( const LHCb::Vertex::Vector& vertices         , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::Vertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const SmartRefVector<LHCb::Vertex>& vertices , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::PhysTypes::VRange& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::PrimVertex::Vector& vertices         , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::PrimVertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const SmartRefVector<LHCb::PrimVertex>& vertices , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::PrimVertices* vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::Vector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const SmartRefVector<LHCb::Vertex>& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LoKi::PhysTypes::VRange& vertices   ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::PrimVertex::Vector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::PrimVertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool&  tool     ,
        const SmartRefVector<LHCb::PrimVertex>& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool     , 
        const LHCb::PrimVertices*              vertices ) ;
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpPar 
      ( VERTEX                                 first , 
        VERTEX                                 last  , 
        const LoKi::Vertices::ImpactParamTool& tool  ) 
        : LoKi::Function<const LHCb::Particle*> ()
        , m_vertices ( first , last )
        , m_fun      ( (const LHCb::Vertex*) 0 , tool )
      { 
        removeVertex() ; 
      } ;
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param tool  helper tool 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      MinImpPar 
      ( const LoKi::Vertices::ImpactParamTool& tool  ,
        VERTEX                                 first , 
        VERTEX                                 last  ) 
        : LoKi::Function<const LHCb::Particle*> ()
        , m_vertices ( first , last )
        , m_fun      ( (const LHCb::Vertex*) 0 , tool )
      { 
        removeVertex() ; 
      } ;
      
      /// copy constructor 
      MinImpPar ( const LoKi::Particles::MinImpPar& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinImpPar(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpPar* clone() const { return new MinImpPar(*this) ; };
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const { return mip( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluator 
      result_type mip ( argument p ) const ;
    public:
      /** add the vertex the list of vertices 
       *  @param v vertex to be added 
       *  @return the actual size of the list 
       */
      size_t addVertex    ( const LHCb::Vertex* v     ) 
      { 
        // append 
        m_vertices.push_back ( v ) ; 
        // remove NULLs
        removeVertex() ; 
        return m_vertices.size() ;
      } ;
      /** add the vertices to the list 
       *  @param first 'begin'-iterator for the sequence of vertices  
       *  @param end   'end'-iterator for the sequence of vertices  
       *  @return the actual size of the list 
       */
      template <class VERTEX>
      size_t  addVertices  ( VERTEX first , VERTEX last ) 
      { 
        // append to the end 
        m_vertices.insert ( m_vertices.end() , first , last ) ; 
        // remove NULLs 
        removeVertex() ; 
        return m_vertices.size() ;
      } ;
      /** remove the vertex from the list
       *  @param  v vertex to be removed 
       *  @return number of removoed vertices
       */
      size_t removeVertex ( const LHCb::Vertex* v = 0 ) ;
    private:
      // default constructor is private 
      MinImpPar();
    private:
      // the actual IP evaluator 
      LoKi::Particles::ImpPar m_fun ; ///< the actual IP evaluator 
      typedef std::vector<const LHCb::Vertex*> VERTICES ;
      // the list of vertices 
      VERTICES  m_vertices ; ///< the list of vertices 
    };
    
  } ; // end of namespace LoKi::Particles
} ; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES4_H
// ============================================================================
