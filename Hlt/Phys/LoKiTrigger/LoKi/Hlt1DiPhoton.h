// $Id: Hlt1DiPhoton.h 2015-03-19 apuignav $
// ============================================================================
#ifndef LOKI_HLT1DIPHOTON_H
#define LOKI_HLT1DIPHOTON_H 1
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
/** @file  LoKi/Hlt1DiPhoton.h
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
    /** @class Hlt1DiPhoton LoKi/Hlt1DiPhoton.h
     *  Class to build di-photon combinations in HLT1
     *  @author Albert Puig (albert.puig@cern.ch)
     *  @date   2015-03-19
     */
    class GAUDI_API Hlt1DiPhoton
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
      , public LoKi::Hlt1::HelperTool
    {
      public:
        // ======================================================================
        ///   constructor from pid hypothesis, name and LoKi basic functor
        Hlt1DiPhoton
          ( const std::string& pid          ,
            const std::string& location     ,
            const double photonEtMin        ,
            const double photonEtSumMin     ,
            const double photonEtAsymMax    ,
            const double diphotonMassMin    ,
            const double diphotonEtMin      ) ;
        /// virtual destructor
        virtual ~Hlt1DiPhoton() ;
        /// clone method ("virtual constructor")
        virtual Hlt1DiPhoton* clone() const ;
        /// the only essential method
        virtual result_type operator() ( argument a ) const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        Hlt1DiPhoton();
        // ======================================================================
      public:
        /// the output selection
        const std::string& location () const { return m_sink.output() ; }
        // ======================================================================
      private:
        // ======================================================================
        /// 'sink': the functor which register the selection in Hlt Data Svc
        LoKi::Hlt1::Sink          m_sink      ;
        /// Output particle
        const LHCb::ParticleProperty* m_pp ;
        /// Cuts
        const double m_photonEtMin ;
        const double m_photonEtSumMin ;
        const double m_photonEtAsymMax ;
        const double m_diPhotonMassSqMin ;
        const double m_diphotonEtMin ;
        /// Calo parameters
        const double m_z ;
        ICaloDataProvider*  m_calo ;
        /// Determine if a L0CaloCandidate is a photon or not
        bool isPhotonCand ( const LHCb::L0CaloCandidate* cand ) const
        {
            const int type = cand->type() ;
            return ( type == L0DUBase::CaloType::Photon || type == L0DUBase::CaloType::Electron ) ;
        } ;
        /// Build particles in the calo
        LHCb::Particle* caloRecoChain( const LHCb::L0CaloCandidate* cand ) const ;
        // ======================================================================
    };
    // ==========================================================================
  } //                                                end of namespace LoKi::Hlt1
  // ============================================================================
  namespace Cuts
  {
    // ==========================================================================
    /** @typedef TC_HLT1DIPHOTON
     *  Build di-photon combinations in HLT1 from L0CaloCandidates
     *
     *  @code
     *
     *  " ... >> TC_HLT1DIPHOTON ( 'gamma', 'output', cuts ) >> ... "
     *
     *  @endcode
     *
     *  @see LHCb::L0CaloCandidate
     *  @see LHCb::Particle
     *
     *  @author Albert Puig (albert.puig@epfl.ch)
     *  @date 2015-03-19
     */
    typedef LoKi::Hlt1::Hlt1DiPhoton                            TC_HLT1DIPHOTON ;
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ==============================================================================
//                                                                        The END
// ==============================================================================
#endif   // LOKI_HLT1DIPHOTON_H
// ==============================================================================
