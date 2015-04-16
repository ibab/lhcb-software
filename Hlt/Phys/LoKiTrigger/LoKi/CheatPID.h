// $Id: CheatPID.h 2015-03-19 apuignav $
// ============================================================================
#ifndef LOKI_CHEATPID_H
#define LOKI_CHEATPID_H 1
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
/** @file  LoKi/CheatPID.h
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
    /** @class CheatPID LoKi/CheatPID.h
     *  Class to implementchange of PID of a composite particle
     *  @author Albert Puig (albert.puig@cern.ch)
     *  @date   2015-03-19
     */
    class GAUDI_API CheatPID
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
      , public LoKi::Hlt1::HelperTool
    {
      public:
        // ======================================================================
        ///   constructor from pid hypothesis, name and LoKi basic functor
        CheatPID
          ( const std::map<std::string,std::string> pids ,
            const std::string& location                  ) ;
        /// virtual destructor
        virtual ~CheatPID() ;
        /// clone method ("virtual constructor")
        virtual CheatPID* clone() const ;
        /// the only essential method
        virtual result_type operator() ( argument a ) const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        CheatPID();
        // ======================================================================
      public:
        /// the output selection
        const std::string& location () const { return m_sink.output() ; }
        // ======================================================================
      private:
        // ======================================================================
        /// 'sink': the functor which register the selection in Hlt Data Svc
        LoKi::Hlt1::Sink          m_sink      ;
        /// pid substitutions to perform
        std::map<LHCb::ParticleID, LHCb::ParticleID> m_pidSubs ;
        // ======================================================================
    };
    // ==========================================================================
  } //                                                end of namespace LoKi::Hlt1
  // ============================================================================
  namespace Cuts
  {
    // ==========================================================================
    /** @typedef TC_CHEATPID
     *  Change PID of composite particle.
     *
     *  @code
     *
     *  " ... >> TC_CHEATPID ( 'gamma', 'output', cuts ) >> ... "
     *
     *  @endcode
     *
     *  @see LHCb::L0CaloCandidate
     *  @see LHCb::Particle
     *
     *  @author Albert Puig (albert.puig@epfl.ch)
     *  @date 2015-03-19
     */
    typedef LoKi::Hlt1::CheatPID              TC_CHEATPID ;
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ==============================================================================
//                                                                        The END
// ==============================================================================
#endif   // LOKI_CHEATPID_H
// ==============================================================================
