// $Id: Particles1.h,v 1.8 2007-11-28 14:39:29 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES1_H 
#define LOKI_PARTICLES1_H 1
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
#include "LoKi/Vertices1.h"
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
  namespace Particles 
  { 
    // ========================================================================
    /** @class VertexDistance
     *
     *  evaluator of the GEOMETRY 
     *  distance between the particle 
     *  "endVertex" and "the vertex". 
     *
     *  The actual evaluation 
     *  is delegated to the function 
     *  LoKi::Vertices::VertexDistance 
     * 
     *  @see LoKi::Vertices::VertexDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     *
     *  @see LoKi::Cuts::VD 
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      /// constructor 
      VertexDistance 
      ( const LHCb::VertexBase*               vertex ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Vertices::VertexDistance& base   ) ;
      /// copy constructor 
      VertexDistance 
      ( const LoKi::Particles::VertexDistance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexDistance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type distance           ( argument p ) const ;
    public:
      void setVertex ( const LHCb::VertexBase* vertex ) const 
      { m_fun.setVertex ( vertex )  ; }
      void setVertex ( const LoKi::Point3D&    vertex ) const 
      { m_fun.setVertex ( vertex )  ; }
    private:
      /// default constructor is private
      VertexDistance();
    private:
      // the actual evaluator 
      LoKi::Vertices::VertexDistance m_fun ;
    } ;
    // ========================================================================
    /** @class VertexSignedDistance
     *
     *  evaluator of the GEOMETRY 
     *  distance between the particle 
     *  "endVertex" and "the vertex". 
     *  The distance is signed according 
     *  to the sign of (Zv-Z0)
     *
     *  The actual evaluation 
     *  is delegated to the function 
     *  LoKi::Vertices::VertexSignedDistance 
     * 
     *  @see LoKi::Vertices::VertexSignedDistance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @see LoKi::Cuts::VDSIGN
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexSignedDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      /// constructor 
      VertexSignedDistance 
      ( const LHCb::VertexBase*               vertex ) ;
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexSignedDistance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// constructor 
      VertexSignedDistance       
      ( const LoKi::Vertices::VertexDistance& base   ) ;
      /// constructor 
      VertexSignedDistance       
      ( const LoKi::Vertices::VertexSignedDistance& base   ) ;
      /// copy constructor 
      VertexSignedDistance 
      ( const LoKi::Particles::VertexSignedDistance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexSignedDistance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexSignedDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type distance           ( argument p ) const ;
    private:
      /// default constructor is private
      VertexSignedDistance();
    private:
      // the actual evaluator 
      LoKi::Vertices::VertexSignedDistance m_fun ;
    } ;
    // ========================================================================
    /** @class VertexDotDistance
     *
     *  evaluator of the
     *  distance between the particle 
     *  "endVertex" and "the vertex" along 
     *  the particle momentum
     *
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @see LoKi::Cuts::VDDOT
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexDotDistance 
      : public    LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public    LoKi::Vertices::VertexHolder 
    {
    public:
      /// constructor 
      VertexDotDistance 
      ( const LHCb::VertexBase*               vertex ) ;
      /// constructor 
      VertexDotDistance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexDotDistance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// copy constructor 
      VertexDotDistance 
      ( const LoKi::Particles::VertexDotDistance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexDotDistance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexDotDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type distance           ( argument p ) const ;
    private:
      /// default constructor is private
      VertexDotDistance();
    } ;
    // ========================================================================    
    /** @class VertexChi2Distance
     *
     *  evaluator of the chi2 of GEOMETRY 
     *  distance between the particle 
     *  "endVertex" and "the vertex" 
     *
     *  The actual evaluation 
     *  is delegated to the function 
     *  LoKi::Vertices::VertexChi2Distance 
     *  
     *  @see LoKi::Vertices::VertexChi2Distance 
     *  @see LHCb::Vertex
     *  @see LHCb::Particle
     * 
     *  @see LoKi::Cuts::VDCHI2 
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexChi2Distance
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      /// constructor 
      VertexChi2Distance 
      ( const LHCb::VertexBase*               vertex ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Point3D&                  vertex ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Vertices::VertexHolder&   base   ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Vertices::VertexChi2Distance& base   ) ;
      /// copy constructor 
      VertexChi2Distance 
      ( const LoKi::Particles::VertexChi2Distance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexChi2Distance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexChi2Distance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type chi2 ( argument p ) const ;
    private:
      /// default constructor is private
      VertexChi2Distance();
    private:
      // the actual evaluator 
      LoKi::Vertices::VertexChi2Distance m_fun ;
    } ;
    // ========================================================================    
    /** @class MinVertexDistance
     *
     *  class for evaluation of minimal GEOMETRY distance 
     *  between the "endVertex" and other vertices
     * 
     *  @see LoKi::Cuts::MINVD 
     *  @see LoKi::Cuts::VDMIN 
     *  
     *  @author Vanya ELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-08
     */
    class MinVertexDistance
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
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
       *  @param vs container of vertices 
       */
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
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
        , m_fun                  ( first , last )
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
    public:
      size_t size  () const { return m_fun.size  () ;  }
      bool   empty () const { return m_fun.empty () ;  }
    private:
      // default constructor is disabled 
      MinVertexDistance () ;
    private:
      LoKi::Vertices::MinVertexDistance m_fun  ;
    };
    // ========================================================================
    /** @class MinVertexChi2Distance
     *  class for evaluation of minimal chi2 distance 
     *  between the vertex and other vertices 
     *
     *  @see LoKi::Cuts::MINVDCHI2
     *  @see LoKi::Cuts::VDMINCHI2  
     *  
     *  @author Vanya ELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-08
     */
    class MinVertexChi2Distance
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
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
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
        , m_fun  ( first , last )
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
      LoKi::Vertices::MinVertexChi2Distance m_fun  ;
    } ;
    // ========================================================================    
  }  // end of namespace LoKi::Particles
} // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES1_H
// ============================================================================
