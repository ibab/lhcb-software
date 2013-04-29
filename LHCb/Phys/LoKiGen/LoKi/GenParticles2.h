// $Id$
// ============================================================================
#ifndef LOKI_GENPARTICLES2_H 
#define LOKI_GENPARTICLES2_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
#include "Kernel/iNode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
#include "LoKi/GenTypes.h"
#include "LoKi/iTree.h"
#include "LoKi/IGenDecay.h"
#include "LoKi/GenChildSelector.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2006-01-23 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace GenParticles
  {
    // ========================================================================
    /** @class PolarizationAngle
     *  Simple functor that evaluates the polarization of daughter 
     *  (vector) particle in the rest system of mother particle 
     *
     *  It is defined for the following decay topologies: 
     *
     *   -  1 ->  ... +  1 + ... + 1 + ....   
     *   -  1 ->  ... +  ( 1 -> 1 + 1 ) +  ...  
     *
     *  @see LoKi::Kinematics::decayAngle 
     *  @see LoKi::Kinematics::cosDecayAngle 
     *  @see IP2VVMCPartAngleCalculator
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::GCOSPOL
     *  @see LoKi::MCParticles::PolarizationAngle
     *  @see LoKi::Cuts::MCCOSPOL
     *  @see LoKi::Particles::PolarizationAngle
     *  @see LoKi::Cuts::COSPOL
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-26
     */
    class GAUDI_API PolarizationAngle : public LoKi::GenTypes::GFunc  
    {
    public:
      // ======================================================================
      /// constructor from child selector  
      PolarizationAngle
      ( const LoKi::GenChild::Selector& daughter                    , 
        const LoKi::GenChild::Selector& parent                      , 
        const bool                      mother    = true            ) ;
      /// constructor from two trees 
      PolarizationAngle
      ( const Decays::IGenDecay::iTree& daughter                    , 
        const Decays::IGenDecay::iTree& parent                      , 
        const bool                     mother    = true            ) ;
      /// constructor from two nodes
      PolarizationAngle
      ( const Decays::iNode&           daughter                    , 
        const Decays::iNode&           parent                      , 
        const bool                     mother    = true            ) ;
      /// constructor from two functions 
      PolarizationAngle
      ( const LoKi::GenTypes::GCuts&   daughter                    , 
        const LoKi::GenTypes::GCuts&   parent                      , 
        const bool                     mother    = true            ) ;
      /// constructor from two trees 
      PolarizationAngle
      ( const std::string&             daughter                     , 
        const std::string&             parent                       ,      
        const bool                     mother    = true             , 
        const std::string&             factory   = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~PolarizationAngle () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PolarizationAngle* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PolarizationAngle () ;             // the default constructor is disabled
      // ======================================================================
    protected:
      // ======================================================================
      /// get the proper decay components  
      StatusCode getComponents12
      ( const HepMC::GenParticle*                p   , 
        Decays::IGenDecay::iTree::Collection&    vct ) const ;
      /// valid trees?
      bool valid  () const { return m_first.valid  () && m_second.valid  () ; }      
      // ======================================================================
    public:      
      // ======================================================================
      /// the first child 
      const LoKi::GenChild::Selector& child1 () const { return m_first  ; }
      /// the second tree 
      const LoKi::GenChild::Selector& child2 () const { return m_second ; }
      // ======================================================================
    private:
      // ======================================================================
      /// rule to find the first  particle 
      LoKi::GenChild::Selector m_first   ;  //  rule to find the first particle 
      /// rule to find the second pa rticle 
      LoKi::GenChild::Selector m_second  ;  // rule to find the second particle 
      /// use (dauther-parent) or two-daughters rule ?
      bool                    m_mother   ; 
      // ======================================================================
    } ;      
    // ========================================================================
    /** @class SinChi 
     *  Simple functor that evaluates the (sine) angle between the 
     *  decay planes of daughter particles 
     *
     *  @see LoKi::Kinematics::sinDecayAngleChi 
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see IP2VVMCPartAngleCalculator
     *  @see LoKi::Cuts::GSINCHI
     *  @see LoKi::Cuts::MCSINCHI
     *  @see LoKi::Cuts::SINCHI
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  The angle is  calculated using the explicit 
     *   Lorentz-invariant expression:
     *   \f[ 
     *   \sin \chi = 
     *   \frac  { 
     *   \epsilon_{\mu\nu\lambda\delta}
     *   L_D^{\mu}L_H^{\nu}H^{\lambda}M^{\delta} }
     *   { \sqrt{ 
     *   \left[ -L_D^2 \right]\left[ -L_H^2 \right] 
     *   \left[ \left( H\ cdot M\right)^2-H^2M^2 \right] 
     *   }} = \frac { 
     *   \epsilon_{\mu\nu\lambda\delta}
     *   d_1^{\mu}d_2^{\nu}h_1^{\lambda}h_2^{\delta}
     *   \left( \left( D \cdot H \right)^2 - D^2H^2 \right) }
     *   { \sqrt{ 
           *   \left[ -L_D^2 \right]\left[ -L_H^2    \right] 
     *   \left[ \left(H\cdot M\right)^2-H^2M^2 \right] 
           *   }},
     *  \f] 
     *  where "4-normales" are defined as:
     *  \f$
     *  L_D^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}\left(h_1+h_2\right)^{\kappa} 
     *  \f$, 
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$
     *  and   \f$ D = d_1 + d_2 \f$, 
     *        \f$ H = h_1 + h_2 \f$, 
     *        \f$ M = D + H = d_1 + d_2 + h_1+h_2 \f$. 
     *
     *  The sign for <c>sin</c> is set according to 
     *  Thomas Blake's code from
     *  P2VVAngleCalculator tool
     *  @see P2VVAngleCalculator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-01-28
     */
    // ========================================================================
    class GAUDI_API SinChi : public PolarizationAngle 
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      SinChi ( const LoKi::GenChild::Selector&   particle1        ,  
               const LoKi::GenChild::Selector&   particle2        , 
               const LoKi::GenChild::Selector&   particle3        , 
               const LoKi::GenChild::Selector&   particle4        ) ;
      /// constructor form the trees 
      SinChi ( const Decays::IGenDecay::iTree& particle1        ,  
               const Decays::IGenDecay::iTree& particle2        , 
               const Decays::IGenDecay::iTree& particle3        ,  
               const Decays::IGenDecay::iTree& particle4        ) ;
      /// constructor form the nodes 
      SinChi ( const Decays::iNode&           particle1        ,  
               const Decays::iNode&           particle2        , 
               const Decays::iNode&           particle3        , 
               const Decays::iNode&           particle4        ) ;
      /// constructor form the cuts
      SinChi ( const LoKi::GenTypes::GCuts&   particle1        ,  
               const LoKi::GenTypes::GCuts&   particle2        , 
               const LoKi::GenTypes::GCuts&   particle3        , 
               const LoKi::GenTypes::GCuts&   particle4        ) ;
      /// constructor from the decay descriptors
      SinChi ( const std::string& particle1                    ,  
               const std::string& particle2                    , 
               const std::string& particle3                    , 
               const std::string& particle4                    , 
               const std::string& factory   = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~SinChi() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SinChi* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SinChi () ;                        // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// get the proper decay components  
      StatusCode getComponents34
      ( const HepMC::GenParticle*                p   , 
        Decays::IGenDecay::iTree::Collection&    vct ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the proper decay components  
      StatusCode getComponents  
      ( const HepMC::GenParticle*               p   , 
        Decays::IGenDecay::iTree::Collection&    vct ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const LoKi::GenChild::Selector& child3  () const { return m_tree3   ; }
      const LoKi::GenChild::Selector& child4  () const { return m_tree4   ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the tree to find the third  particle 
      LoKi::GenChild::Selector m_tree3 ; // the tree to find the third  particle 
      /// the tree to find the fourth particle 
      LoKi::GenChild::Selector m_tree4 ; // the tree to find the fourth particle 
      // ======================================================================
    } ;      
    // ========================================================================
    /** @class CosChi 
     *  Simple functor that evaluates the (cosine) angle between the 
     *  decay planes of daughter particles 
     *
     *  @see LoKi::Kinematics::cosDecayAngleChi 
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::MCCOSCHI
     *
     *  The angle is evaluated using the explicit 
     *  Lorenzt-invariant expression:
     *  \f[ 
     *  \cos \chi =      
     *   - \frac{ L_D^{\mu} L_H^{\mu} }
     *     { \sqrt{ \left[ -L_D^2 \right]\left[ -L_H^2 \right] }},
     &   =
     *   - \frac{ 
           *     \epsilon_{ijkl}d_1^{j}d_2^{k}\left(h_1+h_2\right)^l
     *     \epsilon_{imnp}h_1^{m}h_2^{n}\left(d_1+d_2\right)^p }
     *     { \sqrt{ \left[ -L_D^2 \right]\left[ -L_H^2 \right] }},
     *  \f$ 
     *  where "4-normales" are defined as:
     *  \f$ 
     *   L_D^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}\left(h_1+h_2\right)^{\kappa} 
     *  \f$ 
     *   and 
     *  \f$ 
     *   L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *                h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f].
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-01-28
     */
    // ========================================================================
    class GAUDI_API CosChi : public SinChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosChi ( const LoKi::GenChild::Selector& particle1        ,  
               const LoKi::GenChild::Selector& particle2        , 
               const LoKi::GenChild::Selector& particle3        , 
               const LoKi::GenChild::Selector& particle4        ) ;
      /// constructor form the trees 
      CosChi ( const Decays::IGenDecay::iTree& particle1        ,  
               const Decays::IGenDecay::iTree& particle2        , 
               const Decays::IGenDecay::iTree& particle3        , 
               const Decays::IGenDecay::iTree& particle4        ) ;
      /// constructor form the noeds
      CosChi ( const Decays::iNode&           particle1        ,  
               const Decays::iNode&           particle2        , 
               const Decays::iNode&           particle3        , 
               const Decays::iNode&           particle4        ) ;
      /// constructor form the cuts
      CosChi ( const LoKi::GenTypes::GCuts&   particle1        ,  
               const LoKi::GenTypes::GCuts&   particle2        , 
               const LoKi::GenTypes::GCuts&   particle3        , 
               const LoKi::GenTypes::GCuts&   particle4        ) ;
      /// constructor from the decay descriptors
      CosChi ( const std::string&   particle1 ,  
               const std::string&   particle2 , 
               const std::string&   particle3 , 
               const std::string&   particle4 , 
               const std::string&   factory = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~CosChi() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CosChi* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      CosChi () ;                        // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AngleChi 
     *  Simple functor that evaluates the angle between the 
     *  decay planes of daughter particles 
     *
     *  @see LoKi::Kinematics::decayAngleChi 
     *  @see LoKi::Kinematics::sinDecayAngleChi 
     *  @see LoKi::Kinematics::cosDecayAngleChi 
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::GANGLECHI
     *  @see LoKi::Cuts::ANGLECHI
     *  @see LoKi::Cuts::MCANGLECHI
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2011-01-28
     */
    class GAUDI_API AngleChi : public CosChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      AngleChi ( const LoKi::GenChild::Selector& particle1        ,  
                 const LoKi::GenChild::Selector& particle2        , 
                 const LoKi::GenChild::Selector& particle3        , 
                 const LoKi::GenChild::Selector& particle4        ) ;
      /// constructor from the trees 
      AngleChi ( const Decays::IGenDecay::iTree& particle1        ,  
                 const Decays::IGenDecay::iTree& particle2        , 
                 const Decays::IGenDecay::iTree& particle3        , 
                 const Decays::IGenDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      AngleChi ( const Decays::iNode&           particle1        ,  
                 const Decays::iNode&           particle2        , 
                 const Decays::iNode&           particle3        , 
                 const Decays::iNode&           particle4        ) ;
      /// constructor form the cuts  
      AngleChi ( const LoKi::GenTypes::GCuts&   particle1        ,  
                 const LoKi::GenTypes::GCuts&   particle2        , 
                 const LoKi::GenTypes::GCuts&   particle3        , 
                 const LoKi::GenTypes::GCuts&   particle4        ) ;
      /// constructor from the decay descriptors
      AngleChi ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~AngleChi() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AngleChi* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AngleChi () ;                      // the default constructor is disabled 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class CosThetaTr
     *  Simple functor that evaluates the cosine of the 
     *  transverse angele theta, \f$\cos \theta_{\mathrm{tr}}\f$, 
     *
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression:
     *  \f[
     *   \cos \theta_{\mathrm{tr}} = 
     *    \frac{ \epsilon_{\mu\nu\lambda\kappa}
     *          d_1^{\mu}h_1^{\nu}h_2^{\lambda}L_H^{\kappa} }
     *    {
     *     \sqrt{  \left( d_1 \cdot D  \right) / D^2 - d_1^2 } 
     *     \sqrt{  - L_H^2 }      
     *    },     
     *  \f]
     * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$, and \f$ D = d_1 + d_2 \f$. 
     *
     * 
     *  @see LoKi::Kinematics::cosThetaTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::GCOSTHETATR 
     *  @see LoKi::Cuts::MCCOSTHETATR 
     *  @see LoKi::Cuts::COSTHETATR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    class GAUDI_API CosThetaTr : public AngleChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosThetaTr ( const LoKi::GenChild::Selector& particle1        ,  
                   const LoKi::GenChild::Selector& particle2        , 
                   const LoKi::GenChild::Selector& particle3        , 
                   const LoKi::GenChild::Selector& particle4        ) ;
      /// constructor from the trees 
      CosThetaTr ( const Decays::IGenDecay::iTree& particle1        ,  
                   const Decays::IGenDecay::iTree& particle2        , 
                   const Decays::IGenDecay::iTree& particle3        , 
                   const Decays::IGenDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      CosThetaTr ( const Decays::iNode&           particle1        ,  
                   const Decays::iNode&           particle2        , 
                   const Decays::iNode&           particle3        , 
                   const Decays::iNode&           particle4        ) ;
      /// constructor form the cuts  
      CosThetaTr ( const LoKi::GenTypes::GCuts&   particle1        ,  
                   const LoKi::GenTypes::GCuts&   particle2        , 
                   const LoKi::GenTypes::GCuts&   particle3        , 
                   const LoKi::GenTypes::GCuts&   particle4        ) ;
      /// constructor from the decay descriptors
      CosThetaTr ( const std::string& particle1 ,  
                   const std::string& particle2 , 
                   const std::string& particle3 , 
                   const std::string& particle4 , 
                   const std::string& factory = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~CosThetaTr () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CosThetaTr* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      CosThetaTr () ;                    // the default constructor is disabled 
      // ======================================================================
    } ; 
    // ========================================================================
    /** @class SinPhiTr
     *  Simple functor that evaluates the sine of the 
     *  transverse angel phi, \f$\sin \phi_{\mathrm{tr}}\f$, 
     *
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression:
     *  \f[
     *   \sin \phi_{\mathrm{tr}} = 
     *    - frac { 
     *      \epsilon_{\mu\nu\lambda\kappa}
     *       d_1^{\mu}L_H^{\mu}D^{\lambda}H^{\kappa}
     *     }{
     *     \sqrt{ -L^2 }
     *     \sqrt{  D^2 }
     *     \sqrt{ \left( q \cdot D  \right) / D^2 - q^2 } 
     *     \sqrt{ \left( M \cdot D  \right) / D^2 - M^2 } 
     *     }
     *  \f] 
     * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$,
     * \f$ D = d_1 + d_2 \f$,  
     * \f$ H = h_1 + h_2 \f$,  
     * \f$ M = D+ H \f$ and ``in-plane'' 4-vector \f$q\f$ is defined as 
     *  \f$  q = d_1 - \frac{ d_1 \cdot L_H}{L_H^2}L_H \f$.
     *
     *  @see LoKi::Kinematics::sinPhiTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::GSINPHITR 
     *  @see LoKi::Cuts::MCSINPHITR 
     *  @see LoKi::Cuts::SINPHITR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-20
     */
    class GAUDI_API SinPhiTr : public CosThetaTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      SinPhiTr ( const LoKi::GenChild::Selector& particle1        ,  
                 const LoKi::GenChild::Selector& particle2        , 
                 const LoKi::GenChild::Selector& particle3        , 
                 const LoKi::GenChild::Selector& particle4        ) ;
      /// constructor from the trees 
      SinPhiTr ( const Decays::IGenDecay::iTree& particle1        ,  
                 const Decays::IGenDecay::iTree& particle2        , 
                 const Decays::IGenDecay::iTree& particle3        , 
                 const Decays::IGenDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      SinPhiTr ( const Decays::iNode&           particle1        ,  
                 const Decays::iNode&           particle2        , 
                 const Decays::iNode&           particle3        , 
                 const Decays::iNode&           particle4        ) ;
      /// constructor from the cuts  
      SinPhiTr ( const LoKi::GenTypes::GCuts&   particle1        ,  
                 const LoKi::GenTypes::GCuts&   particle2        , 
                 const LoKi::GenTypes::GCuts&   particle3        , 
                 const LoKi::GenTypes::GCuts&   particle4        ) ;
      /// constructor from the decay descriptors
      SinPhiTr ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~SinPhiTr () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SinPhiTr* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SinPhiTr () ;                      // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CosPhiTr
     *  Simple functor that evaluates the cosine of the 
     *  transverse angel phi, \f$\cos \phi_{\mathrm{tr}}\f$, 
     *
     *  @see LoKi::Kinematics::cosPhiTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::GCOSPHITR 
     *  @see LoKi::Cuts::MCCOSPHITR 
     *  @see LoKi::Cuts::COSPHITR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    class GAUDI_API CosPhiTr : public SinPhiTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosPhiTr ( const LoKi::GenChild::Selector& particle1        ,  
                 const LoKi::GenChild::Selector& particle2        , 
                 const LoKi::GenChild::Selector& particle3        , 
                 const LoKi::GenChild::Selector& particle4        ) ;
      /// constructor from the trees 
      CosPhiTr ( const Decays::IGenDecay::iTree& particle1        ,  
                 const Decays::IGenDecay::iTree& particle2        , 
                 const Decays::IGenDecay::iTree& particle3        , 
                 const Decays::IGenDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      CosPhiTr ( const Decays::iNode&           particle1        ,  
                 const Decays::iNode&           particle2        , 
                 const Decays::iNode&           particle3        , 
                 const Decays::iNode&           particle4        ) ;
      /// constructor form the cuts  
      CosPhiTr ( const LoKi::GenTypes::GCuts&   particle1        ,  
                 const LoKi::GenTypes::GCuts&   particle2        , 
                 const LoKi::GenTypes::GCuts&   particle3        , 
                 const LoKi::GenTypes::GCuts&   particle4        ) ;
      /// constructor from the decay descriptors
      CosPhiTr ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~CosPhiTr () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CosPhiTr* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      CosPhiTr () ;                      // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AnglePhiTr
     *  Simple functor that evaluates the 
     *  transverse angel phi, \f$\phi_{\mathrm{tr}}\f$, 
     *
     *  @see LoKi::Kinematics::anglePhiTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::GANGLEPHITR 
     *  @see LoKi::Cuts::MCANGLEPHITR 
     *  @see LoKi::Cuts::ANGLEPHITR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    class GAUDI_API AnglePhiTr : public CosPhiTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      AnglePhiTr ( const LoKi::GenChild::Selector& particle1        ,  
                   const LoKi::GenChild::Selector& particle2        , 
                   const LoKi::GenChild::Selector& particle3        , 
                   const LoKi::GenChild::Selector& particle4        ) ;
      /// constructor from the trees 
      AnglePhiTr ( const Decays::IGenDecay::iTree& particle1        ,  
                   const Decays::IGenDecay::iTree& particle2        , 
                   const Decays::IGenDecay::iTree& particle3        , 
                   const Decays::IGenDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      AnglePhiTr ( const Decays::iNode&           particle1        ,  
                   const Decays::iNode&           particle2        , 
                   const Decays::iNode&           particle3        , 
                   const Decays::iNode&           particle4        ) ;
      /// constructor form the cuts  
      AnglePhiTr ( const LoKi::GenTypes::GCuts&   particle1        ,  
                   const LoKi::GenTypes::GCuts&   particle2        , 
                   const LoKi::GenTypes::GCuts&   particle3        , 
                   const LoKi::GenTypes::GCuts&   particle4        ) ;
      /// constructor from the decay descriptors
      AnglePhiTr ( const std::string& particle1 ,  
                   const std::string& particle2 , 
                   const std::string& particle3 , 
                   const std::string& particle4 , 
                   const std::string& factory = "LoKi::GenDecay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~AnglePhiTr () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AnglePhiTr* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AnglePhiTr () ;                    // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                  The end of namespace LoKi::GenParticles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef GCOSPOL
     * 
     *   Evaluate polarization of  J/psi from B-decays 
     *   @code
     * 
     *   const GCOSPOL cosPol = GCOSPOL ( 
     *             "Xb ==>  ( J/psi(1S) => ^mu+ mu- ) ... " , 
     *             "Xb ==> ^( J/psi(1S) =>  mu+ mu- ) ... " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = cosPol ( mcp ) ;
     *
     *   @endcode 
     *  
     *  @see LoKi::Kinematics::decayAngle
     *  @see LoKi::GenParticles::PolarizationAngle
     *  @see LoKi::MCParticles::PolarizationAngle
     *  @see LoKi::Particles::PolarizationAngle
     *  @see LoKi::Cuts::MCCOSPOL
     *  @see LoKi::Cuts::COSPOL
     *  @author Vaanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-26
     */
    typedef LoKi::GenParticles::PolarizationAngle                     GCOSPOL ;
    // ========================================================================
    /** @typedef GSINCHI 
     *  evaluator of sine of the angle chi 
     *
     *   @code
     * 
     *   const GSINCHI sinChi = GSINCHI ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = sinChi ( mcp ) ;
     *
     *   @endcode 
     *
     *  The angle is  calculated using the explicit 
     *   Lorentz-invariant expression:
     *  
     *  \f$ 
     *   \sin \chi = 
     *   \frac  { 
     *   \epsilon_{\mu\nu\lambda\delta}
     *   L_D^{\mu}L_H^{\nu}H^{\lambda}M^{\delta} }
     *   { \sqrt{ 
     *   \left[ -L_D^2 \right]\left[ -L_H^2 \right] 
     *   \left[ \left( H\ cdot M\right)^2-H^2M^2 \right] 
     *   }} = \frac { 
     *   \epsilon_{\mu\nu\lambda\delta}
     *   d_1^{\mu}d_2^{\nu}h_1^{\lambda}h_2^{\delta}
     *   \left( \left( D \cdot H \right)^2 - D^2H^2 \right) }
     *   { \sqrt{ 
     *   \left[ -L_D^2 \right]\left[ -L_H^2    \right] 
     *   \left[ \left(H\cdot M\right)^2-H^2M^2 \right] 
     *   }},
     * \f$ 
     *  where "4-normales" are defined as:
     *  \f$
     *  L_D^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}\left(h_1+h_2\right)^{\kappa} 
     *  \f$, 
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$
     *  and   \f$ D = d_1 + d_2 \f$, 
     *        \f$ H = h_1 + h_2 \f$, 
     *        \f$ M = D + H = d_1 + d_2 + h_1+h_2 \f$. 
     *
     *  The sign for <c>sin</c> is set according to 
     *  Thomas Blake's code from
     *  P2VVAngleCalculator tool
     *  @see P2VVAngleCalculator 
     *
     *  @see LoKi::Kinematics::sinDecayAngelChi     
     *  @see LoKi::GenParticles::SinChi
     *  @see LoKi::MCParticles::SinChi
     *  @see LoKi::Particles::SinChi
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    typedef LoKi::GenParticles::SinChi                                GSINCHI ;
    // ======================================================================== 
    /** @typedef GCOSCHI 
     *  evaluator of cosine of the angle chi 
     *
     *
     *   @code
     * 
     *   const GCOSCHI cosChi = GCOSCHI ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = cosChi ( mcp ) ;
     *
     *   @endcode 
     *
     *  The angle is evaluated using the explicit 
     *  Lorenzt-invariant expression:
     *  \f[
     *  \cos \chi =      
     *   - \frac{ L_D^{\mu} L_H^{\mu} }
     *     { \sqrt{ \left[ -L_D^2 \right]\left[ -L_H^2 \right] }},
     *   =
     *   - \frac{ 
     *     \epsilon_{ijkl}d_1^{j}d_2^{k}\left(h_1+h_2\right)^l
     *     \epsilon_{imnp}h_1^{m}h_2^{n}\left(d_1+d_2\right)^p }
     *     { \sqrt{ \left[ -L_D^2 \right]\left[ -L_H^2 \right] }},
     *  \f] 
     *  where "4-normales" are defined as:
     *  \f$ 
     *   L_D^{\mu} = \epsilon_{\mu\nu\lambda\kappa}
     *                d_1^{\nu}d_2^{\lambda}\left(h_1+h_2\right)^{\kappa} 
     *  \f$ 
     *   and 
     *  \f$ 
     *   L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *                h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *   \f$.
     *
     *  @see LoKi::GenParticles::CosChi
     *  @see LoKi::MCParticles::CosChi
     *  @see LoKi::Particles::CosChi
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    typedef LoKi::GenParticles::CosChi                                GCOSCHI ;
    // ========================================================================
    /** @typedef GANGLECHI 
     *  evaluator of the angle chi 
     *
     *   @code
     * 
     *   const GANGLECHI chi = GANGLECHI ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = chi ( mcp ) ;
     *
     *   @endcode 
     *
     *  @see LoKi::Kinematics::decayAngleChi
     *  @see LoKi::GenParticles::AngleChi
     *  @see LoKi::MCParticles::AngleChi
     *  @see LoKi::Particles::AngleChi
     *  @see LoKi::Cuts::MCSINCHI
     *  @see LoKi::Cuts::MCCOSCHI
     *  @see LoKi::Cuts::GSINCHI
     *  @see LoKi::Cuts::GCOSCHI
     *  @see LoKi::Cuts::SINCHI
     *  @see LoKi::Cuts::COSCHI
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    typedef LoKi::GenParticles::AngleChi                            GANGLECHI ;
    // ========================================================================
    /** @typedef GCOSTHETATR 
     *  Simple evaluator of \f$ \cos \theta_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const GCOSTHETATR cosThetaTr = GCOSTHETATR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = cosThetaTr ( mcp ) ;
     *
     *   @endcode 
     *
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression:
     *  \f[
     *   \cos \theta_{\mathrm{tr}} = 
     *    \frac{ \epsilon_{\mu\nu\lambda\kappa}
     *          d_1^{\mu}h_1^{\nu}h_2^{\lambda}L_H^{\kappa} }
     *    {
     *     \sqrt{  \left( d_1 \cdot D  \right) / D^2 - d_1^2 } 
     *     \sqrt{  - L_H^2 }      
     *    },     
     *  \f]
     * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$, and \f$ D = d_1 + d_2 \f$. 
     *
     *  @see LoKi::MCParticles::CosThetaTr
     *  @see LoKi::Kinematics::cosThetaTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::GenParticles::CosThetaTr                        GCOSTHETATR ;
    // ========================================================================
    /** @typedef GSINPHITR 
     *  Simple evaluator of \f$ \sin \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const GSINPHITR sinPhiTr = GSINPHITR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = sinPhiTr ( mcp ) ;
     *
     *   @endcode 
     *
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression:
     *  \f[
     *   \sin \phi_{\mathrm{tr}} = 
     *    - frac { 
     *      \epsilon_{\mu\nu\lambda\kappa}
     *       d_1^{\mu}L_H^{\mu}D^{\lambda}H^{\kappa}
     *     }{
     *     \sqrt{ -L^2 }
     *     \sqrt{  D^2 }
     *     \sqrt{ \left( q \cdot D  \right) / D^2 - q^2 } 
     *     \sqrt{ \left( M \cdot D  \right) / D^2 - M^2 } 
     *     }
     *  \f] 
     * where 4-normal \f$ L_H^{\mu}\f$ is defined as  
     *  \f$ 
     *  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$,
     * \f$ D = d_1 + d_2 \f$,  
     * \f$ H = h_1 + h_2 \f$,  
     * \f$ M = D+ H \f$ and ``in-plane'' 4-vector \f$q\f$ is defined as 
     *  \f$  q = d_1 - \frac{ d_1 \cdot L_H}{L_H^2}L_H \f$.
     *
     *  @see LoKi::GenParticles::SinPhiTr
     *  @see LoKi::MCParticles::SinPhiTr
     *  @see LoKi::Particles::SinPhiTr
     *  @see LoKi::Kinematics::sinPhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    typedef LoKi::GenParticles::SinPhiTr                            GSINPHITR ;
    // ========================================================================
    /** @typedef GCOSPHITR 
     *  Simple evaluator of \f$ \cos \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const GCOSPHITR cosPhiTr = GCOSPHITR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = cosPhiTr ( mcp ) ;
     *
     *   @endcode 
     *
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expression as angle between the ``in-plane'' vector \f$q\f$, and 
     *  vector \f$H\f$ in rest frame of \f$D\f$, where 
     *  \f$  q = d_1 - \frac{ d_1 \cdot L_H}{L_H^2}L_H \f$, 
     *  the ``4-normal'' is defiend as 
     *  \f$  L_H^{\mu} = \epsilon_{\mu\lambda\delta\rho}
     *  h_1^{\lambda}h_2^{\delta}\left(d_1+d_2\right)^{\rho} 
     *  \f$, \f$ D = d_1 + d_2 \f$, \f$ H = h_1 + h_2 \f$.
     *
     *  @see LoKi::GenParticles::CosPhiTr
     *  @see LoKi::MCParticles::CosPhiTr
     *  @see LoKi::Particles::CosPhiTr
     *  @see LoKi::Kinematics::cosPhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::GenParticles::CosPhiTr                            GCOSPHITR ;
    // ========================================================================
    /** @typedef GANGLEPHITR 
     *  Simple evaluator of \f$ \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const GANGLEPHITR phiTr = GANGLEPHITR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const HepMC::GenParticle* mcp = ... ;
     *  
     *   const double value = phiTr ( mcp ) ;
     *
     *   @endcode 
     *
     *  @see LoKi::GenParticles::AnglePhiTr
     *  @see LoKi::MCParticles::AnglePhiTr
     *  @see LoKi::Particles::AnglePhiTr
     *  @see LoKi::Kinematics::anglePhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::GenParticles::AnglePhiTr                        GANGLEPHITR ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                  The end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_GENPARTICLES2_H
// ============================================================================
