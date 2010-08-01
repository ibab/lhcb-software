// $Id: $
// ============================================================================
#ifndef LOKI_HLTSTAGES_H 
#define LOKI_HLTSTAGES_H 1
// ============================================================================
// Include files
// ============================================================================
// HltBsse 
// ============================================================================
#include "Event/HltStage.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/TrackTypes.h"
// ============================================================================
/** @file  LoKi/HltStages.h
 *
 *  Hlt-Stages functors 
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
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Stages
   *  collection of functors that deal with Hlt::Stage objects 
   *  @see Hlt::Stage
   *  @author Vanya Belyaev Ivan.BElyaev@nikhef.nl
   *  @date 2010-08-01
   */
  namespace Stages
  {
    // ========================================================================
    /** @class IsTrack
     *  trivial predicate to check if the stage is a track
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_TRACK 
     *  @date 2010-08-01
     */
    class IsTrack 
      : public LoKi::BasicFunctors<const Hlt::Stage*>::Predicate
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsTrack() ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsTrack* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class IsL0Muon
     *  trivial predicate to check if the stage is a L0-muon stage 
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_L0MUON 
     *  @date 2010-08-01
     */
    class IsL0Muon : public IsTrack
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsL0Muon () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsL0Muon* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class IsL0DiMuon
     *  trivial predicate to check if the stage is a L0-dimuon candidate 
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_L0DIMUON 
     *  @date 2010-08-01
     */
    class IsL0DiMuon : public IsL0Muon
    { 
    public:
      // =======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsL0DiMuon () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsL0DiMuon* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class IsL0Calo
     *  trivial predicate to check if the stage is a L0-calo candidate 
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_L0CALO
     *  @date 2010-08-01
     */
    class IsL0Calo : public IsL0DiMuon
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsL0Calo () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsL0Calo* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class IsVertex
     *  trivial predicate to check if the stage is a RecVertex
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_VERTEX
     *  @date 2010-08-01
     */
    class IsVertex : public IsL0Calo
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsVertex () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsVertex* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class IsMultiTrack
     *  trivial predicate to check if the stage is a Multitrack
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_MULTITRACK
     *  @date 2010-08-01
     */
    class IsMultiTrack : public IsVertex
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsMultiTrack () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsMultiTrack* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsStage
     *  trivial predicate to check if the stage is a Stage
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_STAGE
     *  @date 2010-08-01
     */
    class IsStage : public IsMultiTrack
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsStage () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsStage* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsCandidate
     *  trivial predicate to check if the stage is a Candidate
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_CANDIDATE
     *  @date 2010-08-01
     */
    class IsCandidate : public IsStage
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~IsCandidate () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  IsCandidate* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Type
     *  trivial function to get stage type 
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_TYPE 
     *  @date 2010-08-01
     */
    class Type 
      : public LoKi::BasicFunctors<const Hlt::Stage*>::Function
    { 
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Type () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  Type* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class TrFun
     *  trivial adaptor for track-stage  
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_TRFUN
     *  @date 2010-08-01
     */
    class TrFun 
      : public LoKi::BasicFunctors<const Hlt::Stage*>::Function
    { 
    public:
      // ======================================================================
      /// constructor 
      TrFun ( const LoKi::TrackTypes::TrFunc& fun , 
              const double                    bad ) ;      
      /// constructor 
      TrFun ( const LoKi::TrackTypes::TrFunc& fun ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrFun() ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  TrFun* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrFun() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      LoKi::TrackTypes::TrFun m_fun ;
      double                  m_bad ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class TrCut
     *  trivial adaptor for track-stage  
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_TrCUT
     *  @date 2010-08-01
     */
    class TrCut
      : public LoKi::BasicFunctors<const Hlt::Stage*>::Predicate
    { 
    public:
      // ======================================================================
      /// consttuctor 
      TrCut ( const LoKi::TrackTypes::TrCuts& fun , 
              const bool                      bad ) ;      
      /// constructor 
      TrCut ( const LoKi::TrackTypes::TrCuts& fun ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrCut () ;
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  TrCut* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrCut() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      LoKi::TrackTypes::TrCut m_fun ;
      bool                    m_bad ;
      // ======================================================================
    };      
    // ========================================================================
    /** @class Locked
     *  trivial predicate to check Hlt::Stage::locked 
     *  @author Vanya BELAYEV Ivan.Belyaev@nikhef.nl
     *  @see LoKi::Cuts::TS_LOCKED
     *  @date 2010-08-01
     */
    class Locked
      : public LoKi::BasicFunctors<const Hlt::Stage*>::Predicate
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual descructor 
      virtual ~Locked() ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Locked* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument a ) const ;
      /// OPTIONAL: the ince printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Stages 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLTSTAGES_H
// ============================================================================
