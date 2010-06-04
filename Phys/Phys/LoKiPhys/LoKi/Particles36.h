// $Id: Particles36.h,v 1.1 2010-06-04 12:23:59 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES36_H
#define LOKI_PARTICLES36_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciInterafces
// ============================================================================
#include "Kernel/IDecayTreeFit.h"
#include "Kernel/IDVAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/AuxDTFBase.h"
#include "LoKi/ChildSelector.h"
// ============================================================================
/** @file LoKi/Particles36.h
 *  Collection of functors dealing with DecayTreeFitter
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-03
 */
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class DecayTreeFitterFun
     *  Simple meta-functor that refits the decay
     *  tree of the particle and apply another functor to the
     *  refited decay tree
     *  @see IDecayTreeFit
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-03
     */
    class DecayTreeFitterFun
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::AuxDTFBase 
    {
    public:
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterFun
      ( const LoKi::PhysTypes::Func&         fun           ,
        const bool                           usePV         ,
        const IDecayTreeFit*                 fitter = 0    ) ;
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterFun
      ( const LoKi::PhysTypes::Func&         fun           ,
        const bool                           usePV         ,
        const std::vector<std::string>&      constraints   ,
        const IDecayTreeFit*                 fitter = 0    ) ;
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterFun
      ( const LoKi::PhysTypes::Func&         fun           ,
        const bool                           usePV         ,
        const std::string&                   constraint    ,
        const IDecayTreeFit*                 fitter = 0    ) ;
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterFun
      ( const LoKi::PhysTypes::Func&         fun           ,
        const bool                           usePV         ,
        const LoKi::AuxDTFBase&              base          ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual  ~DecayTreeFitterFun () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual   DecayTreeFitterFun* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator()  ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      DecayTreeFitterFun () ;             // the default constrctor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the functor to be used
      LoKi::PhysTypes::Fun m_fun        ;       //       the functor to be used
      /// use related primary vertex in the fit ?
      bool                 m_usePV      ;       // use related primary vertex ?
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DecayTreeFitterCut
     *  Simple meta-functor that refits the decay
     *  tree of the particle and apply another functor to the
     *  refited decay tree
     *  @see IDecayTreeFit
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-03
     */
    class DecayTreeFitterCut
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
      , public LoKi::AuxDTFBase 
    {
    public:
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterCut
      ( const LoKi::PhysTypes::Cuts&         fun           ,
        const bool                           usePV         ,
        const IDecayTreeFit*                 fitter = 0    ) ;
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterCut
      ( const LoKi::PhysTypes::Cuts&         fun           ,
        const bool                           usePV         ,
        const std::vector<std::string>&      constraints   ,
        const IDecayTreeFit*                 fitter = 0    ) ;
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterCut
      ( const LoKi::PhysTypes::Cuts&         fun           ,
        const bool                           usePV         ,
        const std::string&                   constraint    ,
        const IDecayTreeFit*                 fitter = 0    ) ;
      // ======================================================================
      /// constructor with the functor
      DecayTreeFitterCut
      ( const LoKi::PhysTypes::Cuts&         fun           ,
        const bool                           usePV         ,
        const LoKi::AuxDTFBase&              base          ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual  ~DecayTreeFitterCut () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual   DecayTreeFitterCut* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator()  ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      DecayTreeFitterCut () ;             // the default constrctor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the functor to be used
      LoKi::PhysTypes::Cut m_fun        ;       //       the functor to be used
      /// use related primary vertex in the fit ?
      bool                 m_usePV      ;       // use related primary vertex ?
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ChildCTau
     *  Simple evaluation of \f$c\tau\f$ for the (child) 
     *  particle from the decay tree 
     *  The evaluation is done after refit of the whole decay tree 
     *  using DecayTreeFitter by Wouter HULSBERGEN 
     *  @see LoKi::Cuts::DTF_CTAU
     *  @see IDecayTreeFit 
     *  @see  DecayTreeFitter::Fitter 
     *  @see LoKi::Particles::DecayTreeFitterCut
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-03
     */     
    class ChildCTau
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::AuxDTFBase 
    {
      // ======================================================================
    public:
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const LoKi::Child::Selector&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const std::vector<unsigned int>& child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints =
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const unsigned int               child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints =
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const std::string&               child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints =
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const Decays::iNode&             child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints =
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const Decays::IDecay::iTree&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints =
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const Decays::IDecay::Finder&    child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints =
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTau
      ( const LoKi::PhysTypes::Cuts&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints =
        std::vector<std::string>() ) ;
      /// MANDATORY: virtual descructor 
      virtual ~ChildCTau() ;
      /// MANDATORY: clone method ("virtual cosntructor")
      virtual  ChildCTau* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      ChildCTau () ;                      // the default constrctor is disabled
      // ======================================================================
    protected:
      // ======================================================================
      /// get the child selector 
      const LoKi::Child::Selector& child () const { return m_child ; }
      /// use the child selector !
      const LHCb::Particle* child ( const LHCb::Particle * p ) const
      { return m_child.child ( p ) ; }
      // ======================================================================
      // use PV-constraints for the decay head ? 
      bool usePV () const { return m_usePV ; }
      // ======================================================================
      /// get the fittes parameters
      const IDecayTreeFit::Fitted* params ( const LHCb::Particle* p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// child selector 
      LoKi::Child::Selector m_child      ;       //              child selector 
      /// use related primary vertex in the fit ?
      bool                  m_usePV      ;       // use related primary vertex ?
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ChildCTauErr
     *  Simple evaluation of 
     *  \f$ \sigma \left( c\tau \right\f$ 
     *  for the (child) particle form the 
     *  decay tree 
     *  The evaluation is done after refit of the whole decay tree 
     *  using DecayTreeFitter by Wouter HULSBERGEN 
     *  @see LoKi::Cuts::DTF_CTAU
     *  @see IDecayTreeFit 
     *  @see  DecayTreeFitter::Fitter 
     *  @see LoKi::Particles::DecayTreeFitterCut
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-03
     */     
    class ChildCTauErr : public ChildCTau 
    {
    public:
      // ======================================================================
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const LoKi::Child::Selector&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const std::vector<unsigned int>& child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const unsigned int               child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const std::string&               child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const Decays::iNode&             child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const Decays::IDecay::iTree&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const Decays::IDecay::Finder&    child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauErr
      ( const LoKi::PhysTypes::Cuts&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// MANDATORY: virtual descructor 
      virtual ~ChildCTauErr() ;
      /// MANDATORY: clone method ("virtual cosntructor")
      virtual  ChildCTauErr* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      ChildCTauErr () ;                  // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ChildCTauSignificance
     *  Simple evaluation of 
     *  \f$ \frac{ c\tau} { \sigma \left( c\tau \right} \f$ 
     *  for the (child) particle form the 
     *  decay tree 
     *  The evaluation is done after refit of the whole decay tree 
     *  using DecayTreeFitter by Wouter HULSBERGEN 
     *  @see LoKi::Cuts::DTF_CTAUSIGNIFICANCE
     *  @see IDecayTreeFit 
     *  @see  DecayTreeFitter::Fitter 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-03
     */     
    class ChildCTauSignificance : public ChildCTauErr 
    {
    public:
      // ======================================================================
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const LoKi::Child::Selector&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const std::vector<unsigned int>& child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const unsigned int               child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const std::string&               child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const Decays::iNode&             child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const Decays::IDecay::iTree&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const Decays::IDecay::Finder&    child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// constructor from child selector & PV-flag & constraints 
      ChildCTauSignificance
      ( const LoKi::PhysTypes::Cuts&     child       , 
        const bool                       usePV       , 
        const std::vector<std::string>&  constraints = 
        std::vector<std::string>() ) ;
      /// MANDATORY: virtual descructor 
      virtual ~ChildCTauSignificance () ;
      /// MANDATORY: clone method ("virtual cosntructor")
      virtual  ChildCTauSignificance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      ChildCTauSignificance () ;         // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef DTF_FUN
     *  Simple meta-functor that uses DecayTreeFitter 
     *  and applyes another functor to the resutl of fit 
     *  @see IDecayTreeFit 
     *  @see  DecayTreeFitter::Fitter
     *  
     *  @code
     *
     *    // get the momentum after the fit
     *    const DFT_FUN p1 = DTF_FUN ( P , false ) ;
     *
     *    // get the momentum after the fit, using Primary Vertex-constraint
     *    const DFT_FUN p2 = DTF_FUN ( P , true  ) ;
     *   
     *    // get the momentum after the fit, and mass-constrains for daughter psis:
     *    const DFT_FUN p3 = DTF_FUN ( P , false , 'J/psi(1S)'  ) ;
     *   
     *    // get the momentum after the fit, using Primary Vertex-constraint and 
     *    // mass-constraints for daughter D0 
     *    const DFT_FUN p4 = DTF_FUN ( P , true  , 'D0'  ) ;
     *   
     *    const LHCb::Particle* B = ... ;
     *  
     *    const double val1  = p1 ( B ) ;
     *    const double val2  = p2 ( B ) ;
     *    const double val3  = p3 ( B ) ;
     *    const double val4  = p4 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::DecayTreeFitterFun 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-03
     */
    typedef LoKi::Particles::DecayTreeFitterFun                       DTF_FUN ;
    // ========================================================================
    /** @typedef DTF_CUT
     *  Simple meta-predicate that uses DecayTreeFitter 
     *  and applyes another predicate to the resutl of fit 
     *  @see IDecayTreeFit 
     *  @see  DecayTreeFitter::Fitter
     *  
     *  @code
     *
     *    // get the momentum after the fit
     *    const DFT_CUT p1 = DTF_CUT ( PX > 0  , false ) ;
     *
     *    // get the momentum after the fit, using Primary Vertex-constraint
     *    const DFT_CUT p2 = DTF_CUT ( PX > 0 , true  ) ;
     *   
     *    // get the momentum after the fit, and mass-constrains for daughter psis:
     *    const DFT_CUT p3 = DTF_CUT ( PX >, false , 'J/psi(1S)'  ) ;
     *   
     *    // get the momentum after the fit, using Primary Vertex-constraint and 
     *    // mass-constraints for daughter D0 
     *    const DFT_CUT p4 = DTF_CUT ( P , true  , 'D0'  ) ;
     *   
     *    const LHCb::Particle* B = ... ;
     *  
     *    const bool val1  = p1 ( B ) ;
     *    const bool val2  = p2 ( B ) ;
     *    const bool val3  = p3 ( B ) ;
     *    const bool val4  = p4 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::DecayTreeFitterCut
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-06-03
     */
    typedef LoKi::Particles::DecayTreeFitterCut                       DTF_CUT ;
    // ========================================================================
    /** @typedef DTF_CTAU 
     *  Evaluate \f$c\tau\f$ for the dauthter particle in the 
     *  decay tree. The actual computation inclued the refit of the 
     *  whole decya tree using DecayTreeFitter by Wouter HULSBERGEN 
     *  @see IDecayTreeFit
     *  @see  DecayTreeFitter::Fitter 
     *
     *  @code
     *
     *    // get the c*tau for the first daughter particle 
     *    const DFT_CTAU ctau1 = DTF_CTAU ( 1 , false ) ;
     *
     *    // get the c*tau for the D0 from decay tree of beauty particle  
     *    const DFT_CTAU ctau2 = DTF_CTAU ( "Xb -> ^D0 ... " , false ) ;
     *
     *    // get the c*tau for the D0 from decay tree of beauty particle  
     *    const DFT_CTAU ctau3 = DTF_CTAU ( "D0" == ABSPID , false ) ;
     *
     *    // get the c*tau for the first daughter particle
     *    // with PV-constaint for the decay head  
     *    const DFT_CTAU ctau4 = DTF_CTAU ( 1 , true ) ;
     *
     *    // get the c*tau for the D0 from decay tree of beauty particle 
     *    // with PV-constaint for the decay head  
     *    const DFT_CTAU ctau5 = DTF_CTAU ( "Xb -> ^D0 ... " , true ) ;
     *
     *    // get the c*tau for the D0 from decay tree of beauty particle  
     *    // with PV-constaint for the decay head  
     *    const DFT_CTAU ctau6 = DTF_CTAU ( "D0" == ABSPID , true ) ;
     *
     *
     *    std::vector<std::string> constraints ;
     *    constraints.push_back ( "D0"   ) ;
     *    constraints.push_back ( "D+"   ) ;
     *    constraints.push_back ( "D_s+" ) ;
     *
     *    // get the c*tau for the first daughter particle
     *    // with PV-constaint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAU ctau7 = DTF_CTAU ( 1 , true , constraints ) ;
     *
     *    // get the c*tau for the D0 from decay tree of beauty particle 
     *    // with PV-constaint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAU ctau8 = DTF_CTAU ( "Xb -> ^D0 ... " , 
     *                                      true , constraints ) ;
     *
     *    // get the c*tau for the D0 from decay tree of beauty particle  
     *    // with PV-constaint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAU ctau9 = DTF_CTAU ( "D0" == ABSPID ,
     *                                      true , constraints ) ;
     *
     *    const LHCb::Particle* B = ... ;
     *
     *    const double val1 = ctau1 ( B ) ;
     *    const double val2 = ctau2 ( B ) ;
     *    const double val3 = ctau3 ( B ) ;
     *    const double val4 = ctau4 ( B ) ;
     *    const double val5 = ctau5 ( B ) ;
     *    const double val6 = ctau6 ( B ) ;
     *    const double val7 = ctau7 ( B ) ;
     *    const double val8 = ctau8 ( B ) ;
     *    const double val9 = ctau9 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::ChildCTau
     *  @see LoKi::Cuts::DTF_CTAUERR 
     *  @see LoKi::Cuts::DTF_CTAUSIGNIFICANCE
     *  @see LoKi::Child::Selector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-03
     */
    typedef LoKi::Particles::ChildCTau                               DTF_CTAU ;
    // ========================================================================
    /** @typedef DTF_CTAUERR 
     *  Evaluate \f$\sigma \left( c\tau\right) \f$ for the 
     *  dauthter particle in the 
     *  decay tree. The actual computation inclued the refit of the 
     *  whole decya tree using DecayTreeFitter by Wouter HULSBERGEN 
     *  @see IDecayTreeFit
     *  @see  DecayTreeFitter::Fitter 
     *
     *  @code
     *
     *    // get the sigma(c*tau) for the first daughter particle 
     *    const DFT_CTAUERR sctau1 = DTF_CTAUERR ( 1 , false ) ;
     *
     *    // get the sigma(c*tau) for the D0 from decay tree of beauty particle  
     *    const DFT_CTAUERR sctau2 = DTF_CTAUERR ( "Xb -> ^D0 ... " , false ) ;
     *
     *    // get the sigma(c*tau) for the D0 from decay tree of beauty particle  
     *    const DFT_CTAUERR sctau3 = DTF_CTAUERR ( "D0" == ABSPID , false ) ;
     *
     *    // get the sigma(c*tau) for the first daughter particle
     *    // with PV-constraint for the decay head  
     *    const DFT_CTAUERR sctau4 = DTF_CTAUERR ( 1 , true ) ;
     *
     *    // get the sigma(c*tau) for the D0 from decay tree of beauty particle 
     *    // with PV-constraint for the decay head  
     *    const DFT_CTAUERR sctau5 = DTF_CTAUERR ( "Xb -> ^D0 ... " , true ) ;
     *
     *    // get the sigma(c*tau) for the D0 from decay tree of beauty particle  
     *    // with PV-constraint for the decay head  
     *    const DFT_CTAUERR sctau6 = DTF_CTAUERR ( "D0" == ABSPID , true ) ;
     *
     *
     *    std::vector<std::string> constraints ;
     *    constraints.push_back ( "D0"   ) ;
     *    constraints.push_back ( "D+"   ) ;
     *    constraints.push_back ( "D_s+" ) ;
     *
     *    // get the sigma(c*tau) for the first daughter particle
     *    // with PV-constraint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAUERR sctau7 = DTF_CTAUERR ( 1 , true , constraints ) ;
     *
     *    // get the sigma(c*tau) for the D0 from decay tree of beauty particle 
     *    // with PV-constraint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAUERR sctau8 = DTF_CTAUERR ( "Xb -> ^D0 ... " , 
     *                                      true , constraints ) ;
     *
     *    // get the sigma(c*tau) for the D0 from decay tree of beauty particle  
     *    // with PV-constraint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAUERR sctau9 = DTF_CTAUERR ( "D0" == ABSPID ,
     *                                      true , constraints ) ;
     *
     *    const LHCb::Particle* B = ... ;
     *
     *    const double val1 = sctau1 ( B ) ;
     *    const double val2 = sctau2 ( B ) ;
     *    const double val3 = sctau3 ( B ) ;
     *    const double val4 = sctau4 ( B ) ;
     *    const double val5 = sctau5 ( B ) ;
     *    const double val6 = sctau6 ( B ) ;
     *    const double val7 = sctau7 ( B ) ;
     *    const double val8 = sctau8 ( B ) ;
     *    const double val9 = sctau9 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::ChildCTauErr
     *  @see LoKi::Cuts::DTF_CTAU
     *  @see LoKi::Cuts::DTF_CTAUSIGNIFICANCE 
     *  @see LoKi::Child::Selector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-03
     */
    typedef LoKi::Particles::ChildCTauErr                         DTF_CTAUERR ;
    // ========================================================================
    /** @typedef DTF_CTAUSIGNIFICANCE 
     *  Evaluate \f$ \frac{c\tau}{\sigma \left( c\tau\right) } \f$ for the 
     *  dauthter particle in the 
     *  decay tree. The actual computation inclued the refit of the 
     *  whole decya tree using DecayTreeFitter by Wouter HULSBERGEN 
     *  @see IDecayTreeFit
     *  @see  DecayTreeFitter::Fitter 
     *
     *  @code
     *
     *    // get the c*tau-significance for the first daughter particle 
     *    const DFT_CTAUSIGNIFICANCE sctau1 = DTF_CTAUSIGNIFICANCE ( 1 , false ) ;
     *
     *    // get the c*tau-significance for the D0 
     *    // from decay tree of beauty particle  
     *    const DFT_CTAUSIGNIFICANCE sctau2 = 
     *                       DTF_CTAUSIGNIFICANCE ( "Xb -> ^D0 ... " , false ) ;
     *
     *    // get the c*tau-significance for the D0 
     *    const DFT_CTAUSIGNIFICANCE sctau3 = 
     *                       DTF_CTAUSIGNIFICANCE ( "D0" == ABSPID , false ) ;
     *
     *    // get the c*tau-significance for the first daughter particle
     *    // with PV-constraint for the decay head  
     *    const DFT_CTAUSIGNIFICANCE sctau4 = 
     *                       DTF_CTAUSIGNIFICANCE ( 1 , true ) ;
     *
     *    // get the c*tau-significance for the D0 
     *    // from decay tree of beauty particle 
     *    // with PV-constraint for the decay head  
     *    const DFT_CTAUSIGNIFICANCE sctau5 = 
     *                       DTF_CTAUSIGNIFICANCE ( "Xb -> ^D0 ... " , true ) ;
     *
     *    // get the c*tau-significance for the D0  
     *    // with PV-constraint for the decay head  
     *    const DFT_CTAUSIGNIFICANCE sctau6 = 
     *                       DTF_CTAUSIGNIFICANCE ( "D0" == ABSPID , true ) ;
     *
     *
     *    std::vector<std::string> constraints ;
     *    constraints.push_back ( "D0"   ) ;
     *    constraints.push_back ( "D+"   ) ;
     *    constraints.push_back ( "D_s+" ) ;
     *
     *    // get the c*tau-significance for the first daughter particle
     *    // with PV-constraint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAUSIGNIFICANCE sctau7 = 
     *                       DTF_CTAUSIGNIFICANCE ( 1 , true , constraints ) ;
     *
     *    // get the c*tau-significance for the D 
     *    // with PV-constraint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAUSIGNIFICANCE sctau8 = 
     *                        DTF_CTAUSIGNIFICANCE ( "Xb -> ^D0 ... " , 
     *                                      true , constraints ) ;
     *
     *    // get the c*tau-significance for the D0  
     *    // with PV-constraint for the decay head  
     *    // and mass-constraints for charm mesons 
     *    const DFT_CTAUSIGNIFICANCE sctau9 = 
     *                        DTF_CTAUSIGNIFICANCE ( "D0" == ABSPID ,
     *                                      true , constraints ) ;
     *
     *    const LHCb::Particle* B = ... ;
     *
     *    const double val1 = sctau1 ( B ) ;
     *    const double val2 = sctau2 ( B ) ;
     *    const double val3 = sctau3 ( B ) ;
     *    const double val4 = sctau4 ( B ) ;
     *    const double val5 = sctau5 ( B ) ;
     *    const double val6 = sctau6 ( B ) ;
     *    const double val7 = sctau7 ( B ) ;
     *    const double val8 = sctau8 ( B ) ;
     *    const double val9 = sctau9 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::ChildCTauErr
     *  @see LoKi::Cuts::DTF_CTAU
     *  @see LoKi::Cuts::DTF_CTAUERR
     *  @see LoKi::Child::Selector
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-03
     */
    typedef LoKi::Particles::ChildCTauSignificance       DTF_CTAUSIGNIFICANCE ;
    // ========================================================================
  } //                                              end of namesapce LoKi::Cuts   
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_PARTICLES36_H
// ============================================================================
