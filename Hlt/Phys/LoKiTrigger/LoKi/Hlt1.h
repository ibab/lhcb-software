// $Id$
// ============================================================================
// $URL$
// ============================================================================
#ifndef LOKI_HLT1_H 
#define LOKI_HLT1_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltSelection.h"
#include "HltBase/IHltUnit.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltCandidate.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/TriggerTypes.h"
// ============================================================================
/** @file  LoKi/Hlt1.
 *  Collection of Hlt1-related functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class Selection 
     *  Get "selection" from Hlt-service
     *  @see LoKi::Cuts::TC_SELECTION
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     */
    class Selection 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Source
    {
    public:
      // ======================================================================
      /// constructor from the selection 
      Selection ( const Hlt::TSelection<Hlt::Candidate>* selection ) ;
      /// constructor from the selection 
      Selection ( const Hlt::Selection*                  selection ) ;
      /// constructor from the selection 
      Selection ( const std::string&                     selection ) ;
      /// constructor from the selection & criteria 
      Selection
      ( const std::string&                                           selection , 
        const LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate& cut       ) ;
      /// constructor from the selection & criteria 
      Selection
      ( const std::string&                                           selection , 
        const LoKi::BasicFunctors<const Hlt::Stage*>::Predicate&     cut       ,
        const int                                                    slot = 0  ) ;
      /// constructor from the selection & criteria 
      Selection
      ( const std::string&                                           selection  , 
        const LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate& cut , 
        const int                                                    slot = -1  ) ;
      /// constructor from the selection & criteria 
      Selection
      ( const std::string&                                           selection  , 
        const LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate& cut ,
        const int                                                    slot = -1  ) ;
      /// constructor from the selection & criteria 
      Selection
      ( const std::string&                                           selection , 
        const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    cut       , 
        const int                                                    slot =  0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Selection() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Selection* clone() const { return new Selection(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ()  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<Hlt::Candidate>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string& selName   () const { return m_selName.str() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      Selection () ;                         // default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      const Hlt::TSelection<Hlt::Candidate>*               m_selection ;
      /// the selection key 
      Gaudi::StringKey                                     m_selName   ;
      /// the actual predicate 
      LoKi::FunctorFromFunctor<const Hlt::Candidate*,bool> m_cut       ;
      /// trivial cut?
      bool                                                 m_trivial   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Sink
     *  simple functor which register its input in Hlt Store 
     *  Tracks are copied to Hlt::IData
     *  @see Hlt::IData
     *  @see Hlt::IUnit
     *  @see LoKi::Cuts::TC_SINK
     *  @see LoKi::Cuts::SINK
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class Sink : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      Sink ( const std::string&  selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Sink () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Sink* clone() const { return new Sink (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<Hlt::Candidate>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string&      selName() const { return m_selName.str() ; }
      const Gaudi::StringKey& output () const { return m_selName       ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      Sink () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      Hlt::TSelection<Hlt::Candidate>* m_selection ;
      Gaudi::StringKey                 m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class TrSelection
     *  simple functor which acts as a source for the tracks
     *  Tracks are extracted from Hlt::IData
     *  @see Hlt::IData
     *  @see Hlt::IUnit
     *  @see LoKi::Cuts::TsSELECTION 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class TrSelection : public LoKi::BasicFunctors<const LHCb::Track*>::Source
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      TrSelection ( const Hlt::TSelection<LHCb::Track>* selection ) ;
      /// constructor from the selection 
      TrSelection ( const std::string&  selection ) ;
      /// constructor from the selection 
      TrSelection ( const Hlt::Selection* selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrSelection() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrSelection* clone() const { return new TrSelection(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ()  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::Track>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string& selName   () const { return m_selName.str() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      TrSelection () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      const Hlt::TSelection<LHCb::Track>* m_selection ;
      Gaudi::StringKey                    m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class TrRegister
     *  simple functor which register its input in Hlt Store 
     *  Tracks are copied to Hlt::IData
     *  @see Hlt::IData
     *  @see Hlt::IUnit
     *  @see LoKi::Cuts::TrREGISTER
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class TrRegister : public LoKi::BasicFunctors<const LHCb::Track*>::Pipe
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      TrRegister ( const std::string&  selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrRegister() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrRegister* clone() const { return new TrRegister(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::Track>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string&      selName() const { return m_selName.str() ; }
      const Gaudi::StringKey& output () const { return m_selName       ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      TrRegister() ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      Hlt::TSelection<LHCb::Track>* m_selection ;
      Gaudi::StringKey              m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class RvSelection 
     *  simple functor which acts as a source for the vertices
     *  Vertices are extracted from Hlt::IData
     *  @see Hlt::IData
     *  @see Hlt::IUnit
     *  @see LoKi::Cuts::RvSELECTION
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class RvSelection : public LoKi::BasicFunctors<const LHCb::RecVertex*>::Source
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      RvSelection ( const Hlt::TSelection<LHCb::RecVertex>* selection ) ;
      /// constructor from the selection 
      RvSelection ( const std::string&  selection ) ;
      /// constructor from the selection 
      RvSelection ( const Hlt::Selection* selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RvSelection() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RvSelection* clone() const { return new RvSelection(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ()  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::RecVertex>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string& selName   () const { return m_selName.str() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      RvSelection () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      const Hlt::TSelection<LHCb::RecVertex>* m_selection ;
      Gaudi::StringKey                        m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class RvRegister
     *  simple functor which register its input in Hlt Store 
     *  Vertices are copied to Hlt::IData
     *  @see IHltDataSvc
     *  @see Hlt::IUnit
     *  @see LoKi::Cuts::RvREGISTER
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class RvRegister : public LoKi::BasicFunctors<const LHCb::RecVertex*>::Pipe
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      RvRegister ( const std::string&  selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RvRegister() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RvRegister* clone() const { return new RvRegister(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::RecVertex>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string&     selName() const { return m_selName.str(); }
      const Gaudi::StringKey& output() const { return m_selName      ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      RvRegister() ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      Hlt::TSelection<LHCb::RecVertex>* m_selection ;
      Gaudi::StringKey                  m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class TrTESInput
     *
     *  The special Hlt1 "source-from-TES"
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-07-13
     */
    class TrTESInput
      : public LoKi::BasicFunctors<const LHCb::Track*>::Source 
    {
    public: 
      // ======================================================================
      /// constructor from the key and cuts 
      TrTESInput ( const std::string&         key  , 
                   const LoKi::Types::TrCuts& cuts ) ;
      /// constructor from the key aand cuts 
      TrTESInput ( const LoKi::Types::TrCuts& cuts ,
                   const std::string&         key  ) ;
      /// constructor from the key and cuts 
      TrTESInput ( const std::string&         key  ) ;
      /// MANDATORY: virtual destructor
      virtual ~TrTESInput () ;                      // MANDATORY: virtual destructor
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrTESInput* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ()  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    public:      
      // ======================================================================      
      /// get the unit 
      const Hlt::IUnit*      unit () const { return m_unit ; }  // get unit 
      /// get the key 
      const std::string&         key  () const { return m_key  ; }  //  get key 
      /// get the cuts 
      const LoKi::Types::TrCuts& cuts () const { return m_cut  ; }  //  get cut 
      /// get the cuts 
      const LoKi::Types::TrCuts& cut  () const { return m_cut  ; }  //  get cut 
      // ======================================================================      
    private:
      // ======================================================================
      /// The Unit 
      LoKi::Interface<Hlt::IUnit>  m_unit ;                     // The Unit 
      /// The Key 
      Hlt::IUnit::Key              m_key  ;                     //  The Key
      /// The Cut 
      LoKi::Types::TrCut               m_cut  ;                     //  The Cut
      // ======================================================================      
    };   
    // ========================================================================
    /** @class RvTESInput
     *
     *  The special Hlt1 "source-from-TES"
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-07-13
     */
    class RvTESInput
      : public LoKi::BasicFunctors<const LHCb::RecVertex*>::Source 
    {
    public: 
      // ======================================================================
      /// constructor from the key and cuts 
      RvTESInput ( const std::string&         key  , 
                   const LoKi::Types::RVCuts& cuts ) ;
      /// constructor from the key aand cuts 
      RvTESInput ( const LoKi::Types::RVCuts& cuts ,
                   const std::string&         key  ) ;
      /// constructor from the key and cuts 
      RvTESInput ( const std::string&         key  ) ;
      /// MANDATORY: virtual destructor
      virtual ~RvTESInput () ;                 // MANDATORY: virtual destructor
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RvTESInput* clone() const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ()  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    public:      
      // ======================================================================      
      /// get the unit 
      const Hlt::IUnit*      unit () const { return m_unit ; }  // get unit 
      /// get the key 
      const std::string&         key  () const { return m_key  ; }  //  get key 
      /// get the cuts 
      const LoKi::Types::RVCuts& cuts () const { return m_cut  ; }  //  get cut 
      /// get the cuts 
      const LoKi::Types::RVCuts& cut  () const { return m_cut  ; }  //  get cut 
      // ======================================================================      
    private:
      // ======================================================================
      /// The Unit 
      LoKi::Interface<Hlt::IUnit>  m_unit ;                     // The Unit 
      /// The Key 
      Hlt::IUnit::Key              m_key  ;                     //  The Key
      /// The Cut 
      LoKi::Types::RVCut               m_cut  ;                     //  The Cut
      // ======================================================================      
    };   
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLT1_H
// ============================================================================
