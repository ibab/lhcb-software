// $Id$
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
  // ==========================================================================
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
    class GAUDI_API CosineDirectionAngle 
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
    class GAUDI_API TrgPointingScore 
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
    /** @class PseudoRapidityFromVertex
     *  evaluator of the pseudorapidity obtained from the direction
     *  from the reference vertex/point to the particle decay vertex
     *  @see LoKi::Cuts::VETA
     *  @author Albert Puig
     *  @date   2015-03-03
     */
    class GAUDI_API PseudoRapidityFromVertex
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::Vertices::VertexHolder
    {
    public:
      // ======================================================================
      /// constructor form vertex
      PseudoRapidityFromVertex ( const LHCb::VertexBase* vertex ) ;
      /// constructor form the point
      PseudoRapidityFromVertex ( const LoKi::Point3D&    point  ) ;
      /// constructor from the holder
      PseudoRapidityFromVertex ( const LoKi::Vertices::VertexHolder& holder ) ;
      /// copy constructor
      PseudoRapidityFromVertex ( const PseudoRapidityFromVertex& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~PseudoRapidityFromVertex() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PseudoRapidityFromVertex* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const
      { return eta ( p ) ; }
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      result_type eta ( argument p ) const ;
      result_type phi ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private
      PseudoRapidityFromVertex();
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PhiFromVertex
     *  evaluator of the azimuth obtained from the direction
     *  from the reference vertex/point to the particle decay vertex
     *  @see LoKi::Cuts::VPHI
     *  @author Albert Puig
     *  @date   2015-03-03
     */
    class GAUDI_API PhiFromVertex : public PseudoRapidityFromVertex 
    {
    public:
      // ======================================================================
      /// constructor form vertex
      PhiFromVertex ( const LHCb::VertexBase* vertex ) ;
      /// constructor form the point
      PhiFromVertex ( const LoKi::Point3D&    point  ) ;
      /// constructor from the holder
      PhiFromVertex ( const LoKi::Vertices::VertexHolder& holder ) ;
      /// copy constructor
      PhiFromVertex ( const PhiFromVertex& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~PhiFromVertex() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PhiFromVertex* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const
      { return phi ( p ) ; }
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private
      PhiFromVertex();
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES16_H
// ============================================================================
