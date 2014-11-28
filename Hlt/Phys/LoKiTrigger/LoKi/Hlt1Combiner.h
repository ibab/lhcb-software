// $Id: Hlt1Combiner.h 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
#ifndef LOKI_HLT1COMBINER_H
#define LOKI_HLT1COMBINER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticle2State.h"
#include "Kernel/GetDecay.h"
#include "Kernel/GetParticlesForDecay.h"
// ============================================================================
// HltBase/Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
#include "LoKi/HelperTool.h"
#include "LoKi/ToParticles.h"
#include "LoKi/Selected.h"
#include "LoKi/Combiner.h"
#include "LoKi/ATypes.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
// forward declaration
// ============================================================================
class GaudiAlgorithm;
// ============================================================================
/** @file  LoKi/Hlt1Combiner.h
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Matthew KENZIE matthew.kenzie@cern.ch
 *  @date   2014-11-27
 *
 *                    $Revision: 180655 $
 *  Last Modification $Date: 2014-11-25 11:38:48 +0100 (Tue, 25 Nov 2014) $
 *                 by $Author: mkenzie $
 */
// ============================================================================
namespace LoKi
{
  namespace Hlt1
  {
    // ========================================================================
    /** @class Hlt1Combiner LoKi/Hlt1Combiner.h
     *  Class to implement combining particles into mothers in Hlt1
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date   2014-11-27
     */
    class GAUDI_API Hlt1Combiner
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Source
      , public LoKi::Hlt1::HelperTool
    {
      private:
        // ======================================================================
        /// typedefs
        typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
        typedef LoKi::Selected_<LHCb::Particle::ConstVector>  Selected ;
        // ======================================================================
      public:
        // ======================================================================
        ///   constructor from decay string, output location, two Sources for daughters,
        //    combination cut and mother cut
        Hlt1Combiner
          ( std::string                                                       decay       ,
            std::string                                                       output      ,
            const Source&                                                     particles1  ,
            const Source&                                                     particles2  ,
            const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut     ,
            const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut     );
        ///   constructor from decay string, output location, two Sources for daughters (as strings),
        //    combination cut and mother cut
        Hlt1Combiner
          ( std::string                                                       decay       ,
            std::string                                                       output      ,
            std::string                                                       particles1  ,
            std::string                                                       particles2  ,
            const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut     ,
            const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut     );
        /// virtual destructor
        virtual ~Hlt1Combiner() ;
        /// clone method ("virtual constructor")
        virtual Hlt1Combiner* clone() const ;
        /// the only essential method
        virtual result_type operator() ( /* argument a */ ) const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        Hlt1Combiner();
        // ======================================================================
      public:
        // ======================================================================
        /// the output selection
        const std::string&                location () const { return m_sink.output() ; }
        /// the input source 1
        const Source&                     source1 () const { return m_source1.func() ; }
        /// the input source 2
        const Source&                     source2 () const { return m_source2.func() ; }
        /// the decay string decoder tool
        IDecodeSimpleDecayString*         dsds () { return m_dsds ; }
        /// the particle2state tool
        const IParticle2State*            p2s () const { return m_p2s ; }
        /// the particle combiner tool -- FIXME should get this from the HltUnit
        const IParticleCombiner*          pc () const { return m_pc ; }
        // ======================================================================
      private:
        // ======================================================================
        /// decay string holder
        std::string                       m_decstring      ;
        /// 'sink': the functor which register the selection in Hlt Data Svc
        LoKi::Hlt1::Sink                  m_sink           ;
        /// source holders
        LoKi::Assignable<Source>::Type    m_source1        ;
        LoKi::Assignable<Source>::Type    m_source2        ;
        // tools
        IDecodeSimpleDecayString*         m_dsds           ;
        const IParticle2State*            m_p2s            ;
        const IParticleCombiner*          m_pc             ;
        // decay and cuts
        std::vector<Decays::Decay>                                       m_decays ;
        LoKi::FunctorFromFunctor<LoKi::ATypes::Combination, bool>  m_acut;
        LoKi::FunctorFromFunctor<const LHCb::Particle*, bool>      m_cut;
        // user functions
        const LHCb::State*                getState( const LHCb::Track* &track ) const;
        StatusCode                        setup()         ;
        // ======================================================================
    };
    // ==========================================================================
  } //                                                end of namespace LoKi::Hlt1
  namespace Cuts
  {
    // ==========================================================================
    /** @typedef TC_HLT1COMBINER
     *  particle maker from two sources of daughter particles
     *
     *  @code
     *
     *  " ... >> TC_HLT1COMBINER ( '[D0 -> K- pi+]cc', 'output', 'source1' , 'source2' , combcuts , mothcuts ) >> ... "
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::ProtoParticle
     *  @see LHCb::Particle
     *  @see LoKi::ToParticles
     *
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date 2014-11-27
     */
    typedef LoKi::Hlt1::Hlt1Combiner              TC_HLT1COMBINER ;
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ==============================================================================
//                                                                        The END
// ==============================================================================
#endif     // LOKI_HLT1COMBINER_H
// ==============================================================================
