// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES4_H 
#define LOKI_PARTICLES4_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/VertexBase.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Keeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/ImpParBase.h"
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
 *  @date 2006-02-19 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class ImpPar
     *  class for evaluation of impact parameter of 
     *  particle with respect to vertex
     *  The tool IDistanceCalculator is used 
     *
     *  @see IDistanceCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::IP
     *
     *  Actually it is a common base class for all functors, dealing with
     *  IDistanceCalculator 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API ImpPar 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public LoKi::Vertices::ImpParBase 
    {
    public:
      // ======================================================================
      /// constructor 
      ImpPar 
      ( const LHCb::VertexBase*                 vertex ,   
        const LoKi::Vertices::ImpactParamTool&  tool   ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Point3D&                     point  ,   
        const LoKi::Vertices::ImpactParamTool&   tool   ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::VertexHolder&     vertex ,   
        const LoKi::Vertices::ImpactParamTool&  tool   ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::ImpactParamTool&   tool   ,
        const LHCb::VertexBase*                  vertex ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::ImpactParamTool&   tool   , 
        const LoKi::Point3D&                     point  ) ;
      /// constructor 
      ImpPar 
      ( const LoKi::Vertices::ImpactParamTool&   tool   ,
        const LoKi::Vertices::VertexHolder&      vertex ) ;
      /// constructor 
      ImpPar ( const LoKi::Vertices::ImpParBase& tool   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpPar(){} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual ImpPar* clone() const { return new ImpPar( *this ) ; }        
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return ip( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s <<  "IP" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// the actual evaluator of impact parameter 
      double ip     ( argument p ) const ;
      /// the actual evaluator of impact parameter chi2 
      double ipchi2 ( argument p ) const ;
      /** the actual evaluation of 'path-distance'
       *  @see IDistanceCalculator::pathDistance 
       *  @param p        (INPUT) the particle 
       *  @param distance (OUTPUT) the distance 
       *  @param error    (OUTPUT) the error 
       *  @param chi2     (OUTPUT) the chi2 
       *  @return status code 
       */
      StatusCode  path      ( argument p        , 
                              double&  distance , 
                              double&  error    , 
                              double&  chi2     ) const ;
      /** the actual evaluation of 'projected-distance'
       *  @see IDistanceCalculator::projectedDistance 
       *  @param p        (INPUT) the particle 
       *  @param distance (OUTPUT) the distance 
       *  @param error    (OUTPUT) the error 
       *  @return status code 
       */
      StatusCode  projected ( argument p        , 
                              double&  distance , 
                              double&  error    ) const ;
      /** the actual evaluation of 'projected-distance'
       *  @see IDistanceCalculator::projectedDistance 
       *  @param p        (INPUT) the particle 
       *  @return the distance 
       */
      double     projected ( argument p        ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      ImpPar(); // default constructor is private 
      // ======================================================================
    };
    // ========================================================================    
    /** @class ImpParChi2
     *  class for evaluation of impact parameter chi2 
     *  of particle with respect to vertex
     *  The tool IDistanceCalculator is used 
     *
     *  @see IDistanceCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::IPCHI2
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API ImpParChi2 : public LoKi::Particles::ImpPar 
    {
    public:
      /// constructor 
      ImpParChi2 
      ( const LHCb::VertexBase*                vertex ,   
        const LoKi::Vertices::ImpactParamTool& tool   ) 
        : LoKi::Particles::ImpPar ( vertex , tool ) {}
      /// constructor 
      ImpParChi2 
      ( const LoKi::Point3D&                   point  ,   
        const LoKi::Vertices::ImpactParamTool& tool   ) 
        : LoKi::Particles::ImpPar ( point  , tool ) {}
      /// constructor 
      ImpParChi2  
      ( const LoKi::Vertices::VertexHolder&     vertex ,   
        const LoKi::Vertices::ImpactParamTool&  tool   ) 
        : LoKi::Particles::ImpPar ( vertex , tool ) {}
      /// constructor 
      ImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   ,
        const LHCb::VertexBase*                vertex ) 
        : LoKi::Particles::ImpPar ( vertex , tool ) {}
      /// constructor 
      ImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   , 
        const LoKi::Point3D&                   point  ) 
        : LoKi::Particles::ImpPar ( point  , tool ) {}
      /// constructor 
      ImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool&   tool   ,
        const LoKi::Vertices::VertexHolder&      vertex ) 
        : LoKi::Particles::ImpPar ( vertex , tool ) {}
      /// constructor 
      ImpParChi2 ( const LoKi::Vertices::ImpParBase&  tool  ) 
        : LoKi::Particles::ImpPar ( tool ) {}
      /// MANDATORY: virtual destructor 
      virtual ~ImpParChi2(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ImpParChi2* clone() const {return new ImpParChi2(*this) ;} ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return ipchi2( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s <<  "IPCHI2" ; }
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is private 
      ImpParChi2() ; // default constructor is private 
      // ======================================================================
    };
    // ========================================================================    
    /** @class MinImpPar
     *  class for evaluation of minimal valeu of 
     *  impact parameter of 
     *  particle with respect to seevral vertices 
     *
     *  The tool IDistanceCalculator is used 
     *
     *  @see IDistanceCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LoKi::Vertices::ImpPar 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::MIP
     *  @see LoKi::Cuts::MINIP
     *  @see LoKi::Cuts::IPMIN
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API MinImpPar 
      : public LoKi::Particles::ImpPar 
      , public LoKi::UniqueKeeper<LHCb::VertexBase> 
    {
    public:
      // ======================================================================
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::VertexBase::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::Vertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::PhysTypes::VRange& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::RecVertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LHCb::RecVertex::Container* vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::VertexBase::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LoKi::PhysTypes::VRange& vertices   ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::RecVertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpPar
      ( const LoKi::Vertices::ImpactParamTool& tool     , 
        const LHCb::RecVertex::Container*   vertices ) ;
      // ======================================================================
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
        : LoKi::Particles::ImpPar ( (const LHCb::VertexBase*) 0 , tool )
        , LoKi::UniqueKeeper<LHCb::VertexBase> ( first , last )
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpPar 
      ( const LoKi::Keeper<VERTEX>& keeper , 
        const LoKi::Vertices::ImpactParamTool& tool  ) 
        : LoKi::Particles::ImpPar ( (const LHCb::VertexBase*) 0 , tool )
        , LoKi::UniqueKeeper<LHCb::VertexBase> ( keeper.begin() , keeper.end() )
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpPar 
      ( const LoKi::UniqueKeeper<VERTEX>& keeper , 
        const LoKi::Vertices::ImpactParamTool& tool  ) 
        : LoKi::Particles::ImpPar ( (const LHCb::VertexBase*) 0 , tool )
        , LoKi::UniqueKeeper<LHCb::VertexBase> ( keeper.begin() , keeper.end() )
      {}
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
        : LoKi::Particles::ImpPar ( (const LHCb::VertexBase*) 0 , tool )
        , LoKi::UniqueKeeper<LHCb::VertexBase> ( first , last )
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpPar 
      ( const LoKi::Vertices::ImpactParamTool& tool   , 
        const LoKi::Keeper<VERTEX>&            keeper )
        : LoKi::Particles::ImpPar ( (const LHCb::VertexBase*) 0 , tool )
        , LoKi::UniqueKeeper<LHCb::VertexBase> ( keeper.begin() , keeper.end() )
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpPar 
      ( const LoKi::Vertices::ImpactParamTool& tool   , 
        const LoKi::UniqueKeeper<VERTEX>&      keeper ) 
        : LoKi::Particles::ImpPar ( (const LHCb::VertexBase*) 0 , tool )
        , LoKi::UniqueKeeper<LHCb::VertexBase> ( keeper.begin() , keeper.end() )
      {}
      /// MANDATORY: virtual destructor 
      virtual ~MinImpPar(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpPar* clone() const { return new MinImpPar(*this) ; };
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const { return mip( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// the actual evaluator 
      result_type mip ( argument p ) const ;
      /// the actual evaluator 
      result_type mipchi2 ( argument p ) const ;
      // ======================================================================
    private:
      /// ===================================================================== 
      // default constructor is private 
      MinImpPar();
      /// ===================================================================== 
    };
    // ========================================================================
    /** @class MinImpParChi2
     *  class for evaluation of minimal value of 
     *  chi2 of impact parameter of 
     *  particle with respect to seevral vertices 
     *
     *  The tool IDistanceCalculator is used 
     *
     *  @see IDistanceCalculator
     *  @see LoKi::Vertices::ImpParBase
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see LoKi::Vertices::ImpParChi2 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::MIPCHI2
     *  @see LoKi::Cuts::CHI2MIP
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class GAUDI_API MinImpParChi2 : public LoKi::Particles::MinImpPar
    {
    public:
      // ======================================================================
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::VertexBase::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::Vertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::PhysTypes::VRange& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::RecVertex::ConstVector& vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LHCb::RecVertex::Container* vertices   , 
        const LoKi::Vertices::ImpactParamTool& tool ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::VertexBase::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LHCb::Vertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool ,
        const LoKi::PhysTypes::VRange& vertices   ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool , 
        const LHCb::RecVertex::ConstVector& vertices ) ;
      /// constructor from the vertices and the tool
      MinImpParChi2
      ( const LoKi::Vertices::ImpactParamTool& tool     , 
        const LHCb::RecVertex::Container*  vertices ) ;
      // ======================================================================
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( VERTEX                                 first , 
        VERTEX                                 last  , 
        const LoKi::Vertices::ImpactParamTool& tool  ) 
        : LoKi::Particles::MinImpPar ( first , last , tool ) 
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( const LoKi::Keeper<VERTEX>&            keeper , 
        const LoKi::Vertices::ImpactParamTool& tool   ) 
        : LoKi::Particles::MinImpPar ( keeper , tool ) 
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( const LoKi::UniqueKeeper<VERTEX>&      keeper , 
        const LoKi::Vertices::ImpactParamTool& tool   ) 
        : LoKi::Particles::MinImpPar ( keeper , tool ) 
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param tool  helper tool 
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool  ,
        VERTEX                                 first , 
        VERTEX                                 last  ) 
        : LoKi::Particles::MinImpPar ( tool , first , last , tool ) 
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   , 
        const LoKi::Keeper<VERTEX>&            keeper )
        : LoKi::Particles::MinImpPar ( tool , keeper  ) 
      {}
      /** templated constructor from arbitrary sequence 
       *  of objects, convertible to "const LHCb::Vertex*"
       *  @param first 'begin'-iterator of the sequence 
       *  @param last  'end'-iterator of the sequence 
       *  @param tool  helper tool 
       */
      template <class VERTEX>
      MinImpParChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool   , 
        const LoKi::UniqueKeeper<VERTEX>&      keeper )
        : LoKi::Particles::MinImpPar ( tool , keeper ) 
      {}
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~MinImpParChi2(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParChi2* clone() const 
      { return new MinImpParChi2(*this) ; };
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return mipchi2( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is private 
      MinImpParChi2();
      // ======================================================================
    };
    // ========================================================================    
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES4_H
// ============================================================================
