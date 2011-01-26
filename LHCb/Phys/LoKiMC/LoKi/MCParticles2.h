// $Id: MCParticles2.h,v 1.2 2010-06-02 15:44:17 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCPARTICLES2_H 
#define LOKI_MCPARTICLES2_H 1
// ============================================================================
// Include files
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/iNode.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/MCChildSelector.h"
#include "LoKi/MCTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace MCParticles 
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
     *  @see LoKi::Cuts::MCCOSPOL
     *  @see LoKi::Particles::PolarizationAngle
     *  @see LoKi::Cuts::COSPOL
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    class GAUDI_API PolarizationAngle
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from child selector  
      PolarizationAngle
      ( const LoKi::MCChild::Selector& daughter                    , 
        const LoKi::MCChild::Selector& parent                      , 
        const bool                     mother    = true            ) ;
      /// constructor from two trees 
      PolarizationAngle
      ( const Decays::IMCDecay::iTree& daughter                    , 
        const Decays::IMCDecay::iTree& parent                      , 
        const bool                     mother    = true            ) ;
      /// constructor from two nodes
      PolarizationAngle
      ( const Decays::iNode&           daughter                    , 
        const Decays::iNode&           parent                      , 
        const bool                     mother    = true            ,
        const bool                     decayOnly = true            ) ;
      /// constructor from two functions 
      PolarizationAngle
      ( const LoKi::MCTypes::MCCuts&   daughter                    , 
        const LoKi::MCTypes::MCCuts&   parent                      , 
        const bool                     mother    = true            ,
        const bool                     decayOnly = true            ) ;
      /// constructor from two trees 
      PolarizationAngle
      ( const std::string&             daughter                    , 
        const std::string&             parent                      ,      
        const bool                     mother    = true            , 
        const std::string&             factory   = "LoKi::MCDecay" ) ;
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
      ( const LHCb::MCParticle*                 p   , 
        Decays::IMCDecay::iTree::Collection&    vct ) const ;
      /// valid trees?
      bool valid  () const { return m_first.valid  () && m_second.valid  () ; }      
      // ======================================================================
    public:      
      // ======================================================================
      /// the first child 
      const LoKi::MCChild::Selector& child1 () const { return m_first  ; }
      /// the second tree 
      const LoKi::MCChild::Selector& child2 () const { return m_second ; }
      // ======================================================================
    private:
      // ======================================================================
      /// rule to find the first  particle 
      LoKi::MCChild::Selector m_first   ;   //  rule to find the first particle 
      /// rule to find the second pa rticle 
      LoKi::MCChild::Selector m_second  ;   // rule to find the second particle 
      /// use (dauther-parent) or two-daughters rule ?
      bool                    m_mother  ; 
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
     *  @see LoKi::Cuts::SINCHI
     *  @see LoKi::Cuts::MCSINCHI
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
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    class GAUDI_API SinChi : public PolarizationAngle 
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      SinChi ( const LoKi::MCChild::Selector& particle1        ,  
               const LoKi::MCChild::Selector& particle2        , 
               const LoKi::MCChild::Selector& particle3        , 
               const LoKi::MCChild::Selector& particle4        ) ;
      /// constructor form the trees 
      SinChi ( const Decays::IMCDecay::iTree& particle1        ,  
               const Decays::IMCDecay::iTree& particle2        , 
               const Decays::IMCDecay::iTree& particle3        ,  
               const Decays::IMCDecay::iTree& particle4        ) ;
      /// constructor form the nodes 
      SinChi ( const Decays::iNode&           particle1        ,  
               const Decays::iNode&           particle2        , 
               const Decays::iNode&           particle3        , 
               const Decays::iNode&           particle4        , 
               const bool                     decayOnly = true ) ;
      /// constructor form the cuts
      SinChi ( const LoKi::MCTypes::MCCuts&   particle1        ,  
               const LoKi::MCTypes::MCCuts&   particle2        , 
               const LoKi::MCTypes::MCCuts&   particle3        , 
               const LoKi::MCTypes::MCCuts&   particle4        , 
               const bool                     decayOnly = true ) ;
      /// constructor from the decay descriptors
      SinChi ( const std::string& particle1                   ,  
               const std::string& particle2                   , 
               const std::string& particle3                   , 
               const std::string& particle4                   , 
               const std::string& factory   = "LoKi::MCDecay" ) ;
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
      ( const LHCb::MCParticle*                 p   , 
        Decays::IMCDecay::iTree::Collection&    vct ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the proper decay components  
      StatusCode getComponents  
      ( const LHCb::MCParticle*                 p   , 
        Decays::IMCDecay::iTree::Collection&    vct ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const LoKi::MCChild::Selector& child3  () const { return m_tree3   ; }
      const LoKi::MCChild::Selector& child4  () const { return m_tree4   ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the tree to find the third  particle 
      LoKi::MCChild::Selector m_tree3 ; // the tree to find the third  particle 
      /// the tree to find the fourth particle 
      LoKi::MCChild::Selector m_tree4 ; // the tree to find the fourth particle 
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
     *  @date 2010-02-21
     */
    class GAUDI_API CosChi : public SinChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosChi ( const LoKi::MCChild::Selector& particle1        ,  
               const LoKi::MCChild::Selector& particle2        , 
               const LoKi::MCChild::Selector& particle3        , 
               const LoKi::MCChild::Selector& particle4        ) ;
      /// constructor form the trees 
      CosChi ( const Decays::IMCDecay::iTree& particle1        ,  
               const Decays::IMCDecay::iTree& particle2        , 
               const Decays::IMCDecay::iTree& particle3        , 
               const Decays::IMCDecay::iTree& particle4        ) ;
      /// constructor form the noeds
      CosChi ( const Decays::iNode&           particle1        ,  
               const Decays::iNode&           particle2        , 
               const Decays::iNode&           particle3        , 
               const Decays::iNode&           particle4        , 
               const bool                     decayOnly = true ) ;
      /// constructor form the cuts
      CosChi ( const LoKi::MCTypes::MCCuts&   particle1        ,  
               const LoKi::MCTypes::MCCuts&   particle2        , 
               const LoKi::MCTypes::MCCuts&   particle3        , 
               const LoKi::MCTypes::MCCuts&   particle4        , 
               const bool                     decayOnly = true ) ;
      /// constructor from the decay descriptors
      CosChi ( const std::string&   particle1 ,  
               const std::string&   particle2 , 
               const std::string&   particle3 , 
               const std::string&   particle4 , 
               const std::string&   factory = "LoKi::MCDecay" ) ;
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
     *  @see LoKi::Cuts::MCANGLECHI
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    class GAUDI_API AngleChi : public CosChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      AngleChi ( const LoKi::MCChild::Selector& particle1        ,  
                 const LoKi::MCChild::Selector& particle2        , 
                 const LoKi::MCChild::Selector& particle3        , 
                 const LoKi::MCChild::Selector& particle4        ) ;
      /// constructor from the trees 
      AngleChi ( const Decays::IMCDecay::iTree& particle1        ,  
                 const Decays::IMCDecay::iTree& particle2        , 
                 const Decays::IMCDecay::iTree& particle3        , 
                 const Decays::IMCDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      AngleChi ( const Decays::iNode&           particle1        ,  
                 const Decays::iNode&           particle2        , 
                 const Decays::iNode&           particle3        , 
                 const Decays::iNode&           particle4        , 
                 const bool                     decayOnly = true ) ;
      /// constructor form the cuts  
      AngleChi ( const LoKi::MCTypes::MCCuts&   particle1        ,  
                 const LoKi::MCTypes::MCCuts&   particle2        , 
                 const LoKi::MCTypes::MCCuts&   particle3        , 
                 const LoKi::MCTypes::MCCuts&   particle4        , 
                 const bool                     decayOnly = true ) ;      
      /// constructor from the decay descriptors
      AngleChi ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory = "LoKi::MCDecay" ) ;
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
     *  transverse angel theta, \f$\cos \theta_{\mathrm{tr}}\f$, 
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
     *  @see LoKi::Cuts::MCCOSTHETATR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-06-02
     */
    class GAUDI_API CosThetaTr : public AngleChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosThetaTr ( const LoKi::MCChild::Selector& particle1        ,  
                   const LoKi::MCChild::Selector& particle2        , 
                   const LoKi::MCChild::Selector& particle3        , 
                   const LoKi::MCChild::Selector& particle4        ) ;
      /// constructor from the trees 
      CosThetaTr ( const Decays::IMCDecay::iTree& particle1        ,  
                   const Decays::IMCDecay::iTree& particle2        , 
                   const Decays::IMCDecay::iTree& particle3        , 
                   const Decays::IMCDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      CosThetaTr ( const Decays::iNode&           particle1        ,  
                   const Decays::iNode&           particle2        , 
                   const Decays::iNode&           particle3        , 
                   const Decays::iNode&           particle4        , 
                   const bool                     decayOnly = true ) ;
      /// constructor form the cuts  
      CosThetaTr ( const LoKi::MCTypes::MCCuts&   particle1        ,  
                   const LoKi::MCTypes::MCCuts&   particle2        , 
                   const LoKi::MCTypes::MCCuts&   particle3        , 
                   const LoKi::MCTypes::MCCuts&   particle4        , 
                   const bool                     decayOnly = true ) ;      
      /// constructor from the decay descriptors
      CosThetaTr ( const std::string& particle1 ,  
                   const std::string& particle2 , 
                   const std::string& particle3 , 
                   const std::string& particle4 , 
                   const std::string& factory = "LoKi::MCDecay" ) ;
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
     *  @see LoKi::Cuts::MCSINPHITR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-06-02
     */
    class GAUDI_API SinPhiTr : public CosThetaTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      SinPhiTr ( const LoKi::MCChild::Selector& particle1        ,  
                 const LoKi::MCChild::Selector& particle2        , 
                 const LoKi::MCChild::Selector& particle3        , 
                 const LoKi::MCChild::Selector& particle4        ) ;
      /// constructor from the trees 
      SinPhiTr ( const Decays::IMCDecay::iTree& particle1        ,  
                 const Decays::IMCDecay::iTree& particle2        , 
                 const Decays::IMCDecay::iTree& particle3        , 
                 const Decays::IMCDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      SinPhiTr ( const Decays::iNode&           particle1        ,  
                 const Decays::iNode&           particle2        , 
                 const Decays::iNode&           particle3        , 
                 const Decays::iNode&           particle4        , 
                 const bool                     decayOnly = true ) ;
      /// constructor form the cuts  
      SinPhiTr ( const LoKi::MCTypes::MCCuts&   particle1        ,  
                 const LoKi::MCTypes::MCCuts&   particle2        , 
                 const LoKi::MCTypes::MCCuts&   particle3        , 
                 const LoKi::MCTypes::MCCuts&   particle4        , 
                 const bool                     decayOnly = true ) ;      
      /// constructor from the decay descriptors
      SinPhiTr ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory = "LoKi::MCDecay" ) ;
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
     *  @see LoKi::Cuts::MCSINPHITR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-06-02
     */
    class GAUDI_API CosPhiTr : public SinPhiTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosPhiTr ( const LoKi::MCChild::Selector& particle1        ,  
                 const LoKi::MCChild::Selector& particle2        , 
                 const LoKi::MCChild::Selector& particle3        , 
                 const LoKi::MCChild::Selector& particle4        ) ;
      /// constructor from the trees 
      CosPhiTr ( const Decays::IMCDecay::iTree& particle1        ,  
                 const Decays::IMCDecay::iTree& particle2        , 
                 const Decays::IMCDecay::iTree& particle3        , 
                 const Decays::IMCDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      CosPhiTr ( const Decays::iNode&           particle1        ,  
                 const Decays::iNode&           particle2        , 
                 const Decays::iNode&           particle3        , 
                 const Decays::iNode&           particle4        , 
                 const bool                     decayOnly = true ) ;
      /// constructor form the cuts  
      CosPhiTr ( const LoKi::MCTypes::MCCuts&   particle1        ,  
                 const LoKi::MCTypes::MCCuts&   particle2        , 
                 const LoKi::MCTypes::MCCuts&   particle3        , 
                 const LoKi::MCTypes::MCCuts&   particle4        , 
                 const bool                     decayOnly = true ) ;      
      /// constructor from the decay descriptors
      CosPhiTr ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory = "LoKi::MCDecay" ) ;
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
     *  @see LoKi::Cuts::MCSINPHITR 
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-06-02
     */
    class GAUDI_API AnglePhiTr : public CosPhiTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      AnglePhiTr ( const LoKi::MCChild::Selector& particle1        ,  
                   const LoKi::MCChild::Selector& particle2        , 
                   const LoKi::MCChild::Selector& particle3        , 
                   const LoKi::MCChild::Selector& particle4        ) ;
      /// constructor from the trees 
      AnglePhiTr ( const Decays::IMCDecay::iTree& particle1        ,  
                   const Decays::IMCDecay::iTree& particle2        , 
                   const Decays::IMCDecay::iTree& particle3        , 
                   const Decays::IMCDecay::iTree& particle4        ) ;
      /// constructor from the nodes 
      AnglePhiTr ( const Decays::iNode&           particle1        ,  
                   const Decays::iNode&           particle2        , 
                   const Decays::iNode&           particle3        , 
                   const Decays::iNode&           particle4        , 
                   const bool                     decayOnly = true ) ;
      /// constructor form the cuts  
      AnglePhiTr ( const LoKi::MCTypes::MCCuts&   particle1        ,  
                   const LoKi::MCTypes::MCCuts&   particle2        , 
                   const LoKi::MCTypes::MCCuts&   particle3        , 
                   const LoKi::MCTypes::MCCuts&   particle4        , 
                   const bool                     decayOnly = true ) ;      
      /// constructor from the decay descriptors
      AnglePhiTr ( const std::string& particle1 ,  
                   const std::string& particle2 , 
                   const std::string& particle3 , 
                   const std::string& particle4 , 
                   const std::string& factory = "LoKi::MCDecay" ) ;
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
  } //                                       end of namespace LoKi::MCParticles 
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef MCCOSPOL
     * 
     *   Evaluate polarization of  J/psi from B-decays 
     *   @code
     * 
     *   const MCCOSPOL cosPol = MCCOSPOL ( 
     *             "Xb ==>  ( J/psi(1S) => ^mu+ mu- ) ... " , 
     *             "Xb ==> ^( J/psi(1S) =>  mu+ mu- ) ... " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
     *  
     *   const double value = cosPol ( mcp ) ;
     *
     *   @endcode 
     *  
     *  @see LoKi::Kinematics::decayAngle
     *  @see LoKi::MCParticles::PolarizationAngle
     *  @see LoKi::Particles::PolarizationAngle
     *  @author Vaanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::PolarizationAngle                     MCCOSPOL ;
    // ========================================================================
    /** @typedef MCSINCHI 
     *  evaluator of sine of the angle chi 
     *
     *   @code
     * 
     *   const MCSINCHI sinChi = MCSINCHI ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
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
     *  @see LoKi::MCParticles::SinChi
     *  @see LoKi::Particles::SinChi
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::SinChi                                MCSINCHI ;
    // ========================================================================
    /** @typedef MCCOSCHI 
     *  evaluator of cosine of the angle chi 
     *
     *
     *   @code
     * 
     *   const MCCOSCHI cosChi = MCCOSCHI ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
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
     &   =
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
     *  @see LoKi::MCParticles::CosChi
     *  @see LoKi::Particles::CosChi
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::CosChi                                MCCOSCHI ;
    // ========================================================================
    /** @typedef MCANGLECHI 
     *  evaluator of the angle chi 
     *
     *   @code
     * 
     *   const MCANGLECHI chi = MCCOSCHI ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
     *  
     *   const double value = chi ( mcp ) ;
     *
     *   @endcode 
     *
     *  @see LoKi::Kinematics::decayAngleChi
     *  @see LoKi::MCParticles::AngleChi
     *  @see LoKi::Particles::AngleChi
     *  @see LoKi::Cuts::MCSINCHI
     *  @see LoKi::Cuts::MCCOSCHI
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::AngleChi                            MCANGLECHI ;
    // ========================================================================
    /** @typedef MCCOSTHETATR 
     *  Simple evaluator of \f$ \cos \theta_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const MCCOSTHETATR cosThetaTr = MCCOSTHETATR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
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
    typedef LoKi::MCParticles::CosThetaTr                        MCCOSTHETATR ;
    // ========================================================================
    /** @typedef MCSINPHITR 
     *  Simple evaluator of \f$ \sin \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const MCSINPHITR sinPhiTr = MCSINPHITR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
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
     *  @see LoKi::MCParticles::SinPhiTr
     *  @see LoKi::Kinematics::sinPhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::MCParticles::SinPhiTr                            MCSINPHITR ;
    // ========================================================================
    /** @typedef MCCOSPHITR 
     *  Simple evaluator of \f$ \cos \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const MCCOSPHITR cosPhiTr = MCCOSPHITR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
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
     *  @see LoKi::MCParticles::CosPhiTr
     *  @see LoKi::Kinematics::cosPhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::MCParticles::CosPhiTr                            MCCOSPHITR ;
    // ========================================================================
    /** @typedef MCANGLEPHITR 
     *  Simple evaluator of \f$ \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const MCANGLEPHITR phiTr = MCANGLEPHITR ( 
     *             "Xb ==> ( J/psi(1S) => ^mu+  mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+ ^mu- ) ( phi(1020) =>  K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) => ^K+  K- ) " , 
     *             "Xb ==> ( J/psi(1S) =>  mu+  mu- ) ( phi(1020) =>  K+ ^K- ) " ) ;
     *   
     *   const LHCb::MCParticle* mcp = ... ;
     *  
     *   const double value = phiTr ( mcp ) ;
     *
     *   @endcode 
     *
     *  @see LoKi::MCParticles::AnglePhiTr
     *  @see LoKi::Kinematics::anglePhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::MCParticles::AnglePhiTr                        MCANGLEPHITR ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCPARTICLES2_H
// ============================================================================
