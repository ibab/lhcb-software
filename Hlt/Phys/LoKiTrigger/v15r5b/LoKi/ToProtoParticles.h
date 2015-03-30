// $Id: ToProtoParticles.h 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
#ifndef LOKI_TOPROTOPARTICLES_H
#define LOKI_TOPROTOPARTICLES_H 1
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
#include "Kernel/IParticle2State.h"
// ============================================================================
// HltBase/Event
// ============================================================================
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
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
/** @file  LoKi/ToProtoParticles.h
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
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class ToProtoParticles LoKi/ToProtoParticles.h
     *  Class to implement conversion of tracks to protoparticles in Hlt1
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date   2014-11-27
     */
    class GAUDI_API ToProtoParticles
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
      , public LoKi::Hlt1::HelperTool
    {
      public:
        // ======================================================================
        ///   constructor with output name
        ToProtoParticles
          ( const std::string& location    ) ;
        /// virtual destructor
        virtual ~ToProtoParticles() ;
        /// clone method ("virtual constructor")
        virtual ToProtoParticles* clone() const ;
        /// the only essential method
        virtual result_type operator() ( argument a ) const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        ToProtoParticles();
        // ======================================================================
      public:
        /// the output selection
        const std::string& location () const { return m_sink.output() ; }
        // ======================================================================
      private:
        // ======================================================================
        /// 'sink': the functor which register the selection in Hlt Data Svc
        LoKi::Hlt1::Sink          m_sink      ;
        // ======================================================================
    };
    // ==========================================================================
  } //                                                end of namespace LoKi::Hlt1
  // ============================================================================
  namespace Cuts
  {
    // ==========================================================================
    /** @typedef TC_TOPROTOPARTICLES
     *  particle maker from tracks
     *
     *  @code
     *
     *  " ... >> TC_TOPROTOPARTICLES ( '' ) >> ... "
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::ProtoParticle
     *  @see LHCb::Particle
     *  @see LoKi::Hlt1::ToParticles
     *  @see LoKi::Hlt1::Hlt1Combiner
     *
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date 2014-11-27
     */
    typedef LoKi::Hlt1::ToProtoParticles                    TC_TOPROTOPARTICLES ;
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ==============================================================================
//                                                                        The END
// ==============================================================================
#endif   // LOKI_TOPROTOPARTICLES_H
// ==============================================================================

