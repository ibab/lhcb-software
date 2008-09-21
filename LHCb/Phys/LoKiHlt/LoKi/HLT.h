// $Id: HLT.h,v 1.2 2008-09-21 14:50:34 ibelyaev Exp $
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
    /** @class HasSelection
     *  simple function which allows to check the existence 
     *  of the selection with the given name  
     *  @see LHCb::HltDecReports 
     *  @see LoKi::Cuts::HLT_HAS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class HasSelection 
      : public LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate
    {
    protected:
      // ======================================================================
      typedef std::vector<std::string>                                  Names ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection name 
      HasSelection ( const std::string& name  ) ;
      /// constructor from the selection names ("OR") 
      HasSelection ( const std::string& name1 , 
                     const std::string& name2 ) ;
      /// constructor from the selection names ("OR") 
      HasSelection ( const std::string& name1 , 
                     const std::string& name2 , 
                     const std::string& name3 ) ;
      /// constructor from the selection names ("OR") 
      HasSelection ( const std::string& name1 , 
                     const std::string& name2 , 
                     const std::string& name3 ,
                     const std::string& name4 ) ;
      /// constructor form the selection names ("OR") 
      HasSelection ( const Names&       names ) ;
      /// MANDATORY: virtual destructor 
      virtual ~HasSelection () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasSelection* clone() const 
      { return new HasSelection ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( s , "HLT_HAS" ) ; }
      // ======================================================================
    protected:
      // ======================================================================
      // get the selection names 
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
      HasSelection () ;                  // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the list of selection names 
      Names m_names ; // the list of selection names 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PassSelection
     *  simple function which allows to check if the selection  
     *  has been passed  
     *  of the selection with the given name  
     *  @see LHCb::HltDecReports 
     *  @see LoKi::Cuts::HLT_PASS
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-19
     */
    class PassSelection : public HasSelection
    {
    public:
      // ======================================================================
      /// constructor from the selection name 
      PassSelection ( const std::string& name  ) 
        : HasSelection ( name ) 
      {}
      /// constructor from the selection names ("OR") 
      PassSelection ( const std::string& name1 , 
                      const std::string& name2 ) 
        : HasSelection ( name1 , name2 ) 
      {}
      /// constructor from the selection names ("OR") 
      PassSelection ( const std::string& name1 , 
                      const std::string& name2 , 
                      const std::string& name3 ) 
        : HasSelection ( name1 , name2 , name3 ) 
      {}
      /// constructor from the selection names ("OR") 
      PassSelection ( const std::string& name1 , 
                      const std::string& name2 , 
                      const std::string& name3 ,
                      const std::string& name4 ) 
        : HasSelection ( name1 , name2 , name3 , name4 ) 
      {}
      /// constructor form the selection names ("OR") 
      PassSelection ( const Names& names ) : HasSelection ( names ) {}
      /// MANDATORY: virtual destructor 
      virtual ~PassSelection () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  PassSelection* clone() const 
      { return new PassSelection ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( s , "HLT_PASS" ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      PassSelection () ;                 // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Size 
     *  simple function which returns the number of known selections 
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
     *  simple function which returnn the number of 'passes' selections 
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
  } //end of namespace LoKi::HLT
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLT_H
// ============================================================================
