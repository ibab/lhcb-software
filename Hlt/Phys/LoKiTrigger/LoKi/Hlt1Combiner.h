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
#include "LoKi/Hlt1CombinerConf.h"
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
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
      , public LoKi::Hlt1::HelperTool
    {
      protected:
        // ======================================================================
        /// typedefs
        typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
        typedef LoKi::Selected_<LHCb::Particle::ConstVector>  Selected ;
        // ======================================================================
      public:
        // ======================================================================
        ///   constructor from output location and Hlt1CombinerConf
        //    @see Loki::Hlt1CombinerConf
        Hlt1Combiner
          ( const std::string                        output      ,
            const LoKi::Hlt1::Hlt1CombinerConf&      config      ) ;
        /// virtual destructor
        virtual ~Hlt1Combiner() ;
        /// clone method ("virtual constructor")
        virtual Hlt1Combiner* clone() const ;
        /// the only essential method
        virtual result_type operator() ( argument a ) const ;
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
        const std::string&                 location () const { return m_sink.output() ; }
        /// the particle combiner tool -- FIXME should get this from the HltUnit
        const IParticleCombiner*           pc () const { return m_pc ; }
        // config
        const LoKi::Hlt1::Hlt1CombinerConf config () const { return m_conf ; }
        /// apply cuts
        bool combcut ( const LoKi::ATypes::Combination c ) const
        { return m_conf.combcut ( c ) ; }
        bool mothcut ( const LHCb::Particle* p ) const
        { return m_conf.mothcut ( p ) ; }
        // ======================================================================
      private:
        // ======================================================================
        /// 'sink': the functor which register the selection in Hlt Data Svc
        LoKi::Hlt1::Sink                  m_sink           ;
        /// config
        LoKi::Hlt1::Hlt1CombinerConf      m_conf           ;
        // tools
        const IParticleCombiner*          m_pc             ;
        // user functions
        virtual bool                      getDaughters( Selected& daughters, const result_type* arg1 ) const;
        // ======================================================================
      protected:
        // ======================================================================
        StatusCode                        setup()         ;
        // ======================================================================
    };
    // ==========================================================================
    /** @class Hlt1Combiner2 LoKi/Hlt1Combiner.h
     *  Class to implement combining particles into mothers in Hlt1 using
     *  a second source
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date   2014-11-27
     */
    class GAUDI_API Hlt1Combiner2
      : public Hlt1Combiner
    {
      public:
        // ======================================================================
        ///   constructor from output location, Hlt1CombinerConf and 2nd source
        //    @see Loki::Hlt1CombinerConf
        Hlt1Combiner2
          ( const std::string                        output      ,
            const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
            const Source&                            particles2  ) ;
        ///   constructor from output location, Hlt1CombinerConf and 2nd source
        //    @see Loki::Hlt1CombinerConf
        Hlt1Combiner2
          ( const std::string                        output      ,
            const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
            const std::string                        particles2  ) ;
        /// virtual destructor
        virtual ~Hlt1Combiner2() ;
        /// clone method ("virtual constructor")
        virtual Hlt1Combiner2* clone() const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        Hlt1Combiner2();
        // ======================================================================
      public:
        // ======================================================================
        /// the input source 2
        const Source&                      source2 () const { return m_source2.func() ; }
        // ======================================================================
      private:
        /// 2nd source holder
        LoKi::Assignable<Source>::Type    m_source2        ;
        // ======================================================================
        // user functions
        virtual bool                      getDaughters( Selected& daughters, const result_type* arg1 ) const;
        // ======================================================================
    };
    // ========================================================================
    /** @class Hlt1Combiner3 LoKi/Hlt1Combiner.h
     *  Class to implement combining particles into mothers in Hlt1
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date   2014-11-27
     */
    class GAUDI_API Hlt1Combiner3
      : public Hlt1Combiner
    {
      public:
        // ======================================================================
        ///   constructor from output location, Hlt1CombinerConf, 2nd and 3rd source
        //    @see Loki::Hlt1CombinerConf
        Hlt1Combiner3
          ( const std::string                        output      ,
            const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
            const Source&                            particles2  ,
            const Source&                            particles3  ) ;
        ///   constructor from output location, Hlt1CombinerConf, 2nd and 3rd source
        //    @see Loki::Hlt1CombinerConf
        Hlt1Combiner3
          ( const std::string                        output      ,
            const LoKi::Hlt1::Hlt1CombinerConf&      config      ,
            const std::string                        particles2  ,
            const std::string                        particles3  ) ;
        /// virtual destructor
        virtual ~Hlt1Combiner3() ;
        /// clone method ("virtual constructor")
        virtual Hlt1Combiner3* clone() const ;
        /// nice printout
        virtual std::ostream& fillStream ( std::ostream& s ) const;
        // ======================================================================
      private:
        // ======================================================================
        /// the default constructor is disabled
        Hlt1Combiner3();
        // ======================================================================
      public:
        // ======================================================================
        /// the input source 2
        const Source&                      source2 () const { return m_source2.func() ; }
        /// the input source 3
        const Source&                      source3 () const { return m_source3.func() ; }
        // ======================================================================
      private:
        /// 2nd source holder
        LoKi::Assignable<Source>::Type    m_source2        ;
        /// 3rd source holder
        LoKi::Assignable<Source>::Type    m_source3        ;
        // ======================================================================
        // user functions
        virtual bool                      getDaughters( Selected& daughters, const result_type* arg1 ) const;
        // ======================================================================
    };
    // ==========================================================================
  } //                                                end of namespace LoKi::Hlt1
  namespace Cuts
  {
    // ==========================================================================
    /** @typedef TC_HLT1COMBINER
     *  particle maker from multiple sources of daughter particles
     *
     *  @code
     *
     *  "
     *  " ... >> TC_HLT1COMBINER ( 'output', LoKi.Hlt1.Hlt1CombinerConf() ) >> ... "
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::ProtoParticle
     *  @see LHCb::Particle
     *  @see LoKi::ToParticles
     *  @see LoKi::Hlt1CombinerConf
     *
     *  @author Matthew KENZIE matthew.kenzie@cern.ch
     *  @date 2014-11-27
     */
    typedef LoKi::Hlt1::Hlt1Combiner                           TC_HLT1COMBINER  ;
    typedef LoKi::Hlt1::Hlt1Combiner2                          TC_HLT1COMBINER2 ;
    typedef LoKi::Hlt1::Hlt1Combiner3                          TC_HLT1COMBINER3 ;
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ============================================================================
//                                                                        The END
// ==============================================================================
#endif     // LOKI_HLT1COMBINER_H
// ==============================================================================
