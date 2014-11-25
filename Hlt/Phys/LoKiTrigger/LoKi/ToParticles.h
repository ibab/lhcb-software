#ifndef LOKI_TOPARTICLES_H
#define LOKI_TOPARTICLES_H 1

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "Kernel/IParticle2State.h"

#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Hlt1.h"
#include "LoKi/HelperTool.h"

class GaudiAlgorithm;

namespace LoKi
{
  namespace Hlt1
  {

    class GAUDI_API ToParticles
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
      , public LoKi::Hlt1::HelperTool
    {

      public:

        ToParticles
          ( std::string           pid          ,
            std::string           location     ,
            const double          ptcut        ) ;

        virtual ~ToParticles() ;
        virtual ToParticles* clone() const ;
        virtual result_type operator() ( argument a ) const ;
        virtual std::ostream& fillStream ( std::ostream& s ) const;

      private:

        ToParticles();

      public:

        const std::string& location () const { return m_sink.output() ; }
        double ptcut() const { return m_ptcut; }

      private:

        LoKi::Hlt1::Sink          m_sink  ;
        LHCb::ParticleID          m_pid   ;
        double&                   m_ptcut ;

    };
  } // end of namespace LoKi::Hlt1
  namespace Cuts
  {
    typedef LoKi::Hlt1::ToParticles                   TC_TOPARTICLES ;
  } // end of namespace LoKi::Cuts
} // end of namespace LoKi
#endif
