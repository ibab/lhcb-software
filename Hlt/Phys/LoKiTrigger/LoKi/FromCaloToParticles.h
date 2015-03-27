// $Id: FromCaloToParticles.h 2015-03-19 apuignav $
// ============================================================================
#ifndef LOKI_FROMCALOTOPARTICLES_H
#define LOKI_FROMCALOTOPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/ParticleProperty.h"
// ============================================================================
// HltBase/Event
// ============================================================================
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Event/L0DUBase.h"
#include "Event/L0CaloCandidate.h"
#include "CaloUtils/CaloParticle.h"
#include "CaloDAQ/ICaloDataProvider.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Hlt1.h"
#include "LoKi/HelperTool.h"
// ============================================================================
// forward declaration
// ============================================================================
class GaudiAlgorithm;
// ============================================================================
/** @file  LoKi/FromCaloToParticles.h
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Albert Puig (albert.puig@cern.ch)
 *  @date   2015-03-19
 *
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class FromCaloToParticles LoKi/FromCaloToParticles.h
     *  Class to implement conversion of protos to particles in Hlt1
     *  @author Albert Puig (albert.puig@cern.ch)
     *  @date   2015-03-19
     */
    class GAUDI_API FromCaloToParticles
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
      , public LoKi::Hlt1::HelperTool
    {
      public:
        // ======================================================================
        ///   constructor from pid hypothesis, name and LoKi basic functor
        FromCaloToParticles
          ( const std::string& pid          ,
            const std::string& location     ,
            const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&  cuts ) ;
        /// virtual destructor
        virtual ~FromCaloToParticles() ;
        /// clone method ("virtual constructor")
        virtual FromCaloToParticles* clone() const ;
        /// the only essential method
        virtual result_type operator() ( argument a ) const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        FromCaloToParticles();
        // ======================================================================
      public:
        /// the output selection
        const std::string& location () const { return m_sink.output() ; }
        // ======================================================================
      private:
        // ======================================================================
        /// 'sink': the functor which register the selection in Hlt Data Svc
        LoKi::Hlt1::Sink          m_sink      ;
        /// members which store pid hypoth and ptcut
        const LHCb::ParticleProperty* m_pp ;
        LoKi::FunctorFromFunctor<const LHCb::Particle*, bool> m_cut;
        /// Calo parameters
        const double m_z ;
        ICaloDataProvider*  m_calo ;
        LHCb::Particle* caloRecoChain( const LHCb::L0CaloCandidate* cand ,
                                       const LHCb::ParticleID particleID ) const ;
        // ======================================================================
    };
    // ==========================================================================
  } //                                                end of namespace LoKi::Hlt1
  // ============================================================================
  namespace Cuts
  {
    // ==========================================================================
    /** @typedef TC_FROMCALOTOPARTICLES
     *  particle maker from L0CaloCandidates
     *
     *  @code
     *
     *  " ... >> TC_FROMCALOTOPARTICLES ( 'gamma', 'output', cuts ) >> ... "
     *
     *  @endcode
     *
     *  @see LHCb::L0CaloCandidate
     *  @see LHCb::Particle
     *
     *  @author Albert Puig (albert.puig@epfl.ch)
     *  @date 2015-03-19
     */
    typedef LoKi::Hlt1::FromCaloToParticles              TC_FROMCALOTOPARTICLES ;
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ==============================================================================
//                                                                        The END
// ==============================================================================
#endif   // LOKI_FROMCALOTOPARTICLES_H
// ==============================================================================
