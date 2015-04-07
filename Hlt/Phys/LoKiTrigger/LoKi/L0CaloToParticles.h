// $Id: L0CaloToParticles.h 2015-03-19 apuignav $
// ============================================================================
#ifndef LOKI_L0CALOTOPARTICLES_H
#define LOKI_L0CALOTOPARTICLES_H 1
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
#include "LoKi/CaloHelperTool.h"
// ============================================================================
// forward declaration
// ============================================================================
class GaudiAlgorithm;
// ============================================================================
/** @file  LoKi/L0CaloToParticles.h
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
    /** @class L0CaloToParticles LoKi/L0CaloToParticles.h
     *  Class to implement conversion of protos to particles in Hlt1
     *  @author Albert Puig (albert.puig@cern.ch)
     *  @date   2015-03-19
     */
    class GAUDI_API L0CaloToParticles
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
      , public LoKi::Hlt1::CaloHelperTool
    {
      public:
        // ======================================================================
        ///   constructor from pid hypothesis, name and LoKi basic functor
        L0CaloToParticles
          ( const std::string& pid          ,
            const std::string& location     ,
            const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&  cuts ) ;
        /// virtual destructor
        virtual ~L0CaloToParticles() ;
        /// clone method ("virtual constructor")
        virtual L0CaloToParticles* clone() const ;
        /// the only essential method
        virtual result_type operator() ( argument a ) const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        L0CaloToParticles();
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
        /// Determine if a L0CaloCandidate is a photon or not
        bool isPhotonCand ( const LHCb::L0CaloCandidate* cand ) const
        {
            const int type = cand->type() ;
            return ( type == L0DUBase::CaloType::Photon || type == L0DUBase::CaloType::Electron ) ;
        } ;
        // ======================================================================
    };
    // ==========================================================================
  } //                                                end of namespace LoKi::Hlt1
  // ============================================================================
  namespace Cuts
  {
    // ==========================================================================
    /** @typedef TC_L0CALOTOPARTICLES
     *  particle maker from L0CaloCandidates
     *
     *  @code
     *
     *  " ... >> TC_L0CALOTOPARTICLES ( 'gamma', 'output', cuts ) >> ... "
     *
     *  @endcode
     *
     *  @see LHCb::L0CaloCandidate
     *  @see LHCb::Particle
     *
     *  @author Albert Puig (albert.puig@epfl.ch)
     *  @date 2015-03-19
     */
    typedef LoKi::Hlt1::L0CaloToParticles              TC_L0CALOTOPARTICLES ;
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ==============================================================================
//                                                                        The END
// ==============================================================================
#endif   // LOKI_L0CALOTOPARTICLES_H
// ==============================================================================
