// $Id: L0.h,v 1.6 2010-03-12 12:23:55 ibelyaev Exp $
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
// Boost
// ============================================================================
#include "boost/regex.hpp"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::L0 
   *  The namespace to keep all L0-related LoKi functors 
   *
   *  @attention All functors, but L0_VALID, 
   *    throws the exception for zero pointer to L0DUReport Objects 
   *
   *  @see LHCb::L0DUReport 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-09-19
   */      
  namespace L0 
  {
    // ========================================================================
    /** @class Valid 
     *  Simple functor to check the validity of L0DUReport object.
     *  Unlike other functors is also accept the zero pointer.
     *  
     *  @see LHCb::L0DUReport 
     *  @see LHCb::L0DUReport::valid 
     *  @see LoKi::Cuts::L0_VALID 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-01-08
     */
    class Valid
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Valid() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Valid* clone () const ;
      // ======================================================================
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    };
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
      virtual result_type operator() ( argument a ) const ;
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
      virtual result_type operator() ( argument a ) const  ;
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
      virtual result_type operator() ( argument a ) const ;
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
    /** @class SumDecision
     *  check the decision from the summary
     *  @see LHCb::L0DUReport::decisionFromSummary
     *  @see LoKi::Cuts::L0_SUMDECISION 
     *  @see LoKi::Cuts::L0_SUMDECISION_PHYSICS  
     *  @see LoKi::Cuts::L0_SUMDECISION_BEAM1  
     *  @see LoKi::Cuts::L0_SUMDECISION_BEAM2
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class SumDecision
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from decision mask 
      SumDecision ( const int mask , const int bx = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SumDecision () {}
      /// MANDATORY: clone method ('virtual constructor') 
      virtual  SumDecision* clone () const 
      { return new SumDecision ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disabled 
      SumDecision () ;                  // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      /// the decision type 
      int m_mask ; //  the decision type 
      /// ? 
      int m_bx   ;  // ? 
      // ======================================================================      
    };
    // ========================================================================
    /** @class Decision
     *  check the L0-decision
     *  @see LHCb::L0DUReport::decision
     *  @see LoKi::Cuts::L0_DECISION 
     *  @see LoKi::Cuts::L0_DECISION_PHYSICS  
     *  @see LoKi::Cuts::L0_DECISION_BEAM1  
     *  @see LoKi::Cuts::L0_DECISION_BEAM2
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class Decision
      : public LoKi::BasicFunctors<const LHCb::L0DUReport*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from decision mask 
      Decision ( const int mask ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Decision () ;
      /// MANDATORY: clone method ('virtual constructor') 
      virtual  Decision* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disbaled 
      Decision() ;                      // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================
      /// the decision mask 
      int m_mask ;                                         // the decision mask 
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
      virtual result_type operator() ( argument a ) const ;
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
      virtual result_type operator() ( argument a ) const ;
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
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
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
      typedef std::vector<unsigned int>                              Channels ;
      // ======================================================================
    public:
      // ======================================================================
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
      const Channels&    channels () const { return m_channels ; }
      const Names&       names    () const { return m_names    ; }      
      // ======================================================================
      int  bx   () const { return m_bx   ; }
      // ======================================================================
    protected:
      // ======================================================================
      unsigned int tckPrev   ()                 const { return m_tckPrev   ; }
      void     setTckPrev    ( const int tck  ) const { m_tckPrev = tck    ; }
      void     clearChannels ()                 const { m_channels.clear() ; }
      void     addChannel    ( unsigned int c ) const 
      { m_channels.push_back ( c ) ; }
      void     clearNames    ()                 const { m_names.clear() ; }
      void     addName       ( const std::string& c ) const 
      { m_names.push_back ( c ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ChannelDecision() ;                // the default constructor is disabled 
      // ====================================================================== 
    private:
      // ====================================================================== 
      /// the channel names
      mutable Names m_names  ;                             // the channel names
      /// channels 
      mutable Channels m_channels ; // the channels  
      /// the bx-id 
      int          m_bx      ;                                     // the bx-id 
      // cached TCK 
      mutable unsigned int m_tckPrev ;                           // cached  TCK 
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
    class ChannelDecisionSubString : public ChannelDecision 
    {
    public:
      // ======================================================================
      /// constructor from substring
      ChannelDecisionSubString 
      ( const std::string& substr     , 
        const int          bx     = 0 ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChannelDecisionSubString() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChannelDecisionSubString* clone() const 
      { return new ChannelDecisionSubString ( *this ) ; }
      /// MANDATORY: the only one essential methos 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================      
    public:
      // ======================================================================      
      const std::string& substr() const { return m_substr ; }
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disabled 
      ChannelDecisionSubString () ;      // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      /// the substring 
      std::string m_substr ;                                   // the substring 
      // ======================================================================      
    } ;
    // ========================================================================
    class ChannelDecisionRegex : public ChannelDecisionSubString
    {
    public:
      // ======================================================================
      /// constructor from substring
      ChannelDecisionRegex 
      ( const std::string& substr     , 
        const int          bx     = 0 ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChannelDecisionRegex() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChannelDecisionRegex* clone() const 
      { return new ChannelDecisionRegex ( *this ) ; }
      /// MANDATORY: the only one essential methos 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================      
    public:
      // ======================================================================      
      const boost::regex& expression() const { return m_expression ; }
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disabled 
      ChannelDecisionRegex () ;      // the default constructor is disabled 
      // ======================================================================      
    private:
      // ====================================================================== 
      /// the regular expression 
      boost::regex m_expression ;                     // the regular expression 
      // ======================================================================      
    } ;
    // ========================================================================
    class ChannelPreDecisionSubString : public ChannelDecisionSubString 
    {
    public:
      // ======================================================================
      /// constructor from substring
      ChannelPreDecisionSubString 
      ( const std::string& substr     , 
        const int          bx     = 0 ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChannelPreDecisionSubString() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChannelPreDecisionSubString* clone() const 
      { return new ChannelPreDecisionSubString ( *this ) ; }
      /// MANDATORY: the only one essential methos 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disabled 
      ChannelPreDecisionSubString () ;   // the default constructor is disabled 
      // ======================================================================      
    } ;
    // ========================================================================
    class ChannelPreDecisionRegex : public ChannelDecisionRegex
    {
    public:
      // ======================================================================
      /// constructor from substring
      ChannelPreDecisionRegex 
      ( const std::string& substr     , 
        const int          bx     = 0 ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChannelPreDecisionRegex() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChannelPreDecisionRegex* clone() const 
      { return new ChannelPreDecisionRegex ( *this ) ; }
      /// MANDATORY: the only one essential methos 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================      
    private:
      // ======================================================================      
      /// the default constructor is disabled 
      ChannelPreDecisionRegex () ;       // the default constructor is disabled 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class TriggerDecisionSubString
     *  Accessor to the trigger decision 
     *  @see LHCb::L0DUReport::triggerDecision
     *  @see LHCb::L0DUReport::triggerDecisionByName
     *  @see LoKi::Cuts::L0_TRIGGER_SUBSTR
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class TriggerDecisionSubString : public ChannelDecisionSubString
    {
    public:
      // ======================================================================
      /// trigger decision by name 
      TriggerDecisionSubString ( const std::string& channel , const int bx = 0 ) ;
      /// virtual destructor 
      virtual ~TriggerDecisionSubString () {}
      /// MANDATORY: clone method ('virtual constructor')
      virtual  TriggerDecisionSubString* clone() const 
      { return new TriggerDecisionSubString ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TriggerDecisionSubString () ;      // the default constructor is disabled 
      // ====================================================================== 
    } ;
    // ========================================================================
    /** @class TriggerDecisionRegex
     *  Accessor to the trigger decision 
     *  @see LHCb::L0DUReport::triggerDecision
     *  @see LHCb::L0DUReport::triggerDecisionByName
     *  @see LoKi::Cuts::L0_TRIGGER_SUBSTR
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */     
    class TriggerDecisionRegex : public ChannelDecisionRegex
    {
    public:
      // ======================================================================
      /// trigger decision by name 
      TriggerDecisionRegex ( const std::string& channel , const int bx = 0 ) ;
      /// virtual destructor 
      virtual ~TriggerDecisionRegex () {}
      /// MANDATORY: clone method ('virtual constructor')
      virtual  TriggerDecisionRegex* clone() const 
      { return new TriggerDecisionRegex ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TriggerDecisionRegex () ;          // the default constructor is disabled 
      // ====================================================================== 
    } ;
    // ========================================================================
  } // end of namespace LoKi::L0 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_L0_H
// ============================================================================
