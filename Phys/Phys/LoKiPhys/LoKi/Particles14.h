// $Id$
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
  // ==========================================================================
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
    class GAUDI_API DecayAngle 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /** constructor with daughter index (starts from 1).
       *  E.g. for 2-body decays it could be 1 or 2 
       *  @param child index of daughter particles
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
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      DecayAngle();
      // ======================================================================
    private:
      // ======================================================================
      // index of the daughter particle
      size_t m_child ;
      // ======================================================================
    };
    // ========================================================================
    /** @class ArmenterosPodolanski 
     *  Simple evaluator of Armenteros-Podolandky variable 
     *
     *  The variable is defined as:
     *  \f[
     *  \mathbf{\alpha} = \dfrac
     *  { \mathrm{p}^{\mathrm{L},1} - \mathrm{p}^{\mathrm{L},1} }
     *  { \mathrm{p}^{\mathrm{L},1} + \mathrm{p}^{\mathrm{L},1} },
     *  \f]
     *  where 
     *   \f$ \mathrm{p}^{\mathrm{L},1}\f$ and 
     *   \f$ \mathrm{p}^{\mathrm{L},2}\f$ are longitudinal momentum
     *   components for the first and the seco ddaughter particles 
     *   with respect to the total momentum direction. 
     *
     *  Clearly this expression could be rewritten in an equivalent 
     *  form which however much more easier for calculation:
     *  \f[
     *  \mathbf{\alpha} = \dfrac
     *  { \vec{\mathbf{p}}_1^2 - \vec{\mathbf{p}}_2^2 }  
     *  { \left( \vec{\mathbf{p}}_1 + \vec{\mathbf{p}}_2 \right)^2 }  
     *  \f]
     *
     *  @see @LoKi::Kinematics::armenterosPodolanskiX 
     *  @see LoKi::Cuts::ARMENTEROSX
     *
     *  @attention instead of 
     *     2D \f$\left(\mathrm{p_T},\mathbf{\alpha}\right)\f$ diagram, 
     *     in the case of twobody decays at LHCb it is much better to 
     *     use 2D diagram \f$\left(\cos \theta, \mathrm{m} \right)\f$
     *     diagram, where \f$\cos\theta\f$-is the decay 
     *     angle,see the variable LV01, and \f$\mathrm{m}\f$ is an 
     *     invariant evalauted for some (fixed) mass prescription, 
     *     e.g. \f$\pi^+\pi^-\f$.  
     * 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@inkhef.nl
     *  @date 2008-09-21
     */
    class GAUDI_API ArmenterosPodolanski
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      ArmenterosPodolanski() { }
      /// MANDATORY: virtual destructor 
      virtual ~ArmenterosPodolanski () {}
      /// MANDATORY: virtual destructor 
      virtual  ArmenterosPodolanski* clone () const 
      { return new ArmenterosPodolanski ( *this ) ; }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "ARMENTEROS" ; }
      // ======================================================================
    } ;
    // ========================================================================
  } //                                               end of namespace Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES12_H
// ============================================================================
