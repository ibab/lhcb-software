// $Id: Odin.h,v 1.4 2010-05-17 16:01:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ODIN_H 
#define LOKI_ODIN_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <utility>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Time.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/RoutingBits.h"
#include "LoKi/EvtNum.h"
// ============================================================================
// DAQEvent
// ============================================================================
#include "Event/ODIN.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::Odin
   * The namespace to keep all ODIN-related LoKi functors 
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  @see LHCb::ODIN
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-09-16
   * 
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  namespace Odin
  {
    // ======================================================================
    /// the private state 
    enum Flag { One , Range , List } ;                   // the private state 
    // ========================================================================
    /** @class Run
     *  The trivial function with returnn run-number
     *  @see LHCb::ODIN::runNumber 
     *  @see LoKi::Cuts::ODIN_RUN
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class Run : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Run() { }
      /// MANDATORY: virtual destructor 
      virtual ~Run(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Run* clone() const { return new Run(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> runNumber() ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_RUN" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Evt1
     *  The trivial function with return even number-number (modulo)
     *  @see LHCb::ODIN::eventNumber % modulo  
     *  @see LoKi::Cuts::ODIN_EVT1
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-03
     */
    class Evt1: public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// constructor from the modulo 
      Evt1 ( const unsigned long long modulo ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Evt1() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Evt1* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      unsigned long long modulo () const { return m_modulo ; }
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor 
      Evt1 () ;                                       // no default constructor 
      // ======================================================================
    private:
      // ======================================================================
      /// the modulo
      unsigned long long m_modulo ; // the modulo
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Evt2
     *  The trivial function with return even number-number (modulo)
     *  @see LHCb::ODIN::eventNumber / modulo  
     *  @see LoKi::Cuts::ODIN_EVT2
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-04-03
     */
    class Evt2: public LoKi::Odin::Evt1 
    {
    public:
      // ======================================================================
      /// constructor from the modulo 
      Evt2 ( const unsigned long long modulo ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Evt2() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Evt2* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// no default constructor 
      Evt2 () ;                                       // no default constructor 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Orbit
     *  The trivial function with return orbit-number 
     *  @see LHCb::ODIN::orbitNumber 
     *  @see LoKi::Cuts::ODIN_ORBIT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class Orbit : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Orbit() { }
      /// MANDATORY: virtual destructor 
      virtual ~Orbit(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Orbit* clone() const { return new Orbit(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> orbitNumber() ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_ORBIT" ; }
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class EventType
     *  The trivial function with return the event type 
     *  @see LHCb::ODIN::eventType
     *  @see LoKi::Cuts::ODIN_EVTTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class EventType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      EventType() { }
      /// MANDATORY: virtual destructor 
      virtual ~EventType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  EventType* clone() const { return new EventType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> eventType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_EVTTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BunchId
     *  The trivial function with return the event type 
     *  @see LHCb::ODIN::bunchID
     *  @see LoKi::Cuts::ODIN_BUNCH
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class BunchId : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      BunchId() { }
      /// MANDATORY: virtual destructor 
      virtual ~BunchId(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BunchId* clone() const { return new BunchId(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> bunchId () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_BUNCH" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ErrorBits
     *  The trivial function with return the error bits 
     *  @see LHCb::ODIN::errorBits
     *  @see LoKi::Cuts::ODIN_ERRBITS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class ErrorBits : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      ErrorBits() { }
      /// MANDATORY: virtual destructor 
      virtual ~ErrorBits(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ErrorBits* clone() const { return new ErrorBits(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> errorBits () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_ERRBITS" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TriggerType
     *  The trivial function with return the trigger type 
     *  @see LHCb::ODIN::triggerType 
     *  @see LoKi::Cuts::ODIN_TRGTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class TriggerType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      TriggerType() { }
      /// MANDATORY: virtual destructor 
      virtual ~TriggerType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TriggerType* clone() const { return new TriggerType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> triggerType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_TRGTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ReadOutType
     *  The trivial function with return the readout type 
     *  @see LHCb::ODIN::readoutType 
     *  @see LoKi::Cuts::ODIN_ROTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class ReadOutType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      ReadOutType() { }
      /// MANDATORY: virtual destructor 
      virtual ~ReadOutType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ReadOutType* clone() const { return new ReadOutType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> readoutType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_ROTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ForceBit 
     *  The trivial predicate to check LHCb::ODIN::forceBit 
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::forceBit 
     *  @see LoKi::Cuts::ODIN_FORCEBIT 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class ForceBit : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate 
    {
    public:
      // ======================================================================
      /// Default Constructor
      ForceBit() { }
      // MANDATORY:  virtual desctrutor
      virtual ~ForceBit () {} ;
      // MANDATORY: clone method ("virtual constructor") 
      virtual ForceBit* clone() const { return new ForceBit ( *this ) ; }
      // MANDATORY: the only one essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> forceBit () ; }
      // OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_FORCEBIT" ; }
      // ======================================================================
    };
    // ========================================================================
    /** @class BXType
     *  The trivial function with return the bunch crossing type 
     *  @see LHCb::ODIN::nunchCrossingType 
     *  @see LoKi::Cuts::ODIN_BXTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class BXType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      BXType() { }
      /// MANDATORY: virtual destructor 
      virtual ~BXType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BXType* clone() const { return new BXType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> bunchCrossingType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_BXTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BunchCurrent
     *  The trivial function with return the bunch current 
     *  @see LHCb::ODIN::bunchCurrent 
     *  @see LoKi::Cuts::ODIN_BXCURRENT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class BunchCurrent : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      BunchCurrent() { }
      /// MANDATORY: virtual destructor 
      virtual ~BunchCurrent(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BunchCurrent* clone() const { return new BunchCurrent(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> bunchCurrent() ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_BXCURRENT" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CalibrationStep
     *  The trivial function with return the calibration step 
     *  @see LHCb::ODIN::calibration step 
     *  @see LoKi::Cuts::ODIN_CALSTEP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class CalibrationStep: public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      CalibrationStep() { }
      /// MANDATORY: virtual destructor 
      virtual ~CalibrationStep(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CalibrationStep* clone() const { return new CalibrationStep(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> calibrationStep () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_CALSTEP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TrgConfKey
     *  The trivial function with return the trigger configuration key 
     *  @see LHCb::ODIN::triggerConfigurationKey 
     *  @see LoKi::Cuts::ODIN_TCK
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class TrgConfKey: public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      TrgConfKey() { }
      /// MANDATORY: virtual destructor 
      virtual ~TrgConfKey(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrgConfKey* clone() const { return new TrgConfKey(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> triggerConfigurationKey () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ODIN_TCK" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /// the derived functions
    // ========================================================================
    /** @class InTime 
     *  The trivial predicate whcih checks that the event time is within 
     *  the certain time window 
     *  @see LoKi::Cuts::ODIN_TIME
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class InTime : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate
    {
    public:
      // ======================================================================
      /// the constructor from the start/stop times
      InTime ( const Gaudi::Time& start , 
               const Gaudi::Time& stop  ) ;
      /// the constructor form the start & span 
      InTime ( const Gaudi::Time&     start , 
               const Gaudi::TimeSpan& span  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~InTime() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual InTime* clone() const { return new InTime(*this) ; }
      /// MANDATORY: The only one essential method:
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const Gaudi::Time& start () const { return m_start ; }
      const Gaudi::Time& stop  () const { return m_stop  ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      InTime();                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the start time 
      Gaudi::Time m_start ; // the start time 
      Gaudi::Time m_stop  ; // the stop time 
      // ======================================================================
    };
    // ========================================================================
    /** @class EvtNumber
     *  Simple predicate to check the event number 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-03-07
     */
    class EvtNumber : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate 
    {
    public:
      // ======================================================================
      /// the actual type for event_type
      typedef LoKi::Numbers::EvtNum             event_type ;
      /// the actual type of event list
      typedef LoKi::Numbers::EvtNumList         event_list ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the run range 
      EvtNumber ( const event_type  begin  , 
                  const event_type  end    ) ;
      /// constructor from event list
      EvtNumber ( const event_list& events ) ;
      /// constructor from the event number 
      EvtNumber ( const event_type  evt    ) ;
      /// MANDATORY: virtual destructor 
      virtual ~EvtNumber() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual EvtNumber* clone() const ;
      /// MANDATORY: The only one essential method:
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      EvtNumber() ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      Flag           m_flag  ;
      /// the range: begin 
      event_type     m_begin ;                            // the range: begin 
      /// the range: end
      event_type     m_end   ;                            // the range: end 
      /// the event list 
      event_list     m_evts  ;                            // the event list 
      // ======================================================================
    };
    // ========================================================================
    /** @class RunNumber 
     *  The trivial predicate which checks the run number, 
     *  or range of run numbers or list of run numbers 
     *  @see LoKi::Cuts::ODIN_RUNNUMBER
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class RunNumber : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate
    {
    public:
      // ======================================================================
      /// the actual type of the run 
      typedef LoKi::Numbers::RunEvt::run_type     run_type ;
      typedef std::vector<run_type>               run_list ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the run number 
      RunNumber ( const run_type  run   ) ;
      /// constructor from the run range 
      RunNumber ( const run_type  begin , 
                  const run_type  end   ) ;
      /// constructor from the run list
      RunNumber ( const run_list& runs  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RunNumber() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual RunNumber* clone() const ;
      /// MANDATORY: The only one essential method:
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      RunNumber() ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      Flag                      m_flag  ;
      run_type                  m_begin ;
      run_type                  m_end   ;
      std::vector<unsigned int> m_runs  ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class RunEvtNumber 
     *  The trivial predicate which checks (Run,Event)-identifiers
     *  @see LoKi::Cuts::ODIN_RUNEVT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class RunEvtNumber : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate
    {
    public:
      // ======================================================================
      /// the actual type of run-number 
      typedef LoKi::Numbers::RunEvt::run_type       run_type        ;
      /// the actual type of event-number  
      typedef LoKi::Numbers::RunEvt::evt_type       evt_type        ;
      /// the actual type of run-event pair 
      typedef LoKi::Numbers::RunEvt                 runevt_type     ;
      // the actual type of run-event list 
      typedef LoKi::Numbers::RunEvtList             runevt_list     ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the run/event number 
      RunEvtNumber ( const run_type      run     , 
                     const evt_type      evt     ) ;
      /// constructor from the run/event number 
      RunEvtNumber ( const runevt_type&  runevt  ) ;
      /// constructor from the run/event range 
      RunEvtNumber ( const runevt_type&  begin   , 
                     const runevt_type&  end     ) ;
      /// constructor from the run-event  list
      RunEvtNumber ( const runevt_list& runevts ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RunEvtNumber() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual RunEvtNumber* clone() const ;
      /// MANDATORY: The only one essential method:
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      RunEvtNumber() ;                   // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      Flag                      m_flag      ;
      /// the range: begin 
      runevt_type               m_begin     ;           //     the range: begin 
      /// the range: end 
      runevt_type               m_end       ;           //       the range: end 
      /// the run-event list 
      runevt_list               m_runevts   ;           //   the run-event list 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BXId
     *  The trivial predicate to deal with bunch-crossing IDs  
     *  @see LHCb::ODIN::bunchID
     *  @see LoKi::Cuts::ODIN_BXID
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class BXId : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from the BX-Id
      BXId ( const unsigned int bx   ) ;
      /// constructor from the run range 
      BXId ( const unsigned int begin , 
             const unsigned int end   ) ;
      /// constructor from the run list
      BXId ( const std::vector<unsigned int>& bxs ) ;
      /// MANDATORY: virtual destructor 
      virtual ~BXId() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BXId* clone() const { return new BXId(*this) ; }
      /// MANDATORY: The only one essential method:
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      BXId()      ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      Flag                      m_flag  ;
      unsigned int              m_bx    ;
      unsigned int              m_begin ;
      unsigned int              m_end   ;
      std::vector<unsigned int> m_bxs   ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Tck
     *  The trivial predicate to deal with bunch-crossing IDs  
     *  @see LHCb::ODIN::triggerConfigurationKey 
     *  @see LoKi::Cuts::ODIN_TCKEYS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class Tck : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from the BX-Id
      Tck ( const unsigned int bx    ) ;
      /// constructor from the tck-list
      Tck ( const std::vector<unsigned int>& bxs ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Tck() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Tck* clone() const { return new Tck(*this) ; }
      /// MANDATORY: The only one essential method:
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Tck()      ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      Flag                      m_flag  ;
      unsigned int              m_bx    ;
      std::vector<unsigned int> m_bxs   ;
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class HltRoutingBits 
     *  Simple predicate to check the hlt routing bits 
     *  @see LoKi::Cuts::ODIN_ROUTINGBITS
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-05-17
     */
    class RoutingBits 
      : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from routing bits 
      RoutingBits ( const LoKi::HLT::RoutingBits& bits ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RoutingBits () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RoutingBits* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument /* a */ ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    public:
      // ======================================================================
      operator const LoKi::HLT::RoutingBits&() const { return m_bits ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      RoutingBits () ;                   // the default constructor is disabled 
      // ====================================================================== 
    private:
      // ====================================================================== 
      /// the bits 
      LoKi::HLT::RoutingBits m_bits ;                               // the bits 
      // ====================================================================== 
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Odin
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ODIN_H
// ============================================================================
