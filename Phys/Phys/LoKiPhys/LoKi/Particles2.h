// $Id: Particles2.h,v 1.8 2008-06-03 15:47:08 cattanem Exp $
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
    class TimeDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// constructor 
      TimeDistance 
      ( const LHCb::VertexBase*  vertex );
      /// constructor 
      TimeDistance 
      ( const LoKi::Point3D&     vertex );
      /// constructor
      TimeDistance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// constructor
      TimeDistance 
      ( const LoKi::Particles::VertexDistance& base ) ;
      /// constructor
      TimeDistance 
      ( const LoKi::Vertices::VertexDistance& base ) ;
      /// constructor
      TimeDistance 
      ( const TimeDistance & base ) ;
      /// MANDATORY: destructor
      virtual ~TimeDistance() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TimeDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type  time  ( argument p ) const ;
    private:
      /// default constructor is private 
      TimeDistance();
    private:
      LoKi::Particles::VertexDistance m_fun ;
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
    class TimeSignedDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// constructor 
      TimeSignedDistance 
      ( const LHCb::VertexBase*  vertex );
      /// constructor 
      TimeSignedDistance 
      ( const LoKi::Point3D&     vertex );
      /// constructor
      TimeSignedDistance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// constructor
      TimeSignedDistance 
      ( const LoKi::Vertices::VertexDistance& base ) ;
      /// constructor
      TimeSignedDistance 
      ( const LoKi::Particles::VertexSignedDistance& base ) ;
      /// constructor
      TimeSignedDistance 
      ( const TimeSignedDistance & base ) ;
      /// MANDATORY: destructor
      virtual ~TimeSignedDistance() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TimeSignedDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type  time  ( argument p ) const ;
    private:
      /// default constructor is private 
      TimeSignedDistance();
    private:
      LoKi::Particles::VertexSignedDistance m_fun ;
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
    class TimeDotDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// constructor 
      TimeDotDistance 
      ( const LHCb::VertexBase* vertex ) ;
      /// constructor 
      TimeDotDistance 
      ( const LoKi::Point3D& vertex ) ;
      /// constructor 
      TimeDotDistance 
      ( const LoKi::Vertices::VertexHolder&       base  ) ;
      /// constructor 
      TimeDotDistance 
      ( const LoKi::Particles::VertexDotDistance& base  ) ;
      /// copy constructor 
      TimeDotDistance 
      ( const TimeDotDistance& base  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TimeDotDistance();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TimeDotDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual computation 
      result_type time           ( argument p ) const ;
    private:
      /// default constructor is private
      TimeDotDistance();
    private:
      // the evaluator 
      LoKi::Particles::VertexDotDistance m_fun ;
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
    private:
      // The default constructor is disabled
      LifetimeDistance() ; ///< No default constrtuctor
    } ;
    // ========================================================================
  }  // end of namespace LoKi::Particles
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES1_H
// ============================================================================
