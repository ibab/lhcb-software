// $Id: Particles14.h,v 1.4 2007-07-23 17:35:45 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES14_H 
#define LOKI_PARTICLES14_H 1
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
 *  @date 2006-02-24 
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {   
    // ========================================================================
    /** @class DecayAngle
     *
     *  For partice, which has a some daughter particles, 
     *  it evaluates the cosine of the angle between daughter's 
     *  momentum and mother flight direction in the rest 
     *  system of mother particle. For 2-body decays it is just 
     *  a polarization angle of mother particle.
     *  
     *  It corresponds to a predefined variable <tt>LV0..</tt> from 
     *  H.Albrecht's KAL language used in ARGUS collaboaration
     *
     *  @see LoKi::Cuts::LV0 
     *  @see LoKi::Cuts::LV01
     *  @see LoKi::Cuts::LV02
     *  @see LoKi::Cuts::LV03
     *  @see LoKi::Cuts::LV04
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-11-02
     */
    class DecayAngle : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /** constructor with daughter index (starts from 1).
       *  E.g. for 2-body decays it could be 1 or 2 
       *  @param index of daughter particles
       */
      DecayAngle ( size_t child ) ;
      /// copy constructor 
      DecayAngle ( const DecayAngle& rigth) ;
      /// MANDATORY: virual destructor
      virtual ~DecayAngle(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DecayAngle* clone() const { return new DecayAngle(*this) ; }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      DecayAngle();
    private:
      // index of the daughter particle
      size_t m_child ;
    };
    // ========================================================================
  }  // end of namespace Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES12_H
// ============================================================================
