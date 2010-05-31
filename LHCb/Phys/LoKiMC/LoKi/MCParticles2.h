// $Id: MCParticles2.h,v 1.1 2010-05-31 20:33:54 ibelyaev Exp $
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
    class PolarizationAngle
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
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    class SinChi : public PolarizationAngle 
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
    class AngleChi : public CosChi
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
  } //                                       end of namespace LoKi::MCParticles 
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef MCCOSPOL
     *  Evaluator of cosine of polarization angle
     *  @see LoKi::MCParticles::PolarizationAngle
     *  @see LoKi::Particles::PolarizationAngle
     *  @author Vaanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::PolarizationAngle                     MCCOSPOL ;
    // ========================================================================
    /** @typedef MCSINCHI 
     *  evaluator of sine of the angle chi 
     *  @see LoKi::MCParticles::SinChi
     *  @see LoKi::Particles::SinChi
     *  @author Vaanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::SinChi                                MCSINCHI ;
    // ========================================================================
    /** @typedef MCCOSCHI 
     *  evaluator of cosine of the angle chi 
     *  @see LoKi::MCParticles::CosChi
     *  @see LoKi::Particles::CosChi
     *  @author Vaanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::CosChi                                MCCOSCHI ;
    // ========================================================================
    /** @typedef MCANGLECHI 
     *  evaluator of the angle chi 
     *  @see LoKi::MCParticles::AngleChi
     *  @see LoKi::Particles::AngleChi
     *  @author Vaanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCParticles::AngleChi                            MCANGLECHI ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCPARTICLES2_H
// ============================================================================
