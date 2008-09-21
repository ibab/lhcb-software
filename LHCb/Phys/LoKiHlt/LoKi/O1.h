// $Id: O1.h,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_O1_H 
#define LOKI_O1_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Time.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
// DAQEvent
// ============================================================================
#include "Event/ODIN.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::O1
   *  The namespace to keep all ODIN-related LoKi functors 
   *  @see LHCb::ODIN
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-09-16
   */
  namespace O1 
  {
    // ======================================================================
    /// the private state 
    enum Flag { One , Range , List } ;                   // the private state 
    // ========================================================================
    /** @class Run
     *  The trivial function with returnn run-number
     *  @see LHCb::ODIN::runNumber 
     *  @see LoKi::Cuts::O1_RUN
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class Run : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~Run(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Run* clone() const { return new Run(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> runNumber() ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_RUN" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Orbit
     *  The trivial function with return orbit-number 
     *  @see LHCb::ODIN::orbitNumber 
     *  @see LoKi::Cuts::O1_ORBIT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class Orbit : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~Orbit(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Orbit* clone() const { return new Orbit(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> orbitNumber() ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_ORBIT" ; }
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class EventType
     *  The trivial function with return the event type 
     *  @see LHCb::ODIN::eventType
     *  @see LoKi::Cuts::O1_EVTTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class EventType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~EventType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  EventType* clone() const { return new EventType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> eventType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_EVTTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BunchId
     *  The trivial function with return the event type 
     *  @see LHCb::ODIN::bunchID
     *  @see LoKi::Cuts::O1_BUNCH
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class BunchId : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~BunchId(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BunchId* clone() const { return new BunchId(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> bunchId () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_BUNCH" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ErrorBits
     *  The trivial function with return the error bits 
     *  @see LHCb::ODIN::errorBits
     *  @see LoKi::Cuts::O1_ERRBITS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class ErrorBits : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~ErrorBits(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ErrorBits* clone() const { return new ErrorBits(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> errorBits () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_ERRBITS" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TriggerType
     *  The trivial function with return the trigger type 
     *  @see LHCb::ODIN::triggerType 
     *  @see LoKi::Cuts::O1_TRGTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class TriggerType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TriggerType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TriggerType* clone() const { return new TriggerType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> triggerType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_TRGTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ReadOutType
     *  The trivial function with return the readout type 
     *  @see LHCb::ODIN::readoutType 
     *  @see LoKi::Cuts::O1_ROTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class ReadOutType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~ReadOutType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ReadOutType* clone() const { return new ReadOutType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> readoutType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_ROTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ForceBit 
     *  The trivial predicate to check LHCb::ODIN::forceBit 
     *  @see LHCb::ODIN
     *  @see LHCb::ODIN::forceBit 
     *  @see LoKi::Cuts::O1_FORCEBIT 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class ForceBit : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate 
    {
    public:
      // ======================================================================
      // MANDATORY:  virtual desctrutor
      virtual ~ForceBit () {} ;
      // MANDATORY: clone method ("virtual constructor") 
      virtual ForceBit* clone() const { return new ForceBit ( *this ) ; }
      // MANDATORY: the only one essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> forceBit () ; }
      // OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_FORCEBIT" ; }
      // ======================================================================
    };
    // ========================================================================
    /** @class BXType
     *  The trivial function with return the bunch crossing type 
     *  @see LHCb::ODIN::nunchCrossingType 
     *  @see LoKi::Cuts::O1_BXTYP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class BXType : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~BXType(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BXType* clone() const { return new BXType(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> bunchCrossingType () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_BXTYP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BunchCurrent
     *  The trivial function with return the bunch current 
     *  @see LHCb::ODIN::bunchCurrent 
     *  @see LoKi::Cuts::O1_BXCURRENT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class BunchCurrent : public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~BunchCurrent(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  BunchCurrent* clone() const { return new BunchCurrent(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> bunchCurrent() ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_BXCURRENT" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CalibrationStep
     *  The trivial function with return the calibration step 
     *  @see LHCb::ODIN::calibration step 
     *  @see LoKi::Cuts::O1_CALSTEP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class CalibrationStep: public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~CalibrationStep(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CalibrationStep* clone() const { return new CalibrationStep(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> calibrationStep () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_CALSTEP" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TrgConfKey
     *  The trivial function with return the trigger configuration key 
     *  @see LHCb::ODIN::triggerConfigurationKey 
     *  @see LoKi::Cuts::O1_TCK
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class TrgConfKey: public LoKi::BasicFunctors<const LHCb::ODIN*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TrgConfKey(){}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrgConfKey* clone() const { return new TrgConfKey(*this); }
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const 
      { return o -> triggerConfigurationKey () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "O1_TCK" ; }
      // ======================================================================
    } ;

    /// the derived functions 

    // ========================================================================
    /** @class InTime 
     *  The trivial predicate whcih checks that the event time is within 
     *  the certain time window 
     *  @see LoKi::Cuts::O1_TIME
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
    /** @class RunNumber 
     *  The trivial predicate which checks the run number, 
     *  or range of run numbers or list of run numbers 
     *  @see LoKi::Cuts::O1_RUNNUMBER
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class RunNumber : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the run number 
      RunNumber ( const unsigned int run   ) ;
      /// constructor from the run range 
      RunNumber ( const unsigned int begin , 
                  const unsigned int end   ) ;
      /// constructor from the run list
      RunNumber ( const std::vector<unsigned int>& runs ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RunNumber() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual RunNumber* clone() const { return new RunNumber(*this) ; }
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
      unsigned int              m_run   ;
      unsigned int              m_begin ;
      unsigned int              m_end   ;
      std::vector<unsigned int> m_runs  ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BXId
     *  The trivial predicate to deal with bunch-crossing IDs  
     *  @see LHCb::ODIN::bunchID
     *  @see LoKi::Cuts::O1_BXID
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
     *  @see LoKi::Cuts::O1_TCKEYS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-16
     */
    class Tck : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from the BX-Id
      Tck ( const unsigned int bx    ) ;
      /// constructor from the run range 
      Tck ( const unsigned int begin , 
            const unsigned int end   ) ;
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
      unsigned int              m_begin ;
      unsigned int              m_end   ;
      std::vector<unsigned int> m_bxs   ;
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::O1 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_O1_H
// ============================================================================
