// $Id$
// ============================================================================ 
#ifndef LOKI_PARTICLES46_H 
#define LOKI_PARTICLES46_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
/** @file LoKi/Particles46.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-02-04
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class MissingParticle
     *  Helper base class for missing particles 
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date   2015-02-05
     */
    class GAUDI_API MissingParticle : public virtual LoKi::AuxDesktopBase 
    {
    public:
      // =======================================================================
      /** constructor with all parameters 
       *  @param mother   nominal mass of mother  particle 
       *  @param missing  nominal mass of missing particle 
       *  @param onlygood count only good solutions 
       */
      MissingParticle
        ( const double mother            , 
          const double missing           , 
          const bool   only_good = false ) ;
      /** constructor with all parameters 
       *  @param mother   mother  particle 
       *  @param missing  missing particle 
       *  @param onlygood count only good solutions 
       */
      MissingParticle
        ( const std::string& mother      , 
          const std::string& missing     , 
          const bool   only_good = false ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MissingParticle () ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MissingParticle () ; // default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// get nominal mass of mother  particle 
      double mother    () const 
      { return m_m_mother  ; }          // get nominal mass of mother  particle 
      /// get nominal mass of missing particle 
      double missing   () const 
      { return m_m_missing ; }          // get nominal mass of missing particle 
      /// count only good solutions ?
      bool   only_good () const { return m_only_good ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// check that good masses are specified 
      bool goodMasses () const 
      { return 0 <= m_m_missing && m_m_missing < m_m_mother ; }
      /// get masses 
      void getMasses  () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// name of mother  particle 
      std::string    m_mother     ;
      /// name of missing particle 
      std::string    m_missing    ;
      /// mass of mother  particle 
      mutable double m_m_mother   ;
      /// mass of missing particle 
      mutable double m_m_missing  ;
      /// count onlygood solutions ? 
      bool           m_only_good  ;
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class MP_nSolutions 
     *  Number of solutions for missing particle
     *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
     *  @date   2015-02-05
     */
    class GAUDI_API MP_nSolutions 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public LoKi::Particles::MissingParticle 
    {
    public:
      // =======================================================================
      /** constructor with all parameters 
       *  @param mother   nominal mass of mother  particle 
       *  @param missing  nominal mass of missing particle 
       *  @param onlygood count only good solutions 
       */
      MP_nSolutions
        ( const double mother            , 
          const double missing           , 
          const bool   only_good = false ) ;
      /** constructor with all parameters 
       *  @param mother   mother  particle 
       *  @param missing  missing particle 
       *  @param onlygood count only good solutions 
       */
      MP_nSolutions
        ( const std::string& mother      , 
          const std::string& missing     , 
          const bool   only_good = false ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MP_nSolutions () ;
      /// MANDARORY: clone method ("virtual constructor")
      virtual  MP_nSolutions* clone() const ;
      /// MANDARORY: the only interesitng method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      // ======================================================================
    private:    
      // ======================================================================
      /// default constructor is disabled 
      MP_nSolutions () ; // default constructor is disabled
      // ======================================================================
    } ;
      // ========================================================================
    /** @class MissingDNeutrinoDOCA 
     *  The class is only for decays like B->DX, with D decaying into semi-leptonic decays
     *  For this decay topology, the PV, D vertex and that of X particle should be in the same plane
     *  This class is used to calulate the distance between (PV,DV) and X particles to quantify this requirement
     *  @author Wenbin Qian
     *  @date   2015-11-30
     */
    class GAUDI_API MissingDNeutrinoDOCA 
      : public virtual LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::Particles::MissingParticle
    {
    public:
      // =======================================================================
      /** constructor with all parameters 
       *  @param mother   nominal mass of mother  particle 
       *  @param missing  nominal mass of missing particle 
       *  @param onlygood count only good solutions        
       */
      MissingDNeutrinoDOCA( const double mother, const double missing, const bool only_good = false);
      MissingDNeutrinoDOCA( const std::string& mother, const std::string& missing, const bool only_good = false);
      // MANDATORY: clone method ('virtual constructor')
      virtual  MissingDNeutrinoDOCA* clone() const { return new MissingDNeutrinoDOCA(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// MANDATORY: virtual destructor
      virtual ~MissingDNeutrinoDOCA() ;
      // ======================================================================
    private:
      MissingDNeutrinoDOCA();
      // ======================================================================
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_PARTICLES46_H
// ============================================================================
