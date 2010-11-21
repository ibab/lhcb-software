// $Id$
// ============================================================================
#ifndef LOKI_HLTCANDIDATES_H
#define LOKI_HLTCANDIDATES_H 1
// ============================================================================
// Include files
// ============================================================================
#include<iostream>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
// HltBase
// ============================================================================
#include "Event/HltCandidate.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/TriggerTypes.h"
#include "LoKi/Streamers.h"
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
 *  By usage of this code one clearly states the disagreement
 *  with the campain of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
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
       *     - negative value corresponds to initiator stage
       *     - positive value scorresponds to step-back in history
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
    /** @class SlotFun
     *  generalized function that applies the function to the content 
     *  of the actual slot/stage
     *  @thanks Alexander MAZUROV 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-21
     */
    class SlotFun : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
    {
      // ======================================================================
    public :
      // ======================================================================      
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value scorresponds to step-back in history
       *  @see Hlt::Candidate::get 
       */
      SlotFun
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun , 
        const int     slot = 0 ,
        const double  bad  = 0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value scorresponds to step-back in history
       *  @see Hlt::Candidate::get 
       */
      SlotFun
      ( const LoKi::BasicFunctors<const Hlt::Stage*>::Function& fun , 
        const int    slot = 0 ,
        const double bad  = 0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value scorresponds to step-back in history
       *  @see Hlt::Candidate::get 
       */
      SlotFun
      ( const LoKi::BasicFunctors<const Hlt::Candidate*>::Function& fun ,
        const int    slot     , 
        const double bad  = 0 ) ;
      /** constructor
       *  @param fun the function
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value scorresponds to step-back in history
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
    class SlotCut : public LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate
    {
      // ======================================================================
    public :
      // ======================================================================      
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - positive value scorresponds to step-back in history
       */
      SlotCut 
      ( const LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate& cut , 
        const int slot  = 0 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value scorresponds to step-back in history
       */
      SlotCut 
      ( const LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate& cut , 
        const int slot  = 0 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value scorresponds to step-back in history
       */
      SlotCut 
      ( const LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut , 
        const int slot  = 0 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value scorresponds to step-back in history
       */
      SlotCut 
      ( const LoKi::BasicFunctors<const Hlt::Stage*>::Predicate& cut , 
        const int slot  = 0 ) ;
      /** constructor
       *  @param fun the predicate
       *  @param slot the slot:
       *     - 0 corresponds to current stage ,
       *     - negative value corresponds to initiator stage
       *     - posiitve value scorresponds to step-back in history
       */
      SlotCut 
      ( const LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate& cut , 
        const int slot  ) ;
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
    // ========================================================================
    /** @class
     *  simple functor which filter Hlt::Candidates by some trigger condition
     *  @see Hlt::Candidate
     *  @see Hlt::Track
     *  @author alexander.mazurov@cern.ch
     *  @date 2010-10-01
     */
    /*class TracksFilter : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection
      TracksFilter (const
                    LoKi::BasicFunctors<const LHCb::Track*>::Predicate& cut) ;
      /// MANDATORY: virtual destructor
      virtual ~TracksFilter () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TracksFilter* clone() const { return new TracksFilter (*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      TracksFilter () ;
      // ======================================================================
    private:
      // ======================================================================
      LoKi::FunctorFromFunctor<const LHCb::Track*, bool> m_cut;
      // ======================================================================
    };*/

    template<class STAGETYPE>
    class StageSlotFilter :
    					public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
    {
          // ==================================================================
        public:
    		typedef const STAGETYPE* TYPE;
          // ==================================================================
          /// constructor from the selection
    	StageSlotFilter (
             const typename
             	 	 	 LoKi::BasicFunctors<const STAGETYPE*>::Predicate& cut,
             int position = -1)
             :LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe ()
              , m_cut  ( cut  ), m_position(position){}
          /// MANDATORY: virtual destructor
          virtual ~StageSlotFilter () {}
          /// MANDATORY: clone method ("virtual constructor")
          virtual  StageSlotFilter* clone() const
        								{ return new StageSlotFilter (*this) ; }
          /// MANDATORY: the only one essential method
          virtual result_type operator() ( argument a )  const ;
          /// OPTIONAL: the nice printout
          virtual std::ostream& fillStream ( std::ostream& s ) const ;
          // ==================================================================
        public:
          // ==================================================================
          // ==================================================================
        private:
          // ==================================================================
          /// default constructor is disabled
          StageSlotFilter ();
          // ==================================================================
        private:
          // ===============b==================================================
          LoKi::FunctorFromFunctor<TYPE, bool> m_cut;
          int m_position;
          // ==================================================================
    };

    // ========================================================================
    // MANDATORY: the only one essential method
    // ========================================================================
    template<class STAGETYPE>
    inline typename LoKi::Candidates::StageSlotFilter<STAGETYPE>::result_type
    LoKi::Candidates::StageSlotFilter<STAGETYPE>::operator()
      (typename LoKi::Candidates::StageSlotFilter<STAGETYPE>::argument a ) const
    {
    	typedef typename
    	   LoKi::Candidates::StageSlotFilter<STAGETYPE>::argument_type ARGUMENT;
    	typedef typename
    	   LoKi::Candidates::StageSlotFilter<STAGETYPE>::result_type RESULT;
    	RESULT result;
    	// Loop over candidates
    	for (typename ARGUMENT::const_iterator
    	  cur = a.begin(); cur != a.end(); cur++){
    	 const SmartRefVector<Hlt::Stage>& stages = (*cur)->stages() ;
    	 	 int pos = -1 == m_position?stages.size()-1:m_position;
    	 	 if ((pos < 0) || (pos >= int(stages.size()))) continue;
    	 	 const Hlt::Stage* stage = stages[pos];
    	 	 if ((NULL == stage) || !stage->is<STAGETYPE>()) continue;

    	 	 if (m_cut(stage->get<STAGETYPE>())){
    	 		 result.push_back(*cur);
    	 	 }
    	}

    	return result;
    }
    // ========================================================================
    // OPTIONAL: the nice printout
    // ========================================================================
    template<class STAGETYPE> inline
    std::ostream& LoKi::Candidates::StageSlotFilter<STAGETYPE>::fillStream
    ( std::ostream& s ) const
    {
      return s << "TC_FILTER(" << m_cut << ")";
    		  //              << this  -> objType()
              //<< " type of cut : "
              // << m_cut .  objType() ;

    }


    // ========================================================================
  } //                                        end of namespace LoKi::Candidates
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================


template <class TYPE,class TYPE1> inline
typename boost::disable_if<boost::is_same<TYPE1,Hlt::Candidate::ConstVector> ,
			LoKi::FunctorFromFunctor<TYPE, Hlt::Candidate::ConstVector> >::type
operator >> ( const LoKi::Functor<TYPE, Hlt::Candidate::ConstVector>& left ,
              const LoKi::Functor<TYPE1,bool>& right )
{
  typedef typename
    boost::remove_const<typename boost::remove_pointer<TYPE1>::type>::type
    																  STAGETYPE;
  return left >> LoKi::Candidates::StageSlotFilter<STAGETYPE> ( right );
}

/*
template <class TYPE> inline
LoKi::FunctorFromFunctor<TYPE, Hlt::Candidate::ConstVector>
operator >> ( const LoKi::Functor<TYPE, Hlt::Candidate::ConstVector>& left ,
        const LoKi::Functor<const LHCb::L0MuonCandidate*, bool>& right ){
	return left >>
			LoKi::Candidates::NStageFilter<LHCb::L0MuonCandidate> ( right );
}

template <class TYPE> inline
LoKi::FunctorFromFunctor<TYPE, Hlt::Candidate::ConstVector>
operator >> ( const LoKi::Functor<TYPE, Hlt::Candidate::ConstVector>& left ,
        const LoKi::Functor<const LHCb::L0CaloCandidate*, bool>& right ){
	return left >>
			LoKi::Candidates::NStageFilter<LHCb::L0CaloCandidate> ( right );
}

template <class TYPE> inline
LoKi::FunctorFromFunctor<TYPE, Hlt::Candidate::ConstVector>  
operator >> ( const LoKi::Functor<TYPE, Hlt::Candidate::ConstVector>& left ,
        const LoKi::Functor<const LHCb::Track*, bool>& right ){
	return left >> LoKi::Candidates::NStageFilter<LHCb::Track> ( right );
}

template <class TYPE> inline
LoKi::FunctorFromFunctor<TYPE, Hlt::Candidate::ConstVector>
operator >> ( const LoKi::Functor<TYPE, Hlt::Candidate::ConstVector>& left ,
        const LoKi::Functor<const LHCb::RecVertex*, bool>& right ){
	return left >>
			LoKi::Candidates::NStageFilter<LHCb::RecVertex> ( right );
}

template <class TYPE> inline
LoKi::FunctorFromFunctor<TYPE, Hlt::Candidate::ConstVector>
operator >> ( const LoKi::Functor<TYPE, Hlt::Candidate::ConstVector>& left ,
        const LoKi::Functor<const Hlt::MultiTrack*, bool>& right ){
	return left >>
			LoKi::Candidates::NStageFilter<Hlt::MultiTrack> ( right );
}

template <class TYPE> inline
LoKi::FunctorFromFunctor<TYPE, Hlt::Candidate::ConstVector>
operator >> ( const LoKi::Functor<TYPE, Hlt::Candidate::ConstVector>& left ,
        const LoKi::Functor<const Hlt::Stage*, bool>& right ){
	return left >>
			LoKi::Candidates::NStageFilter<Hlt::Stage> ( right );
}
*/


// ============================================================================
// The END
// ============================================================================
#endif // LOKI_HLTCANDIDATES_H
// ============================================================================
