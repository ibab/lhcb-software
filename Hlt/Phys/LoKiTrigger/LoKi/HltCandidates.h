// $Id$
// ============================================================================
#ifndef LOKI_HLTCANDIDATES_H
#define LOKI_HLTCANDIDATES_H 1
// ============================================================================
// Include files
// ============================================================================
#include<iostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// HltBase
// ============================================================================
#include "Event/HltCandidate.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/TriggerTypes.h"
#include "LoKi/Streamers.h"
#include "LoKi/AlgUtils.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/HltTool.h"
// ============================================================================
/** @file  LoKi/HltCandidates.h
 *
 *  Hlt-Candidates functors
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 *
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::Candidates
   *  collection of functors that deal with Hlt::Candidate objects
   *  @see Hlt::Candidate
   *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
   *  @date 2010-08-01
   */
  namespace Candidates
  {
    // ========================================================================
    /** @class TotalStages trivial functor to get number of stages
     *  @see LoKi::Cuts::TC_TOTSTAGES
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-01
     */
    class GAUDI_API TotalStages
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual desctructor
      virtual ~TotalStages () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TotalStages* clone() const ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** trivial functor to counts certain of stages
     *  @see LoKi::Cuts::TC_NSTAGES
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-01
     */
    class GAUDI_API NStages 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
    {
    public:
      // ======================================================================
      /// constructor
      NStages  ( const  LoKi::TriggerTypes::TS_Cuts& cut ) ;
      /// MANDATORY: virtual desctructor
      virtual ~NStages () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual NStages* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      NStages () ;                       // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual predicate
      LoKi::TriggerTypes::TS_Cut m_cut ;                // the actual predicate
      // ======================================================================
    } ;
    // ========================================================================
    /** trivial functor to check if Candidate is 'branch', e.g. resutl
     *  of split of another candidate
     *  @see LoKi::Cuts::TC_BRANCH
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-01
     */
    class GAUDI_API Branch
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual desctructor
      virtual ~Branch () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Branch* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class StageFun
     *  simple meta-functor for stage functors
     *  @see LoKi::Cuts::TC_StFUN
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-01
     */
    class GAUDI_API StageFun 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
    {
    public:
      // ======================================================================
      /** constructor
       *  @param fun  (INPUT) the function
       *  @param slot (INPUT) the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initianor stage
       *     - positive value corresponds to step-back in history
       */
      StageFun ( const LoKi::TriggerTypes::TS_Func& fun           ,
                 const int                          slot  = 0     ) ;
      /// MANDATORY: virtual desctructor
      virtual ~StageFun () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  StageFun* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      StageFun() ;                       // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      LoKi::TriggerTypes::TS_Fun m_cut  ;               // the actual function
      /// the slot
      int                        m_slot ;               // the slot
      // ======================================================================
    } ;
    // ========================================================================
    /** @class StageCut
     *  simple meta-functor for stage functors
     *  @see LoKi::Cuts::TC_StCUT
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-01
     */
    class GAUDI_API StageCut 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate
    {
    public:
      // ======================================================================
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value corresponds to step-back in history
       */
      StageCut ( const LoKi::TriggerTypes::TS_Cuts& fun           ,
                 const int                          slot  = 0     ) ;
      /// MANDATORY: virtual desctructor
      virtual ~StageCut () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  StageCut* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      StageCut () ;                     // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      LoKi::TriggerTypes::TS_Cut m_cut ;                // the actual function
      /// the slot
      int                        m_slot ;               // the slot
      // ======================================================================
    } ;
    // ========================================================================
    /** @class SlotFun
     *  generalized function that applies the function to the content
     *  of the actual slot/stage
     *  @thanks Alexander MAZUROV
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-21
     */
    class GAUDI_API SlotFun 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
    {
      // ======================================================================
    public :
      // ======================================================================
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @param bad  the bad value 
       *  @see Hlt::Candidate::get
       */
      SlotFun
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun ,
        const int     slot = 0     ,
        const double  bad  = 0     ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
      SlotFun
      ( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun ,
        const int     slot = 0 ,
        const double  bad  = 0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
      SlotFun
      ( const LoKi::BasicFunctors<const Hlt::Stage*>::Function& fun ,
        const int    slot = 0 ,
        const double bad  = 0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
      SlotFun
      ( const LoKi::BasicFunctors<const LHCb::Track*>::FunVal& fun ,
        const int    slot = 0 ,
        const double bad  = 0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value corresponds to step-back in history
       *  @see Hlt::Candidate::get
       */
      SlotFun
      ( const LoKi::Hlt1::TrackFunction& fun ,
        const int    slot     ,
        const double bad  = 0 ) ;
      // =====================================================================
      /// MANDATORY: virtual destructor
      virtual ~SlotFun () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SlotFun* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      int     slot () const { return m_slot ; }
      double  bad  () const { return m_bad  ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      SlotFun () ;                       // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      LoKi::FunctorFromFunctor<const Hlt::Stage*,double> m_fun  ; // the cut
      /// the slot
      int                                                m_slot ; // the slot
      /// bad value
      double                                             m_bad  ; // bad value
      // ======================================================================
    } ;
    // ========================================================================
    /** @class SlotCut
     *  generalized predicate that applies the predicate to the content
     *  of the actual slot/stage
     *  @thanks Alexander MAZUROV
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-21
     */
    class GAUDI_API SlotCut 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate
    {
      // ======================================================================
    public :
      // ======================================================================
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value corresponds to step-back in history
       */
      SlotCut
      ( const LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate& cut ,
        const int slot  = -1 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value corresponds to step-back in history
       */
      SlotCut
      ( const LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate& cut ,
        const int slot  = -1 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value corresponds to step-back in history
       */
      SlotCut
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut ,
        const int slot  =  0 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value corresponds to step-back in history
       */
      SlotCut
      ( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate& cut ,
        const int slot  =  0 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value corresponds to step-back in history
       */
      SlotCut
      ( const LoKi::BasicFunctors<const Hlt::Stage*>::Predicate& cut ,
        const int slot  = 0 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value corresponds to step-back in history
       */
      SlotCut
      ( const LoKi::BasicFunctors<const LHCb::Track*>::CutVal& cut ,
        const int slot  = 0 ) ;
      // =====================================================================
      /// MANDATORY: virtual destructor
      virtual ~SlotCut () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SlotCut* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      SlotCut () ;                     // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      LoKi::FunctorFromFunctor<const Hlt::Stage*,bool> m_cut  ; // the cut
      /// the slot
      int                                              m_slot ; // the slot
      // ======================================================================
    } ;
    // =========================================================================
    /** @class StageFilter
     *  simple meta-functor for stage filters
     *  @author Alexander Mazurov alexander.mazurov@gmail.com
     *  @date 2010-10-22
     */
    class GAUDI_API SlotFilter
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
    {
    public:
      // ======================================================================
      /// constructor from the selection
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value corresponds to step-back in history
       */
    	SlotFilter
      ( const LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate& cut,
    		const int slot = -1 );
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotFilter
      ( const LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate& cut,
        const int slot = -1 );
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotFilter
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut,
    		const int slot =  0 );
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotFilter
      ( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate& cut,
    		const int slot =  0 );
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotFilter
      ( const LoKi::BasicFunctors<const Hlt::Stage*>::Predicate& cut,
        const int slot =  0 );
      /// constructor
      SlotFilter ( const SlotCut& cut ) ;
      /// MANDATORY: virtual destructor
      virtual ~SlotFilter ();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SlotFilter* clone() const;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // =======================================================================
    private:
      // =======================================================================
      /// default constructor is disabled
      SlotFilter ();
      // ========================================================================
    private:
      // ========================================================================
      SlotCut m_cut;
      // ========================================================================
    };
    // ==========================================================================
    /** @class SlotMap
     *  generalized function that applies the function to the content
     *  of the actual slot/stage
     *  @thanks Vanya Belyaev
     *  @author Alexander MAZUROV alexander.mazurov@gmail.com
     *  @date 2010-11-21
     */
    class GAUDI_API SlotMap 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Map
    {
    public:
      // ======================================================================
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotMap
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun,
    		const int    slot =  0 ,
        const double bad  =  0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotMap
      ( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun,
    		const int    slot =  0 ,
        const double bad  =  0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotMap
      ( const LoKi::BasicFunctors<const Hlt::Stage*>::Function& fun,
    		const int    slot = 0 ,
        const double bad  = 0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *  @see Hlt::Candidate::get
       */
    	SlotMap
      ( const LoKi::BasicFunctors<const LHCb::Track*>::FunVal& fun,
    		const int    slot = 0 ,
        const double bad  = 0 ) ;
      /// constructor
      SlotMap ( const SlotFun& fun ) ;
      /// MANDATORY: virtual destructor
      virtual ~SlotMap ();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SlotMap* clone() const;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      SlotMap ();
      // ======================================================================
    private:
      // ======================================================================
      SlotFun m_fun;
      // ======================================================================
    };
    // ========================================================================
  } //                                        end of namespace LoKi::Candidates 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_HLTCANDIDATES_H
// ============================================================================
