// $Id: Particles7.h,v 1.5 2007-07-23 17:35:47 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES7_H 
#define LOKI_PARTICLES7_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
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
 *  @date 2006-02-21 
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {
    // ========================================================================
    /** @class VFunAsPFun
     *  The simple adapter of "Vertex function"
     *  as "Particle function"
     *
     *  e.g. return z-position of particle endVertex:
     *
     *  @code 
     *
     *  const LHCb::Particle* particle = ... ;
     *  
     *  // create teh function
     *  Fun vz = VFuncAsFun( VZ ) ;
     *
     *  // use it 
     *  double z  = vz ( particle ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VFASPF 
     *  @see LoKi::Cuts::VFUNASPFUN
     *  @see LHCb::Particle
     *  @see LHCb::Vertex
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-02-27
     */
    class VFunAsPFun : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /** constructor form "Vertex function"
       *  
       *  @code 
       *
       *  Fun fun = VFunAsPFun( VCHI2 ) ;
       *  const Particle* particle = ... ;
       *
       *  // evaluat evertex chi2 
       *  double vxchi2 = fun( particle ) ;
       *
       *  @endcode 
       *
       *  @param vfun reference to "Vertex function"
       *  @param bad the error valeu to be retirned for invalid particle 
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date   2004-02-27
       */
      VFunAsPFun           
      ( const LoKi::Types::VFunc& vfun           , 
        const double              bad  = -1000.0 ) ;
      /// copy constructor 
      VFunAsPFun ( const VFunAsPFun&  fun ) ;
      /// virtual destructor
      virtual ~VFunAsPFun();
      /// clone: virtual constructor ;
      virtual VFunAsPFun* clone() const ;
      /// the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default constructor is private 
      VFunAsPFun() ;
      // assigenement operator 
      VFunAsPFun& operator=( const VFunAsPFun&  fun );
    private:
      LoKi::Types::VFun m_vfun ;
      double            m_bad  ;
    };
    // ========================================================================      
  }  // end of namespace LoKi::Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES7_H
// ============================================================================
