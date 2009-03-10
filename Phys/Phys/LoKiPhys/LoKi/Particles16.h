// $Id: Particles16.h,v 1.7 2009-03-10 22:49:57 spradlin Exp $
// ============================================================================
#ifndef LOKI_PARTICLES16_H 
#define LOKI_PARTICLES16_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/VertexHolder.h"
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
 *  @date 2006-03-20 
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {
    // ========================================================================
    /** @class CosineDirectionAngle
     *  evaluator of the angle between particle momentum   
     *  and the direction from the reference vertex/point 
     *  to the particle decay vertex
     *  @see LoKi::Cuts::DIRA
     *  @see LoKi::Cuts::DANG
     *  @author Vanya BELYAEV ibelyave@physics.syr.edu
     *  @date   2002-07-15
     */
    class CosineDirectionAngle 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::Vertices::VertexHolder
    {
    public:
      // ======================================================================
      /// constructor form vertex 
      CosineDirectionAngle ( const LHCb::VertexBase* vertex ) ;
      /// constructor form the point 
      CosineDirectionAngle ( const LoKi::Point3D&    point  ) ;
      /// constructor from the holder 
      CosineDirectionAngle ( const LoKi::Vertices::VertexHolder& holder ) ;
      /// copy constructor 
      CosineDirectionAngle ( const CosineDirectionAngle& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~CosineDirectionAngle() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CosineDirectionAngle* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const
      { return dira ( p ) ; }  
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      result_type dira ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      CosineDirectionAngle();
      // ======================================================================
    } ;
    // ========================================================================

    // ========================================================================
    /** @class TrgPointingScore
     *  Evaluator of the custom 'pointing angle' sometimes used in
     *  trigger applications:
     *  \f[
     *    a = (1 + \frac{\sum_{\mathrm{daug}} p_{i} \sin\theta_{i}}{p \sin\theta})^{-1}
     *  \f]
     *  where \f$p\f$ is the total momentum of the particle,
     *  the \f$p_{i}\f$ are the momenta of the particle descendents, and
     *  \f$\sin\theta_{(i)}\f$ are the angles between the particle momenta and
     *  the parent displacement from the specified vertex.
     *
     *  Not exactly a pointing angle. Rather, a normalized transverse momentum
     *  with respect to the flight displacement, transformed to lay within
     *  the range [0, 1].
     *
     *  @see LoKi::Cuts::TRGPOINTING
     *  @author patrick spradlin
     *  @date   10 March 2009
     */
    class TrgPointingScore 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::Vertices::VertexHolder
    {
    public:
      // ======================================================================
      /// constructor from vertex 
      TrgPointingScore ( const LHCb::VertexBase* vertex ) ;
      /// constructor from point 
      TrgPointingScore ( const LoKi::Point3D&    point  ) ;
      /// constructor from the holder 
      TrgPointingScore ( const LoKi::Vertices::VertexHolder& holder ) ;
      /// copy constructor 
      TrgPointingScore ( const TrgPointingScore& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~TrgPointingScore() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrgPointingScore* clone() const 
      { return new LoKi::Particles::TrgPointingScore(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const
      { return pointing ( p ) ; }  
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "TRGPOINTING" ; }
      // ======================================================================
    public:
      // ======================================================================
      result_type pointing ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      TrgPointingScore();
      // ======================================================================
    } ;
    // ========================================================================
  }  // end of namespace LoKi::Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES16_H
// ============================================================================
