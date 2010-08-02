// $Id$
// ============================================================================
// $URL$
// ============================================================================
#ifndef LOKI_HLTCANDIDATES_H 
#define LOKI_HLTCANDIDATES_H 1
// ============================================================================
// Include files
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltCandidate.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/TriggerTypes.h"
// ============================================================================
/** @file  LoKi/HltCandidates.h
 *
 *  Hlt-Candidates functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 *
 *  $Revision$
 *  Last Modification $Date$ by $Author$ 
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
    class TotalStages
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
    class NStages : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
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
     *  of split of another candidate or stage 
     *  @see LoKi::Cuts::TC_BRANCH 
     *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
     *  @date 2010-08-01
     */
    class Branch
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
    class StageFun : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
    {
    public:
      // ======================================================================
      /** constructor
       *  @param fun the function 
       *  @param slot the slot: 
       *     - 0 corresponds to current stage , 
       *     - negative value corresponds to initianor stage 
       *     - posiitve valye scorresponds to step-back in history
       */
      StageFun ( const LoKi::TriggerTypes::TS_Func& fun       , 
                 const int                          slot  = 0 ) ;
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
      LoKi::TriggerTypes::TS_Fun m_cut ;                // the actual function
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
    class StageCut : public LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate
    {
    public:
      // ======================================================================
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot: 
       *     - 0 corresponds to current stage , 
       *     - negative value corresponds to initianor stage 
       *     - posiitve valye scorresponds to step-back in history
       */
      StageCut ( const LoKi::TriggerTypes::TS_Cuts& fun       , 
                 const int                          slot  = 0 ) ;
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
  } //                                        end of namespace LoKi::Candidates 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTCANDIDATES_H
// ============================================================================
