// $Id: Hlt1Combiner.h 180655 2014-11-25 10:38:48Z mkenzie $
// ============================================================================
#ifndef LOKI_HLT1COMBINER_H
#define LOKI_HLT1COMBINER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IDVAlgorithm.h"
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
  // ==========================================================================
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
    protected:
      // ======================================================================
      /// typedefs
      typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source     ;
      typedef LoKi::Selected_<LHCb::Particle::ConstVector>       Selected   ;
      typedef result_type                                        CANDIDATES ;
      // ======================================================================
    public:
      // ======================================================================
      ///   constructor from output location and Hlt1CombinerConf
      //    @see Loki::Hlt1CombinerConf
      //
      Hlt1Combiner
        ( const std::string&                                             output  ,
          const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
          const std::string&                                             source1 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    ) ;
      Hlt1Combiner
        ( const std::string&                                             output  ,
          const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
          const std::string&                                             source1 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    ,
          const std::string&                                             source2 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut2    );
      Hlt1Combiner
        ( const std::string&                                             output  ,
          const LoKi::Hlt1::Hlt1CombinerConf&                            config  ,
          const std::string&                                             source1 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut1    ,
          const std::string&                                             source2 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut2    ,
          const std::string&                                             source3 ,
          const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate&   cut3    ) ;
      /// virtual destructor
      virtual ~Hlt1Combiner() ;
      /// clone method ("virtual constructor")
      virtual Hlt1Combiner* clone() const ;
      /// the only essential method
      virtual result_type operator() ( /*argument a*/ ) const ;
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
      /// the input source
      const Source&                      source () const { return m_source.func() ; }
      /// the particle combiner tool 
      const IParticleCombiner*           pc () const { return m_pc ; }
      // config
      const LoKi::Hlt1::Hlt1CombinerConf config () const { return m_conf ; }
      /// apply cuts 
      bool combcut    ( const LoKi::ATypes::Combination c ) const
      { return m_conf.combcut    ( c ) ; }
      bool combcut12  ( const LoKi::ATypes::Combination c ) const
      { return m_conf.combcut12  ( c ) ; }
      bool combcut123 ( const LoKi::ATypes::Combination c ) const
      { return m_conf.combcut123 ( c ) ; }
      bool mothcut    ( const LHCb::Particle*           p ) const
      { return m_conf.mothcut    ( p ) ; }
      // ======================================================================
    private: // load input data 
      // ======================================================================
      // user functions
      bool getDaughters 
        ( const CANDIDATES&                 input     , 
          Selected&                         daughters ) const ;
      // ======================================================================
    private: // perform the actual creation of  decay trees 
      // ======================================================================
      unsigned long  
        executeCombineParticles 
        ( CANDIDATES&          output    , 
          const Selected&      daughters , 
          const Decays::Decay& decay     ) const ;
      unsigned long  
        execute3BodyCombination
        ( CANDIDATES&          output    , 
          const Selected&      daughters , 
          const Decays::Decay& decay     ) const ;
      unsigned long  
        execute4BodyCombination
        ( CANDIDATES&          output    , 
          const Selected&      daughters , 
          const Decays::Decay& decay     ) const ;
      // ======================================================================
    private:
      // ======================================================================
      bool tooMuchCandidates ( const size_t nGood, const Decays::Decay& decay ) const
      {
        if ( 0 < m_maxCand && m_maxCand <= nGood )
        {
          Warning ( "MaxCandidates exceeded for '" + decay.toString() + "'",
                    StatusCode::FAILURE, 0 ).ignore() ;
          if ( m_maxCandStop ) { return true ; }  // RETURN 
        }
        return false ;
      }
      bool tooMuchCombinations ( const double ncomb, const Decays::Decay& decay ) const 
      {
        if ( 0 < m_maxComb && m_maxComb <= ncomb )
        {      
          Warning ( "MaxCombinations exceeded for '" + decay.toString() + "'",
                    StatusCode::FAILURE, 0 ).ignore() ;
          if ( m_maxCombStop ) { return true ; } 
        }
        return false ;  
      }
      // ======================================================================
    protected:
      // ======================================================================
      StatusCode                        setup()         ;
      // ======================================================================
    private:
      // ======================================================================
      /// 'sink': the functor which register the selection in Hlt Data Svc
      LoKi::Hlt1::Sink                      m_sink   ;
      /// config
      LoKi::Hlt1::Hlt1CombinerConf          m_conf   ;
      /// source holder
      LoKi::Assignable<Source>::Type        m_source ;
      // tools
      LoKi::Interface<IParticleCombiner>    m_pc     ;
      /// the actual list of decays                         
      std::vector<Decays::Decay>            m_decays ;
      /// local map of all involved daughter particles 
      typedef std::map<LHCb::ParticleID, std::string> MAP ;
      MAP m_items  ;
      // ======================================================================
    private:
      // ======================================================================
      /// my IDValgorithm
      LoKi::Interface<IDVAlgorithm>  m_dvAlg ;  // my IDValgorithm
      // ======================================================================
    private: // shoudld be this stuff be picked from Hlt1CombinerConf? 
      // ======================================================================
      unsigned long             m_maxCand     ;
      bool                      m_maxCandStop ;
      unsigned long             m_maxComb     ;
      bool                      m_maxCombStop ;
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
     *  conf = LoKi.Hlt1.Hlt1CombinedConf( ... ) 
     *  "
     *  " ... >> TC_HLT1COMBINER ( 'output' , conf , source1, cut1 ) >> ... "
     *  " ... >> TC_HLT1COMBINER ( 'output' , conf , source1, cut1, source2, cut2 ) >> ... "
     *  " ... >> TC_HLT1COMBINER ( 'output' , conf , source1, cut1, source2, cut2, source3, cut3 ) >> ... "
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
    // ==========================================================================
  } //                                                end of namespace LoKi::Cuts
  // ============================================================================
} //                                                        end of namespace LoKi
// ==============================================================================
//                                                                        The END
// ==============================================================================
#endif     // LOKI_HLT1COMBINER_H
// ==============================================================================
