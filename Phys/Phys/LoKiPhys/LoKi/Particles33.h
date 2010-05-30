// $Id: Particles33.h,v 1.2 2010-05-30 17:11:01 ibelyaev Exp $
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
      /// constructor from the decay descriptors
      SinChi ( const std::string& particle1 ,  
               const std::string& particle2 , 
               const std::string& particle3 , 
               const std::string& particle4 , 
               const std::string& factory      = "LoKi::Decay" ) ;
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
      /// constructor from the decay descriptors
      CosChi ( const std::string& particle1 ,  
               const std::string& particle2 , 
               const std::string& particle3 , 
               const std::string& particle4 , 
               const std::string& factory      = "LoKi::Decay" ) ;
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
      /// constructor form the trees 
      AngleChi ( const Decays::IDecay::iTree& particle1 ,  
                 const Decays::IDecay::iTree& particle2 , 
                 const Decays::IDecay::iTree& particle3 , 
                 const Decays::IDecay::iTree& particle4 ) ;
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
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES33_H
// ============================================================================
