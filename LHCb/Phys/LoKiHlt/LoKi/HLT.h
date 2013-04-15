// $Id: HLT.h,v 1.6 2010-05-17 16:01:38 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLT_H
#define LOKI_HLT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StringKey.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/RoutingBits.h"
// ============================================================================
// L0Event
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/regex.hpp"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::HLT
   *  The namespace to keep all HltDecReport-related LoKi functors
   *  @see LHCb::HltDecisionReports
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-09-19
   */
  // ==========================================================================
  namespace HLT
  {
    // ========================================================================
    /** @class HasDecision
     *  simple function which allows to check the existence
     *  of the decision with the given name
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_HAS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class GAUDI_API HasDecision
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    protected:
      // ======================================================================
      typedef std::vector<Gaudi::StringKey>                             Names ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the decision name
      HasDecision ( const std::string& name  ) ;
      /// constructor from the decision names ("OR")
      HasDecision ( const std::string& name1 ,
                    const std::string& name2 ) ;
      /// constructor from the decision names ("OR")
      HasDecision ( const std::string& name1 ,
                    const std::string& name2 ,
                    const std::string& name3 ) ;
      /// constructor from the decision names ("OR")
      HasDecision ( const std::string& name1 ,
                    const std::string& name2 ,
                    const std::string& name3 ,
                    const std::string& name4 ) ;
      /// constructor form the decision names ("OR")
      HasDecision ( const Names&       names ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasDecision () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasDecision* clone() const
      { return new HasDecision ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return print ( s , "HLT_HAS" ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      // get the decision names
      const Names& names() const { return m_names ; }
      // ======================================================================
    protected:
      // ======================================================================
      Names::const_iterator begin () const { return m_names.begin () ; }
      Names::const_iterator end   () const { return m_names.end   () ; }
      // ======================================================================
      std::ostream& print ( std::ostream& s , const std::string& name ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      HasDecision () ;                  // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the list of decision names
      Names m_names ; // the list of decision names
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PassDecision
     *  simple function which allows to check if the decision
     *  has been passed
     *  of the decision with the given name
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_PASS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class GAUDI_API PassDecision : public HasDecision
    {
    public:
      // ======================================================================
      /// constructor from the decision name
      PassDecision ( const std::string& name  )
        : HasDecision ( name )
      {}
      /// constructor from the decision names ("OR")
      PassDecision ( const std::string& name1 ,
                     const std::string& name2 )
        : HasDecision ( name1 , name2 )
      {}
      /// constructor from the decision names ("OR")
      PassDecision ( const std::string& name1 ,
                     const std::string& name2 ,
                     const std::string& name3 )
        : HasDecision ( name1 , name2 , name3 )
      {}
      /// constructor from the decision names ("OR")
      PassDecision ( const std::string& name1 ,
                     const std::string& name2 ,
                     const std::string& name3 ,
                     const std::string& name4 )
        : HasDecision ( name1 , name2 , name3 , name4 )
      {}
      /// constructor form the decision names ("OR")
      PassDecision ( const Names& names ) : HasDecision ( names ) {}
      /// MANDATORY: virtual destructor
      virtual ~PassDecision () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PassDecision* clone() const
      { return new PassDecision ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return print ( s , "HLT_PASS" ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PassDecision () ;                 // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Size
     *  simple function which returns the number of known decision
     *  @see LHCb::HltDecReports
     *  @see LHCb::HltDecReports::size
     *  @see LoKi::Cuts::HLT_SIZE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class GAUDI_API Size
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      Size() {}
      /// MANDATORY: virtual destructor
      virtual ~Size () {}
      /// MANDATORY: clone method ("virtual consructor")
      virtual  Size* clone() const { return new Size ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator () ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << "HLT_SIZE" ; }
      // ======================================================================
    };
    // ========================================================================
    /** @class NPass
     *  simple function which returnn the number of 'passes' decision
     *  @see LHCb::HltDecReports
     *  @see LHCb::HltDecReports::decReports
     *  @see LHCb::HltDecReport
     *  @see LHCb::HltDecReport::decision
     *  @see LoKi::Cuts::HLT_SIZE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class GAUDI_API NPass
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      NPass() {}
      /// MANDATORY: virtual destructor
      virtual ~NPass () {}
      /// MANDATORY: clone method ("virtual consructor")
      virtual  NPass* clone() const { return new NPass ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator () ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << "HLT_NPASS" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Decision
     *  simple function which checks the presence of at least one positive
     *  decision
     *  @see LHCb::HltDecReports
     *  @see LHCb::HltDecReports::decReports
     *  @see LHCb::HltDecReport
     *  @see LHCb::HltDecReport::decision
     *  @see LoKi::Cuts::HLT_DECISION
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class GAUDI_API Decision
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      Decision() {}
      /// MANDATORY: virtual destructor
      virtual ~Decision () {}
      /// MANDATORY: clone method ("virtual consructor")
      virtual  Decision* clone() const { return new Decision ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator () ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const
      { return s << "HLT_DECISION" ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DecisionBut
     *  Simple predicate which checks the positive decisison, ignoring
     *  "the special" cases
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_PASSIGNORING
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-03
     */
    class GAUDI_API DecisionBut
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    protected:
      // ======================================================================
      typedef std::vector<Gaudi::StringKey>                             Names ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from one "special" decicion
      DecisionBut ( const std::string& name ) ;
      /// constructor from two "special" decicions
      DecisionBut ( const std::string& name1 ,
                    const std::string& name2 ) ;
      /// constructor from three "special" decicions
      DecisionBut ( const std::string& name1 ,
                    const std::string& name2 ,
                    const std::string& name3 ) ;
      /// constructor from four "special" decicions
      DecisionBut ( const std::string& name1 ,
                    const std::string& name2 ,
                    const std::string& name3 ,
                    const std::string& name4 ) ;
      /// constructor from vector of "special" decicions
      DecisionBut ( const Names&       names ) ;
      /// constructor from vector of "special" decicions
      DecisionBut ( const std::vector<std::string>& names ) ;
      /// MANDATORY: virtual destructor
      virtual ~DecisionBut() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DecisionBut* clone () const { return new DecisionBut ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator () ( argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual vector of "special" decision
      Names m_special ; // the actual vector of "special" decision
      // ======================================================================
    };
    // ========================================================================
    /** @class HasDecisionSubString
     *  simple function which allows to check the existence
     *  of the decision with the given name
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_HAS_SUBSTR
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class GAUDI_API HasDecisionSubString
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from substring
      HasDecisionSubString ( const std::string& substr ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasDecisionSubString () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasDecisionSubString* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// access to the substring
      const std::string& substr() const { return m_substr ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      HasDecisionSubString () ;          // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the substring to be looked
      std::string m_substr ;                      // the substring to be looked
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PassDecisionSubString
     *  Simple class to check the presence of at least one passes selection
     *  which contains the substring
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_PASS_SUBSTR
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-06-16
     */
    class GAUDI_API PassDecisionSubString : public HasDecisionSubString
    {
    public:
      // ======================================================================
      /// constructor from substring
      PassDecisionSubString ( const std::string& substr ) ;
      /// MANDATORY: virtual destructor
      virtual ~PassDecisionSubString () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual PassDecisionSubString* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PassDecisionSubString () ;          // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class DecisionButSubString
     *  Simple predicate which checks the positive decisison, ignoring
     *  "the special" cases
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_PASSIGNORING_SUBSTR
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-06-16
     */
    class GAUDI_API DecisionButSubString : public PassDecisionSubString
    {
    public:
      // ======================================================================
      /// constructor from substring
      DecisionButSubString ( const std::string& substr ) ;
      /// MANDATORY: virtual destructor
      virtual ~DecisionButSubString () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual DecisionButSubString* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      DecisionButSubString () ;          // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class HasDecisionRegex
     *  Simple class to check the presence of at least one passes selection
     *  which matches the regular expression
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_HAS_RE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-06-16
     */
    class GAUDI_API HasDecisionRegex : public HasDecisionSubString
    {
    public:
      // ======================================================================
      /// constructor from substring
      HasDecisionRegex ( const std::string& substr ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasDecisionRegex () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasDecisionRegex* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      HasDecisionRegex () ;              // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// regurn the regular expression
      const boost::regex& expression () const { return m_expression ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the regular expression
      boost::regex  m_expression ;                    // the regular expression
      // ======================================================================
    };
    // ========================================================================
    /** @class PAssDecisionRegex
     *  Simple class to check the presence of at least one passes selection
     *  which matches the regular expression
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_HAS_RE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-06-16
     */
    class GAUDI_API PassDecisionRegex : public HasDecisionRegex
    {
    public:
      // ======================================================================
      /// constructor from substring
      PassDecisionRegex ( const std::string& substr ) ;
      /// MANDATORY: virtual destructor
      virtual ~PassDecisionRegex () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PassDecisionRegex* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      PassDecisionRegex () ;             // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class DecisionButRegex
     *  Simple predicate which checks the positive decisison, ignoring
     *  "the special" cases
     *  @see LHCb::HltDecReports
     *  @see LoKi::Cuts::HLT_PASSIGNORING_RE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-06-16
     */
    class GAUDI_API DecisionButRegex : public PassDecisionRegex
    {
    public:
      // ======================================================================
      /// constructor from substring
      DecisionButRegex ( const std::string& substr ) ;
      /// MANDATORY: virtual destructor
      virtual ~DecisionButRegex () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual DecisionButRegex* clone () const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      DecisionButRegex () ;          // the default constructor is disabled
      // ======================================================================
    };
    // ========================================================================
    /** @class ErrorBits
     *  Simple functor to extract the error bits from the the given channel
     *  @see LoKi::Cuts::HLT_ERRORBITS
     *  @see LHCb::HltDecReport::errorBits
     *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
     *  @date 2010-05-17
     */
    class GAUDI_API ErrorBits
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function
    {
    public:
      // ======================================================================
      /// constructor from the channel name
      ErrorBits ( const std::string&      name ) ;
      /// constructor from the channel name
      ErrorBits ( const Gaudi::StringKey& name ) ;
      /// MANDATORY: virtual destructor
      virtual ~ErrorBits() ;
      /// MANDATORY: clone method ( "virtual constructor")
      virtual  ErrorBits* clone () const ;
      /// MANDATORY: the only one essential methor
      result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      ErrorBits () ;                     // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      const Gaudi::StringKey& channel( ) const { return m_key ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the channel key
      Gaudi::StringKey m_key ; // the channle key
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ExecutionStage
     *  Simple functor to extract the execution stage for the given channel
     *  @see LoKi::Cuts::HLT_EXECUTIONSTAGE
     *  @see LHCb::HltDecReport::executionStage
     *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
     *  @date 2010-05-17
     */
    class GAUDI_API ExecutionStage : public ErrorBits
    {
    public:
      // ======================================================================
      /// constructor from the channel name
      ExecutionStage ( const std::string&      name ) ;
      /// constructor from the channel name
      ExecutionStage ( const Gaudi::StringKey& name ) ;
      /// MANDATORY: virtual destructor
      virtual ~ExecutionStage() ;
      /// MANDATORY: clone method ( "virtual constructor")
      virtual  ExecutionStage* clone () const ;
      /// MANDATORY: the only one essential methor
      result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      ExecutionStage () ;                // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================
    /** @class NumberOfCandidates
     *  Simple functor to extract the execution stage for the given channel
     *  @see LoKi::Cuts::HLT_NCANDIDATES
     *  @see LHCb::HltDecReport::numberOfCandidates
     *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
     *  @date 2010-05-17
     */
    class GAUDI_API NumberOfCandidates : public ErrorBits
    {
    public:
      // ======================================================================
      /// constructor from the channel name
      NumberOfCandidates ( const std::string&      name ) ;
      /// constructor from the channel name
      NumberOfCandidates ( const Gaudi::StringKey& name ) ;
      /// MANDATORY: virtual destructor
      virtual ~NumberOfCandidates() ;
      /// MANDATORY: clone method ( "virtual constructor")
      virtual  NumberOfCandidates* clone () const ;
      /// MANDATORY: the only one essential methor
      result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      NumberOfCandidates () ;  // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Saturated
     *  Simple functor to extract the execution stage for the given channel
     *  @see LoKi::Cuts::HLT_SATURATED
     *  @see LHCb::HltDecReport::numberOfCandidates
     *  @see LHCb::HltDecReport::saturatedSaturated
     *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
     *  @date 2010-05-17
     */
    class GAUDI_API Saturated
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the channel name
      Saturated ( const std::string&      name ) ;
      /// constructor from the channel name
      Saturated ( const Gaudi::StringKey& name ) ;
      /// MANDATORY: virtual destructor
      virtual ~Saturated() ;
      /// MANDATORY: clone method ( "virtual constructor")
      virtual  Saturated* clone () const ;
      /// MANDATORY: the only one essential methor
      result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      Saturated () ;  // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      const Gaudi::StringKey& channel( ) const { return m_key ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the channel key
      Gaudi::StringKey m_key ; // the channle key
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CountErrorBits
     *  @see LoKi::Cuts::HLT_COUNT_ERRORBITS
     *  @see LHCb::HltDecReport::errorBits
     *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
     *  @date 2010-05-17
     */
    class GAUDI_API CountErrorBits
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function
    {
    public:
      // =====================================================================
      /// constructor from the list of lines & mask
      CountErrorBits
      ( const std::vector<std::string>&      lines ,
        const unsigned int                   mask  ) ;
      // =====================================================================
      /// constructor from the list of lines & mask
      CountErrorBits
      ( const std::vector<Gaudi::StringKey>& lines ,
        const unsigned int                   mask  ) ;
      // =====================================================================
      /// constructor from the lines & mask
      CountErrorBits
      ( const std::string& line1 ,
        const std::string& line2 ,
        const unsigned int mask  ) ;
      // =====================================================================
      /// constructor from the lines & mask
      CountErrorBits
      ( const std::string& line1 ,
        const std::string& line2 ,
        const std::string& line3 ,
        const unsigned int mask  ) ;
      // =====================================================================
      /// constructor from the lines & mask
      CountErrorBits
      ( const std::string& line1 ,
        const std::string& line2 ,
        const std::string& line3 ,
        const std::string& line4 ,
        const unsigned int mask  ) ;
      // =====================================================================
      /// MANDATORY: virtual desctructor
      virtual ~CountErrorBits() ;
      /// MANDATORY: clone method ( "virtual constructor")
      virtual  CountErrorBits* clone () const ;
      /// MANDATORY: the only one essential methor
      result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      CountErrorBits () ;  // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// list of lines
      std::vector<Gaudi::StringKey> m_lines ; // list of lines
      /// the mask
      unsigned int                  m_mask  ; // the mask
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CountErrorBitsRegex
     *  @see LoKi::Cuts::HLT_COUNT_ERRORBITS
     *  @see LHCb::HltDecReport::errorBits
     *  @author Vanya BELYAEV  Ivan.Belyaev@cern.ch
     *  @date 2010-05-17
     */
    class GAUDI_API CountErrorBitsRegex
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function
    {
    public:
      // =====================================================================
      /// constructor from the regular expression & mask
      CountErrorBitsRegex
      ( const std::string&  expression ,
        const unsigned int  mask       ) ;
      /// MANDATORY: virtual destructor
      virtual ~CountErrorBitsRegex() ;
      /// MANDATORY: clone method ( "virtual constructor")
      virtual  CountErrorBitsRegex* clone () const ;
      /// MANDATORY: the only one essential method
      result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      CountErrorBitsRegex () ;          // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// regurn the regular expression
      const boost::regex& expression () const { return m_expression ; }
      // ======================================================================
    private:
      // ======================================================================
      std::string   m_pattern    ;
      /// the regular expression
      boost::regex  m_expression ;                    // the regular expression
      /// the mask
      unsigned int  m_mask       ;                    // the mask
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HltRoutingBits
     *  Simple predicate to check the hlt routing bits
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2010-05-17
     */
    class GAUDI_API HltRoutingBits
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from routing bits
      HltRoutingBits ( const LoKi::HLT::RoutingBits& bits ) ;
      /// MANDATORY: virtual destructor
      virtual ~HltRoutingBits () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HltRoutingBits* clone() const ;
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
      HltRoutingBits () ;          // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the bits
      LoKi::HLT::RoutingBits m_bits ;                               // the bits
      // ======================================================================
    };
    // ========================================================================
  } //end of namespace LoKi::HLT
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_HLT_H
// ============================================================================
