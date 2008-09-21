// $Id: L0.h,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_L0_H 
#define LOKI_L0_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
// L0Event
// ============================================================================
#include "Event/L0DUReport.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::L0 
   *  The namespace to keep all L0-related LoKi functors 
   *  @see LHCb::L0DUReport 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-09-19
   */      
  namespace L0 
  {
    // ========================================================================
    enum Flag { 
      /// get channel by name 
      ByName ,                                           // get channel by name 
      /// get channel by number 
      ByNumber                                         // get channel by number 
    } ;
    // ========================================================================
    /** @class SumEt 
     *  @see LHCb::L0DUReport 
     *  @see LHCb::L0DUReport::sumEt 
     *  @see LoKi::Cuts::L0_SUMET 
     *  @see LoKi::Cuts::L0_SUMET0 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */      
    class SumEt 
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Function
    {
    public:
      // ======================================================================
      /// constructor from bx-id 
      SumEt ( const int bx = 0  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SumEt () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SumEt* clone () const { return new SumEt ( *this ) ; }
      // ======================================================================
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a->sumEt ( m_bx ) ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// BX-id 
      int m_bx ;                                                       // BX-id 
      // ======================================================================
    };
    // ========================================================================
    /** @class DataValue 
     *  Simple accessor to the data value 
     *  @see LHCb::L0DUReport 
     *  @see LHCb::L0DUReport::dataValue
     *  @see LoKi::Cuts::L0_DATA
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */      
    class DataValue 
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Function
    {
    public:
      // ======================================================================
      /// constructor form the data name 
      DataValue ( const std::string& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DataValue() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DataValue* clone() const { return new DataValue ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a -> dataValue ( name () ) ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const std::string& name() const { return m_name ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DataValue() ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// data name 
      std::string m_name ;                                         // data name 
      // ======================================================================      
    };
    // ========================================================================
    /** @class DataDigit
     *  Simple accessor to the data digit 
     *  @see LHCb::L0DUReport 
     *  @see LHCb::L0DUReport::dataDigit
     *  @see LoKi::Cuts::L0_DIGIT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */      
    class DataDigit : public DataValue 
    {
    public:
      // ======================================================================
      /// constructor form the data name 
      DataDigit ( const std::string& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DataDigit () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DataDigit* clone() const { return new DataDigit ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a -> dataDigit ( name () ) ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DataDigit () ;                     // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class ChannelDecision 
     *  Accessor to the channel decision
     *  @see LHCb::L0DUReport::channelDecision 
     *  @see LHCb::L0DUReport::channelDecisionNyName  
     *  @see LoKi::Cuts::L0_CHDECISION 
     *  @see LoKi::Cuts::L0_CHANNEL
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class ChannelDecision 
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate 
    {
    protected:
      // ======================================================================
      typedef std::vector<std::string>                                  Names ;
      // ======================================================================
    public:
      // ======================================================================
      /// channel decision by channel number 
      ChannelDecision ( const unsigned int channel , const int bx = 0 ) ;
      /// channel decision by channel name 
      ChannelDecision ( const std::string& channel , const int bx = 0 ) ;
      /// channel decision by channel names  ("OR")
      ChannelDecision ( const std::vector<std::string>& channels , 
                        const int bx = 0 ) ;
      /// virtual destructor 
      virtual ~ChannelDecision () {}
      /// MANDATORY: clone method ('virtual constructor')
      virtual  ChannelDecision* clone() const 
      { return new ChannelDecision ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const unsigned int index () const { return m_channel ; }
      const Names&       names () const { return m_names   ; }      
      // ======================================================================
      const Flag mode () const { return m_mode ; }
      const int  bx   () const { return m_bx   ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ChannelDecision() ;                // the default constructor is disabled 
      // ====================================================================== 
    private:
      // ====================================================================== 
      Flag         m_mode    ;
      /// the channel number 
      unsigned int m_channel ;                            // the channel number 
      /// the channel names
      Names        m_names   ;                             // the channel names 
      /// the bx-id 
      int          m_bx      ;                                     // the bx-id 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ChannelPreDecision 
     *  Accessor to the channel predecision
     *  @see LHCb::L0DUReport::channelPreDecision 
     *  @see LHCb::L0DUReport::channelPreDecisionNyName  
     *  @see LoKi::Cuts::L0_CHPREDECISION 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class ChannelPreDecision : public ChannelDecision
    {
    public:
      // ======================================================================
      /// channel decision by channel number 
      ChannelPreDecision ( const unsigned int channel , const int bx = 0 ) ;
      /// channel decision by channel name 
      ChannelPreDecision ( const std::string& channel , const int bx = 0 ) ;
      /// channel decision by channel names  ("OR")
      ChannelPreDecision ( const std::vector<std::string>& channels , 
                           const int bx = 0 ) ;
      /// virtual destructor 
      virtual ~ChannelPreDecision () {}
      /// MANDATORY: clone method ('virtual constructor')
      virtual  ChannelPreDecision* clone() const 
      { return new ChannelPreDecision ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ChannelPreDecision() ;             // the default constructor is disabled 
      // ====================================================================== 
    } ;
    // ========================================================================
    /** @class ConditionValue 
     *  Accessor to the condition value 
     *  @see LHCb::L0DUReport::conditionValue
     *  @see LHCb::L0DUReport::conditionValueByName
     *  @see LoKi::Cuts::L0_CONDITION
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class ConditionValue : public ChannelDecision
    {
    public:
      // ======================================================================
      /// channel decision by channel number 
      ConditionValue ( const unsigned int channel , const int bx = 0 ) ;
      /// channel decision by channel name 
      ConditionValue ( const std::string& channel , const int bx = 0 ) ;
      /// channel decision by channel names  ("OR")
      ConditionValue ( const std::vector<std::string>& channels , 
                       const int bx = 0 ) ;
      /// virtual destructor 
      virtual ~ConditionValue () {}
      /// MANDATORY: clone method ('virtual constructor')
      virtual  ConditionValue* clone() const 
      { return new ConditionValue ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ConditionValue () ;               // the default constructor is disabled 
      // ====================================================================== 
    } ;
    // ========================================================================
    /** @class TriggerDecision
     *  Accessor to the trigger decision 
     *  @see LHCb::L0DUReport::triggerDecision
     *  @see LHCb::L0DUReport::triggerDecisionByName
     *  @see LoKi::Cuts::L0_TRIGGER
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class TriggerDecision : public ChannelDecision
    {
    public:
      // ======================================================================
      /// channel decision by channel number 
      TriggerDecision ( const unsigned int channel , const int bx = 0 ) ;
      /// channel decision by channel name 
      TriggerDecision ( const std::string& channel , const int bx = 0 ) ;
      /// channel decision by channel names  ("OR")
      TriggerDecision ( const std::vector<std::string>& channels , 
                       const int bx = 0 ) ;
      /// virtual destructor 
      virtual ~TriggerDecision () {}
      /// MANDATORY: clone method ('virtual constructor')
      virtual  TriggerDecision* clone() const 
      { return new TriggerDecision ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TriggerDecision () ;               // the default constructor is disabled 
      // ====================================================================== 
    } ;
    // ========================================================================
    /** @class SumDecision
     *  check the decision form the summary
     *  @see LHCb::L0DUReport::decisionFromSummary
     *  @see LoKi::Cuts::L0_SUMDECISION 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class SumDecision
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate 
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~SumDecision () {}
      /// MANDATORY: clone method ('virtual constructor') 
      virtual  SumDecision* clone () const 
      { return new SumDecision ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a -> decisionFromSummary () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    };
    // ========================================================================
    /** @class Decision
     *  check the L0-decision
     *  @see LHCb::L0DUReport::decision
     *  @see LoKi::Cuts::L0_DECISION 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class Decision
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate 
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Decision () {}
      /// MANDATORY: clone method ('virtual constructor') 
      virtual  Decision* clone () const 
      { return new Decision ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a -> decision () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    };
    // ========================================================================
    /** @class ForceBit
     *  check the force-bit
     *  @see LHCb::L0DUReport::forceBit
     *  @see LoKi::Cuts::L0_FORCEBIT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class ForceBit
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate 
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~ForceBit () {}
      /// MANDATORY: clone method ('virtual constructor') 
      virtual  ForceBit* clone () const 
      { return new ForceBit ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a -> forceBit () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    };
    // ========================================================================
    /** @class Tck
     *  get the trigger configuration key 
     *  @see LHCb::L0DUReport::tck
     *  @see LoKi::Cuts::L0_TCK
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class Tck
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Tck () {}
      /// MANDATORY: clone method ('virtual constructor') 
      virtual  Tck* clone () const { return new Tck ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a -> tck () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    };
    // ========================================================================    
    /** @class TimingBit
     *  check the timing-bit
     *  @see LHCb::L0DUReport::timingTriggerBit
     *  @see LoKi::Cuts::L0_TIMINGBIT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class TimingBit
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate 
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~TimingBit () {}
      /// MANDATORY: clone method ('virtual constructor') 
      virtual  TimingBit* clone () const 
      { return new TimingBit ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const 
      { return a -> timingTriggerBit () ; }
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    };
    // ========================================================================
  } // end of namespace LoKi::L0 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_L0_H
// ============================================================================
