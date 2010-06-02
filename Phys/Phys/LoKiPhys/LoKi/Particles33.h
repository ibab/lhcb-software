// $Id: Particles33.h,v 1.4 2010-06-02 15:52:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES33_H 
#define LOKI_PARTICLES33_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/iTree.h"
#include "LoKi/IDecay.h"
#include "LoKi/ChildSelector.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles 
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
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::COSPOL
     *
     *  The decay angle calculated  is that between 
     *  the flight direction of the daughter neson, "D",
     *  in the rest frame of "Q" (the parent of "D"), 
     *  with respect to "Q"'s flight direction in "P"'s
     *  (the parent of "Q") rest frame
     * 
     *  \f[ 
     *  \cos \theta = \frac
     *  { \left(P \cdot D\right)Q^2 - 
     *    \left(P \cdot Q\right)\left(D \cdot Q \right) }
     *  {\sqrt{ \left[ \left( P \cdot Q \right)^2 - Q^2 P^2 \right] 
     *          \left[ \left( D \cdot Q \right)^2 - Q^2 D^2 \right] } } 
     *  \f] 
     *  
     *  Note that the expression has the symmetry: \f$ P \leftrightarrow D \f$ 
     *  
     *  Essentially it is a rewritten <c>EvtDecayAngle(P,Q,D)</c> 
     *  routine from EvtGen package
     *
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    class PolarizationAngle
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from child selector  
      PolarizationAngle
      ( const LoKi::Child::Selector& daughter                  , 
        const LoKi::Child::Selector& parent                    , 
        const bool                   mother    = true          ) ;
      /// constructor from two trees 
      PolarizationAngle
      ( const Decays::IDecay::iTree& daughter                  , 
        const Decays::IDecay::iTree& parent                    , 
        const bool                   mother    = true          ) ;
      /// constructor from two nodes
      PolarizationAngle
      ( const Decays::iNode&         daughter                  , 
        const Decays::iNode&         parent                    , 
        const bool                   mother    = true          ) ;
      /// constructor from two cuts 
      PolarizationAngle
      ( const LoKi::PhysTypes::Cuts& daughter                  , 
        const LoKi::PhysTypes::Cuts& parent                    , 
        const bool                   mother    = true          ) ;
      /// constructor from two trees 
      PolarizationAngle
      ( const std::string&           daughter                  , 
        const std::string&           parent                    ,      
        const bool                   mother    = true          , 
        const std::string&           factory   = "LoKi::Decay" ) ;
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
      PolarizationAngle () ; // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// get the proper decay components  
      StatusCode getComponents12
      ( const LHCb::Particle*                 p   , 
        Decays::IDecay::iTree::Collection&    vct ) const ;
      /// valid trees?
      bool valid  () const { return m_first.valid  () && m_second.valid  () ; }      
      // ======================================================================
    public:      
      // ======================================================================
      /// the first child 
      const LoKi::Child::Selector& child1 () const { return m_first  ; }
      /// the second tree 
      const LoKi::Child::Selector& child2 () const { return m_second ; }
      // ======================================================================
    private:
      // ======================================================================
      /// rule to find the first  particle 
      LoKi::Child::Selector m_first   ;      //  rule to find the first particle 
      /// rule to find the second pa rticle 
      LoKi::Child::Selector m_second  ;      // rule to find the second particle 
      /// use (dauther-parent) or two-daughters rule ?
      bool                 m_mother  ; 
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
     *  @see LoKi::Cuts::SINCHI
     *
     *  The angle is  calculated using the explicit 
     *   Lorentz-invariant expression:
     *  \f[ 
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
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    class SinChi : public PolarizationAngle 
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      SinChi ( const LoKi::Child::Selector& particle1 ,  
               const LoKi::Child::Selector& particle2 , 
               const LoKi::Child::Selector& particle3 , 
               const LoKi::Child::Selector& particle4 ) ;
      /// constructor form the trees 
      SinChi ( const Decays::IDecay::iTree& particle1 ,  
               const Decays::IDecay::iTree& particle2 , 
               const Decays::IDecay::iTree& particle3 , 
               const Decays::IDecay::iTree& particle4 ) ;
      /// constructor form the nodes 
      SinChi ( const Decays::iNode& particle1 ,  
               const Decays::iNode& particle2 , 
               const Decays::iNode& particle3 , 
               const Decays::iNode& particle4 ) ;
      /// constructor from the cuts 
      SinChi ( const LoKi::PhysTypes::Cuts& particle1 , 
               const LoKi::PhysTypes::Cuts& particle2 , 
               const LoKi::PhysTypes::Cuts& particle3 , 
               const LoKi::PhysTypes::Cuts& particle4 ) ;
      /// constructor from the decay descriptors
      SinChi ( const std::string& particle1 ,  
               const std::string& particle2 , 
               const std::string& particle3 , 
               const std::string& particle4 , 
               const std::string& factory   = "LoKi::Decay" ) ;
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
      ( const LHCb::Particle*                 p   , 
        Decays::IDecay::iTree::Collection&    vct ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the proper decay components  
      StatusCode getComponents  
      ( const LHCb::Particle*                 p   , 
        Decays::IDecay::iTree::Collection&    vct ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const LoKi::Child::Selector& child3  () const { return m_tree3   ; }
      const LoKi::Child::Selector& child4  () const { return m_tree4   ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the tree to find the third  particle 
      LoKi::Child::Selector m_tree3 ;    // the tree to find the third  particle 
      /// the tree to find the fourth particle 
      LoKi::Child::Selector m_tree4 ;    // the tree to find the fourth particle 
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
     *  @see LoKi::Cuts::SINCHI
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
     *
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    class CosChi : public SinChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosChi ( const LoKi::Child::Selector& particle1 ,  
               const LoKi::Child::Selector& particle2 , 
               const LoKi::Child::Selector& particle3 , 
               const LoKi::Child::Selector& particle4 ) ;
      /// constructor form the trees 
      CosChi ( const Decays::IDecay::iTree& particle1 ,  
               const Decays::IDecay::iTree& particle2 , 
               const Decays::IDecay::iTree& particle3 , 
               const Decays::IDecay::iTree& particle4 ) ;
      /// constructor form the noeds
      CosChi ( const Decays::iNode& particle1 ,  
               const Decays::iNode& particle2 , 
               const Decays::iNode& particle3 , 
               const Decays::iNode& particle4 ) ;
      /// constructor from the cuts 
      CosChi ( const LoKi::PhysTypes::Cuts& particle1 , 
               const LoKi::PhysTypes::Cuts& particle2 , 
               const LoKi::PhysTypes::Cuts& particle3 , 
               const LoKi::PhysTypes::Cuts& particle4 ) ;
      /// constructor from the decay descriptors
      CosChi ( const std::string&   particle1 ,  
               const std::string&   particle2 , 
               const std::string&   particle3 , 
               const std::string&   particle4 , 
               const std::string&   factory      = "LoKi::Decay" ) ;
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
     *  @see LoKi::Cuts::SINCHI
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    class AngleChi : public CosChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      AngleChi ( const LoKi::Child::Selector& particle1 ,  
                 const LoKi::Child::Selector& particle2 , 
                 const LoKi::Child::Selector& particle3 , 
                 const LoKi::Child::Selector& particle4 ) ;
      /// constructor from the trees 
      AngleChi ( const Decays::IDecay::iTree& particle1 ,  
                 const Decays::IDecay::iTree& particle2 , 
                 const Decays::IDecay::iTree& particle3 , 
                 const Decays::IDecay::iTree& particle4 ) ;
      /// constructor from the nodes 
      AngleChi ( const Decays::iNode& particle1 ,  
                 const Decays::iNode& particle2 , 
                 const Decays::iNode& particle3 , 
                 const Decays::iNode& particle4 ) ;
      /// constructor from the cuts 
      AngleChi ( const LoKi::PhysTypes::Cuts& particle1 , 
                 const LoKi::PhysTypes::Cuts& particle2 , 
                 const LoKi::PhysTypes::Cuts& particle3 , 
                 const LoKi::PhysTypes::Cuts& particle4 ) ;
      /// constructor from the decay descriptors
      AngleChi ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory      = "LoKi::Decay" ) ;
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
     *
     *  Simple functor that evaluates the cosine of 
     *  transversity angle \f$\cos \theta_{\mathrm{tr}} \f$
     *
     *  @see LoKi::Kinematics::cosThetaTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::COSTHETATR
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
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    class CosThetaTr : public AngleChi
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosThetaTr ( const LoKi::Child::Selector& particle1 ,  
                   const LoKi::Child::Selector& particle2 , 
                   const LoKi::Child::Selector& particle3 , 
                   const LoKi::Child::Selector& particle4 ) ;
      /// constructor from the trees 
      CosThetaTr ( const Decays::IDecay::iTree& particle1 ,  
                   const Decays::IDecay::iTree& particle2 , 
                   const Decays::IDecay::iTree& particle3 , 
                   const Decays::IDecay::iTree& particle4 ) ;
      /// constructor from the nodes 
      CosThetaTr ( const Decays::iNode& particle1 ,  
                   const Decays::iNode& particle2 , 
                   const Decays::iNode& particle3 , 
                   const Decays::iNode& particle4 ) ;
      /// constructor from the cuts 
      CosThetaTr ( const LoKi::PhysTypes::Cuts& particle1 , 
                   const LoKi::PhysTypes::Cuts& particle2 , 
                   const LoKi::PhysTypes::Cuts& particle3 , 
                   const LoKi::PhysTypes::Cuts& particle4 ) ;
      /// constructor from the decay descriptors
      CosThetaTr ( const std::string& particle1 ,  
                   const std::string& particle2 , 
                   const std::string& particle3 , 
                   const std::string& particle4 , 
                   const std::string& factory      = "LoKi::Decay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~CosThetaTr() ;
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
     *
     *  Simple functor that evaluates the sine of 
     *  transversity angle phi \f$\sin \phi_{\mathrm{tr}} \f$
     *
     *  @see LoKi::Kinematics::sinPhiTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::SINPHITR
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
     *
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    class SinPhiTr : public CosThetaTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      SinPhiTr ( const LoKi::Child::Selector& particle1 ,  
                 const LoKi::Child::Selector& particle2 , 
                 const LoKi::Child::Selector& particle3 , 
                 const LoKi::Child::Selector& particle4 ) ;
      /// constructor from the trees 
      SinPhiTr ( const Decays::IDecay::iTree& particle1 ,  
                 const Decays::IDecay::iTree& particle2 , 
                 const Decays::IDecay::iTree& particle3 , 
                 const Decays::IDecay::iTree& particle4 ) ;
      /// constructor from the nodes 
      SinPhiTr ( const Decays::iNode& particle1 ,  
                 const Decays::iNode& particle2 , 
                 const Decays::iNode& particle3 , 
                 const Decays::iNode& particle4 ) ;
      /// constructor from the cuts 
      SinPhiTr ( const LoKi::PhysTypes::Cuts& particle1 , 
                 const LoKi::PhysTypes::Cuts& particle2 , 
                 const LoKi::PhysTypes::Cuts& particle3 , 
                 const LoKi::PhysTypes::Cuts& particle4 ) ;
      /// constructor from the decay descriptors
      SinPhiTr ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory      = "LoKi::Decay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~SinPhiTr() ;
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
      SinPhiTr () ;                    // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CosPhiTr
     *
     *  Simple functor that evaluates the cosine of 
     *  transversity angle phi \f$\cos \phi_{\mathrm{tr}} \f$
     *
     *  @see LoKi::Kinematics::cosPhiTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::COSPHITR
     *
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expressions
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    class CosPhiTr : public SinPhiTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      CosPhiTr ( const LoKi::Child::Selector& particle1 ,  
                 const LoKi::Child::Selector& particle2 , 
                 const LoKi::Child::Selector& particle3 , 
                 const LoKi::Child::Selector& particle4 ) ;
      /// constructor from the trees 
      CosPhiTr ( const Decays::IDecay::iTree& particle1 ,  
                 const Decays::IDecay::iTree& particle2 , 
                 const Decays::IDecay::iTree& particle3 , 
                 const Decays::IDecay::iTree& particle4 ) ;
      /// constructor from the nodes 
      CosPhiTr ( const Decays::iNode& particle1 ,  
                 const Decays::iNode& particle2 , 
                 const Decays::iNode& particle3 , 
                 const Decays::iNode& particle4 ) ;
      /// constructor from the cuts 
      CosPhiTr ( const LoKi::PhysTypes::Cuts& particle1 , 
                 const LoKi::PhysTypes::Cuts& particle2 , 
                 const LoKi::PhysTypes::Cuts& particle3 , 
                 const LoKi::PhysTypes::Cuts& particle4 ) ;
      /// constructor from the decay descriptors
      CosPhiTr ( const std::string& particle1 ,  
                 const std::string& particle2 , 
                 const std::string& particle3 , 
                 const std::string& particle4 , 
                 const std::string& factory      = "LoKi::Decay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~CosPhiTr() ;
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
     *
     *  Simple functor that evaluates the 
     *  transversity angle phi \f$\phi_{\mathrm{tr}} \f$
     *
     *  @see LoKi::Kinematics::anglePhiTr
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::ANGLEPHITR
     *
     *  The evaluation is performed using the explicit Lorentz-invariant 
     *  expressions
     *
     *  @attention It is very important to know the structure 
     *             of the decay for proper use of this functor
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    class AnglePhiTr : public CosPhiTr
    {    
    public:
      // ======================================================================
      /// constructor from child-selector 
      AnglePhiTr ( const LoKi::Child::Selector& particle1 ,  
                   const LoKi::Child::Selector& particle2 , 
                   const LoKi::Child::Selector& particle3 , 
                   const LoKi::Child::Selector& particle4 ) ;
      /// constructor from the trees 
      AnglePhiTr ( const Decays::IDecay::iTree& particle1 ,  
                   const Decays::IDecay::iTree& particle2 , 
                   const Decays::IDecay::iTree& particle3 , 
                   const Decays::IDecay::iTree& particle4 ) ;
      /// constructor from the nodes 
      AnglePhiTr ( const Decays::iNode& particle1 ,  
                   const Decays::iNode& particle2 , 
                   const Decays::iNode& particle3 , 
                   const Decays::iNode& particle4 ) ;
      /// constructor from the cuts 
      AnglePhiTr ( const LoKi::PhysTypes::Cuts& particle1 , 
                   const LoKi::PhysTypes::Cuts& particle2 , 
                   const LoKi::PhysTypes::Cuts& particle3 , 
                   const LoKi::PhysTypes::Cuts& particle4 ) ;
      /// constructor from the decay descriptors
      AnglePhiTr ( const std::string& particle1 ,  
                   const std::string& particle2 , 
                   const std::string& particle3 , 
                   const std::string& particle4 , 
                   const std::string& factory      = "LoKi::Decay" ) ;
      /// MANDATORY: virtual destructor
      virtual ~AnglePhiTr() ;
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
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef COSPOL
     *  Simple evaluation of cosine of the polarization angle for
     *  daughter particles
     * 
     *  @code
     * 
     *   const COSPOL func = COSPOL  ( "Xb -> (  J/psi(1S) -> ^mu+ mu- ) K+ K-" , 
     *                                 "Xb -> ( ^J/psi(1S) ->  mu+ mu- ) K+ K-" ) ;
     *
     *   const LHCb::Particle* B = ... ;
     * 
     *   const double cosangle = func ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::PolarizationAngle
     *  @see LoKi::Kinematics::decayAngle 
     *  @see LoKi::Kinematics::cosDecayAngle 
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::COSPOL
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    typedef LoKi::Particles::PolarizationAngle                         COSPOL ;
    // ========================================================================
    /** @typedef SINCHI
     *  Simple evaluation of (sine) the angle betwen two decay planes.
     *  Each plane is defined by two particles: (1,2) and (3,4).
     * 
     *  @code
     * 
     *   const SINCHI sinChi = SINCHI ( "Xb --> ^K+  K-  mu+  mu- " , 
     *                                  "Xb -->  K+ ^K-  mu+  mu- " , 
     *                                  "Xb -->  K+  K- ^mu+ ^mu- " , 
     *                                  "Xb -->  K+ ^K-  mu+  mu- " ) ;
     * 
     *   const LHCb::Particle* B = ... ;
     * 
     *   const double sine = sinChi ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Kinematics::sinDecayAngleChi
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::COSPOL
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    typedef LoKi::Particles::SinChi                                    SINCHI ;
    // ========================================================================
    /** @typedef COSCHI
     *  Simple evaluation of (cosine) the angle betwen two decay planes.
     *  Each plane is defined by two particles: (1,2) and (3,4).
     * 
     *  @code
     * 
     *   const COSCHI cosChi = COSCHI ( "Xb --> ^K+  K-  mu+  mu- " , 
     *                                  "Xb -->  K+ ^K-  mu+  mu- " , 
     *                                  "Xb -->  K+  K- ^mu+ ^mu- " , 
     *                                  "Xb -->  K+ ^K-  mu+  mu- " ) ;
     * 
     *   const LHCb::Particle* B = ... ;
     * 
     *   const double cosine = cosChi ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Kinematics::cosDecayAngleChi
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::COSPOL
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    typedef LoKi::Particles::CosChi                                    COSCHI ;
    // ========================================================================
    /** @typedef ANGLECHI
     *  Simple evaluation of (the angle betwen two decay planes.
     *  Each plane is defined by two particles: (1,2) and (3,4).
     * 
     *  @code
     * 
     *   const ANGLECHI chi = ANGLECHI ( "Xb --> ^K+  K-  mu+  mu- " , 
     *                                   "Xb -->  K+ ^K-  mu+  mu- " , 
     *                                   "Xb -->  K+  K- ^mu+ ^mu- " , 
     *                                   "Xb -->  K+ ^K-  mu+  mu- " ) ;
     * 
     *   const LHCb::Particle* B = ... ;
     * 
     *   const double angle = chi ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Kinematics::decayAngleChi
     *  @see LoKi::Kinematics::sinDecayAngleChi
     *  @see LoKi::Kinematics::cosDecayAngleChi
     *  @see IP2VVAngleCalculator
     *  @see IP2VVPartAngleCalculator
     *  @see LoKi::Cuts::COSPOL
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-21
     */
    typedef LoKi::Particles::AngleChi                                ANGLECHI ;
    // ========================================================================
    /** @typedef COSTHETATR 
     *  Simple evaluator of \f$ \cos \theta_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const COSTHETATR cosThetaTr = COSTHETATR ( 
     *             "Xb -> ( J/psi(1S) -> ^mu+  mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+ ^mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) -> ^K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) ->  K+ ^K- ) " ) ;
     *   
     *   const LHCb::Particle* mcp = ... ;
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
     *  @see LoKi::Particles::CosThetaTr
     *  @see LoKi::Kinematics::cosThetaTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::Particles::CosThetaTr                            COSTHETATR ;
    // ========================================================================
    /** @typedef SINPHITR 
     *  Simple evaluator of \f$ \sin \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const SINPHITR sinPhiTr = SINPHITR ( 
     *             "Xb -> ( J/psi(1S) -> ^mu+  mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+ ^mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) -> ^K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) ->  K+ ^K- ) " ) ;
     *   
     *   const LHCb::Particle* mcp = ... ;
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
     *  @see LoKi::Particles::SinPhiTr
     *  @see LoKi::Kinematics::sinPhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::Particles::SinPhiTr                                SINPHITR ;
    // ========================================================================
    /** @typedef COSPHITR 
     *  Simple evaluator of \f$ \cos \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const COSPHITR cosPhiTr = COSPHITR ( 
     *             "Xb -> ( J/psi(1S) -> ^mu+  mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+ ^mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) -> ^K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) ->  K+ ^K- ) " ) ;
     *   
     *   const LHCb::Particle* mcp = ... ;
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
     *  @see LoKi::Particles::CosPhiTr
     *  @see LoKi::Kinematics::cosPhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::Particles::CosPhiTr                                COSPHITR ;
    // ========================================================================
    /** @typedef ANGLEPHITR 
     *  Simple evaluator of \f$ \phi_{\mathrm{tr}}\f$
     *
     *   @code
     * 
     *   const ANGLEPHITR phiTr = ANGLEPHITR ( 
     *             "Xb -> ( J/psi(1S) -> ^mu+  mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+ ^mu- ) ( phi(1020) ->  K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) -> ^K+  K- ) " , 
     *             "Xb -> ( J/psi(1S) ->  mu+  mu- ) ( phi(1020) ->  K+ ^K- ) " ) ;
     *   
     *   const LHCb::Particle* mcp = ... ;
     *  
     *   const double value = phiTr ( mcp ) ;
     *
     *   @endcode 
     *
     *  @see LoKi::Particles::AnglePhiTr
     *  @see LoKi::Kinematics::anglePhiTr 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::Particles::AnglePhiTr                            ANGLEPHITR ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES33_H
// ============================================================================
