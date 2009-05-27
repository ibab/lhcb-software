// $Id: HLT.h,v 1.4 2009-05-27 06:40:02 graven Exp $
// ============================================================================
#ifndef LOKI_HLT_H 
#define LOKI_HLT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
// L0Event
// ============================================================================
#include "Event/HltDecReports.h"
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
    class HasDecision 
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    protected:
      // ======================================================================
      typedef std::vector<std::string>                                  Names ;
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
    class PassDecision : public HasDecision
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
    class Size 
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function
    {
    public:
      // ======================================================================
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
    class NPass
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function
    {
    public:
      // ======================================================================
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
    class Decision
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    public:
      // ======================================================================
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
     *  @see LoKi::Cuts::HLT_PASSBUT 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-03
     */
    class DecisionBut 
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    protected:
      // ======================================================================
      typedef std::vector<std::string>                                  Names ;
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
  } //end of namespace LoKi::HLT
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLT_H
// ============================================================================
