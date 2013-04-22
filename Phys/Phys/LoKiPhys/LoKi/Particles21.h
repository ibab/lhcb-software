// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES21_H 
#define LOKI_PARTICLES21_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles0.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class DeltaNominalMass
     *  simple function which evaluates the delta mass with respect 
     *  to the nominal mass 
     *  @see LoKi::Cuts::DPDGM 
     *  @see LoKi::Cuts::DPDGMASS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-23
     */
    class GAUDI_API DeltaNominalMass 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      DeltaNominalMass() { }
      /// MANDATORY: virtual destructor 
      virtual ~DeltaNominalMass () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaNominalMass* clone() const 
      { return new DeltaNominalMass ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return delta ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "DPDGM" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// the evalautor of the delta mass 
      double delta ( argument p ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class AbsDeltaNominalMass
     *  simple function whcih evalaute the absolute value of 
     *  delta mass with respect to the nominal mass 
     *  @see LoKi::Cuts::ADPDGM 
     *  @see LoKi::Cuts::ADPDGMASS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-23
     */
    class GAUDI_API AbsDeltaNominalMass : public DeltaNominalMass 
    {
    public:
      // ======================================================================
      /// Default Constructor
      AbsDeltaNominalMass() { }
      /// MANDATORY: virtual destructor 
      virtual ~AbsDeltaNominalMass () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsDeltaNominalMass* clone() const 
      { return new AbsDeltaNominalMass ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return ::fabs ( delta ( p ) ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ADPDGM" ; }     
      // ======================================================================
    public:
      // ======================================================================
      /// the evalautor of the delta mass 
      double adelta ( argument p ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class DeltaNominalMassChi2
     *  simple function whcih evaluate the che2 of delta mass with resepct 
     *  to the nominal mass 
     *  @see LoKi::Cuts::CHI2PDGM 
     *  @see LoKi::Cuts::CHI2PDGMASS 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-23
     */
    class DeltaNominalMassChi2 : public AbsDeltaNominalMass 
    {
    public:
      // ======================================================================
      /// Default Constructor
      DeltaNominalMassChi2() { }
      /// MANDATORY: virtual destructor 
      virtual ~DeltaNominalMassChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaNominalMassChi2* clone() const 
      { return new DeltaNominalMassChi2 ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const 
      { return chi2 ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "CHI2PDGM" ; }     
      // ======================================================================
    public:
      // ======================================================================
      /// the evalautor of the delta mass 
      double chi2 ( argument p ) const ;
      // ======================================================================
    };
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES21_H
// ============================================================================
