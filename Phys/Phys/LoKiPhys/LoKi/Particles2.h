// $Id: Particles2.h,v 1.10 2008-11-02 20:13:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES2_H 
#define LOKI_PARTICLES2_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Particles1.h"
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
    /** @class TimeDistance
     *  
     *  evaluator of the time distance (c*tau) 
     *  between particle vertex and the 'vertex'
     *  
     *  LoKi::Particles::VertexDistance is used 
     *  for evaluation of distance
     * 
     *  @see LoKi::Particles::VertexDistance 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class TimeDistance : public LoKi::Particles::VertexDistance 
    {
    public:
      /// constructor 
      TimeDistance 
      ( const LHCb::VertexBase*  vertex )
        : LoKi::Particles::VertexDistance ( vertex ) {}
      /// constructor 
      TimeDistance 
      ( const LoKi::Point3D&     vertex )
        : LoKi::Particles::VertexDistance ( vertex ) {}
      /// constructor
      TimeDistance 
      ( const LoKi::Vertices::VertexHolder& vertex ) 
        : LoKi::Particles::VertexDistance ( vertex ) {}
      /// MANDATORY: destructor
      virtual ~TimeDistance(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TimeDistance* clone() const 
      { return new LoKi::Particles::TimeDistance(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return time ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TDIST" ; }
      /// the actual computation 
      result_type  time  ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      TimeDistance();
      // ======================================================================
    };
    // ========================================================================
    /** @class TimeSignedDistance
     *  
     *  evaluator of the time distance (c*tau) 
     *  between particle vertex and the 'vertex'
     *  
     *  LoKi::Particles::VertexSignedDistance is used 
     *  for evaluation of distance
     * 
     *  @see LoKi::Particles::VertexSignedDistance 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class TimeSignedDistance : public LoKi::Particles::VertexSignedDistance 
    {
    public:
      // ======================================================================
      /// constructor 
      TimeSignedDistance 
      ( const LHCb::VertexBase*  vertex )
        : LoKi::Particles::VertexSignedDistance ( vertex ) {}
      /// constructor 
      TimeSignedDistance 
      ( const LoKi::Point3D&     vertex )
        : LoKi::Particles::VertexSignedDistance ( vertex ) {}
      /// constructor
      TimeSignedDistance 
      ( const LoKi::Vertices::VertexHolder& vertex ) 
        : LoKi::Particles::VertexSignedDistance ( vertex ) {}
      /// MANDATORY: destructor
      virtual ~TimeSignedDistance(){} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TimeSignedDistance* clone() const 
      { return new TimeSignedDistance ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return time ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TDSIGN" ; }
      /// the actual computation 
      result_type  time  ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      TimeSignedDistance();
      // ======================================================================
    };
    // ========================================================================
    /** @class TimeDotDistance
     *  
     *  evaluator of the time distance (c*tau) 
     *  between particle vertex and the 'vertex'
     *  
     *  LoKi::Particles::VertexDotDistance is used 
     *  for evaluation of distance
     * 
     *  @see LoKi::Particles::VertexDotDistance 
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class TimeDotDistance : public LoKi::Particles::VertexDotDistance 
    {
    public:
      // ======================================================================
      /// constructor 
      TimeDotDistance 
      ( const LHCb::VertexBase* vertex ) 
        : LoKi::Particles::VertexDotDistance ( vertex ) {}
      /// constructor 
      TimeDotDistance 
      ( const LoKi::Point3D& vertex ) 
        : LoKi::Particles::VertexDotDistance ( vertex ) {}
      /// constructor 
      TimeDotDistance 
      ( const LoKi::Vertices::VertexHolder&    vertex ) 
        : LoKi::Particles::VertexDotDistance ( vertex ) {}
      /// MANDATORY: virtual destructor 
      virtual ~TimeDotDistance () {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TimeDotDistance* clone() const 
      { return new TimeDotDistance(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return time ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "TDOT" ; }
      /// the actual computation 
      result_type time           ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private
      TimeDotDistance();
      // ======================================================================
    };
    // ========================================================================
    /** @class LifetimeDistance
     *
     *  The estimate of the particle lifetime (c*tau), based on the 
     *  first iteration of "Lifetime-fitter" and neglecting the errors 
     *   in particle momenta 
     *
     *  Essentially the algorithm is described in detail by Paul Avery in  
     *     http://www.phys.ufl.edu/~avery/fitting/lifetime.pdf
     *
     *  More precise variants (more iterations) use
     *   the abstract interface ILifetimeFitter 
     * 
     *  @see LoKi::Cuts::LT
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-19
     */
    class LifetimeDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::Vertices::VertexHolder 
    {
    public:
      // ======================================================================
      /// constructor 
      LifetimeDistance 
      ( const LHCb::VertexBase* vertex ) ;
      /// constructor 
      LifetimeDistance 
      ( const LoKi::Point3D&    vertex ) ;
      /// constructor 
      LifetimeDistance 
      ( const LoKi::Vertices::VertexHolder&       base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~LifetimeDistance () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifetimeDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default constructor is disabled
      LifetimeDistance() ; // No default constrtuctor
      // ======================================================================
    } ;
    // ========================================================================
  }  // end of namespace LoKi::Particles
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES1_H
// ============================================================================
