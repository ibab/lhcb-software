#ifndef LOKI_HLT1COMBINER_H
#define LOKI_HLT1COMBINER_H 1

#include "GaudiKernel/Kernel.h"

#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticle2State.h"
#include "Kernel/GetDecay.h"
#include "Kernel/GetParticlesForDecay.h"

#include "Event/Particle.h"

#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
#include "LoKi/HelperTool.h"
#include "LoKi/ToParticles.h"
#include "LoKi/Selected.h"
#include "LoKi/Combiner.h"
#include "LoKi/ATypes.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"

class GaudiAlgorithm;

namespace LoKi
{
  namespace Hlt1
  {
    class GAUDI_API Hlt1Combiner
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Source
      , public LoKi::Hlt1::HelperTool
    {

      private:

        typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
        typedef LoKi::Selected_<LHCb::Particle::ConstVector>  Selected ;

      public:

        Hlt1Combiner
          ( std::string                                                       decay       ,
            std::string                                                       output      ,
            const Source&                                                     particles1  ,
            const Source&                                                     particles2  ,
            const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut     ,
            const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut     );
            //std::string           combcut     ,
            //std::string           mothcut     );

        Hlt1Combiner
          ( std::string                                                       decay       ,
            std::string                                                       output      ,
            std::string                                                       particles1  ,
            std::string                                                       particles2  ,
            const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate&  combcut     ,
            const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&      mothcut     );

        virtual ~Hlt1Combiner() ;
        virtual Hlt1Combiner* clone() const ;
        virtual result_type operator() ( /* argument a */ ) const ;
        virtual std::ostream& fillStream ( std::ostream& s ) const;

      private:

        Hlt1Combiner();

      public:

        const std::string&                location () const { return m_sink.output() ; }
        const Source&                     source1 () const { return m_source1.func() ; }
        const Source&                     source2 () const { return m_source2.func() ; }
        IDecodeSimpleDecayString*         dsds () { return m_dsds ; }
        const IParticle2State*            p2s () const { return m_p2s ; }
        const IParticleCombiner*          pc () const { return m_pc ; }

      private:

        // data members
        std::string                       m_decstring      ;
        LoKi::Hlt1::Sink                  m_sink           ;
        LoKi::Assignable<Source>::Type    m_source1        ;
        LoKi::Assignable<Source>::Type    m_source2        ;

        // tools
        IDecodeSimpleDecayString*         m_dsds           ;
        const IParticle2State*            m_p2s            ;
        const IParticleCombiner*          m_pc             ;

        // decay and cuts
        std::vector<Decays::Decay>                                       m_decays ;
        //const LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate  m_acut   ;
        //const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate      m_cut    ;
        LoKi::FunctorFromFunctor<LoKi::ATypes::Combination, bool>  m_acut;
        LoKi::FunctorFromFunctor<const LHCb::Particle*, bool>      m_cut;

        // user functions
        const LHCb::State*                getState( const LHCb::Track* &track ) const;
        StatusCode                        setup()         ;

    };
  } // end of namespace LoKi::Hlt1
  namespace Cuts
  {
    typedef LoKi::Hlt1::Hlt1Combiner              TC_HLT1COMBINER ;
  } // end of namespace LoKi::Cuts
} // end of namespace LoKi
#endif
