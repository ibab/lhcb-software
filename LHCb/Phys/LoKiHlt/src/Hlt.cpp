// $Id: Hlt.cpp,v 1.7 2010-05-18 07:13:33 cattanem Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/HLT.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace LoKi::HLT 
 *  @date 2008-09-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// constructor from the decision name 
// ============================================================================
LoKi::HLT::HasDecision::HasDecision 
( const std::string& name  ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names ( 1 , name ) 
{}
// ============================================================================
// constructor from the decision names 
// ============================================================================
LoKi::HLT::HasDecision::HasDecision 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
}
// ============================================================================
// constructor from the decision names 
// ============================================================================
LoKi::HLT::HasDecision::HasDecision 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  m_names.push_back ( name3 ) ;
}
// ============================================================================
// constructor from the decision names 
// ============================================================================
LoKi::HLT::HasDecision::HasDecision 
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 , 
  const std::string& name4 ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  m_names.push_back ( name3 ) ;
  m_names.push_back ( name4 ) ;
}
// ============================================================================
// constructor form the decision names ("OR") 
// ============================================================================
LoKi::HLT::HasDecision::HasDecision 
( const LoKi::HLT::HasDecision::Names& names ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names ( names ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::HasDecision::result_type
LoKi::HLT::HasDecision::operator() 
  ( LoKi::HLT::HasDecision::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  for ( Names::const_iterator iname = begin() ; end() != iname ; ++iname ) 
  { if ( a -> hasDecisionName ( *iname ) ) { return true ; } } // RETURN
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::HLT::HasDecision::print 
( std::ostream&      s    , 
  const std::string& name ) const
{
  switch ( names().size() ) 
  {
  case 1 : 
    return s << name << "('" 
             <<   names().front()    << "') " ;  // RETURN 
  case 2 : 
    return s << name << "('"
             << *(names().begin()  ) << "','"
             << *(names().begin()+1) << "') " ;  // RETURN 
  case 3 : 
    return s << name << "('"
             << *(names().begin()  ) << "','"
             << *(names().begin()+1) << "','"
             << *(names().begin()+2) << "') " ;  // RETURN 
  case 4 : 
    return s << name << "('" 
             << *(names().begin()  ) << "','"
             << *(names().begin()+1) << "','"
             << *(names().begin()+2) << "','"
             << *(names().begin()+3) << "') " ;  // RETURN 
    
  default:
    return s << name << "(" 
             << Gaudi::Utils::toString ( names() ) << "') " ;  // RETURN
  }
  // This is unreachable
  //  return s << name << "(" 
  //           << Gaudi::Utils::toString ( names() ) << "') " ;  // RETURN
}

// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::PassDecision::result_type
LoKi::HLT::PassDecision::operator() 
  ( LoKi::HLT::PassDecision::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  //
  typedef LHCb::HltDecReports::Container::const_iterator IT ;
  IT _e = a -> decReports ().end   () ;
  for ( Names::const_iterator iname = begin() ; end() != iname ; ++iname ) 
  { 
    IT found = a -> find ( *iname ) ;
    if ( _e != found && found->second.decision () ) { return true ; }  // RETURN 
  }
  return false ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::Size::result_type
LoKi::HLT::Size::operator() 
  ( LoKi::HLT::Size::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  return a -> size() ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::NPass::result_type
LoKi::HLT::NPass::operator() 
  ( LoKi::HLT::NPass::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  //
  size_t nPass = 0  ;
  typedef LHCb::HltDecReports::Container::const_iterator IT ;
  IT _end = a -> end   () ;
  for ( IT it = a -> begin () ; _end != it ; ++it ) 
  { if ( it->second.decision() ) { ++nPass ; } }
  //
  return nPass ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::Decision::result_type
LoKi::HLT::Decision::operator() 
  ( LoKi::HLT::Decision::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  //
  typedef LHCb::HltDecReports::Container::const_iterator IT ;
  IT _end = a -> end   () ;
  for ( IT it = a -> begin () ; _end != it ; ++it ) 
  { if ( it->second.decision() ) { return true ; } }  // RETURN
  //
  return false ;
}


// ============================================================================
// constructor from one "special" decicion
// ============================================================================
LoKi::HLT::DecisionBut::DecisionBut 
( const std::string& name )
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_special ( 1 , name ) 
{}
// ============================================================================
// constructor from two "special" decicions
// ============================================================================
LoKi::HLT::DecisionBut::DecisionBut 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_special () 
{
  m_special.push_back ( name1 ) ;
  m_special.push_back ( name2 ) ;
}
// ============================================================================
// constructor from three "special" decicions
// ============================================================================
LoKi::HLT::DecisionBut::DecisionBut 
( const std::string& name1 , 
  const std::string& name2 , 
  const std::string& name3 ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_special () 
{
  m_special.push_back ( name1 ) ;
  m_special.push_back ( name2 ) ;
  m_special.push_back ( name3 ) ;
}
// ============================================================================
// constructor from four "special" decicions
// ============================================================================
LoKi::HLT::DecisionBut::DecisionBut 
( const std::string& name1 , 
  const std::string& name2 , 
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_special () 
{
  m_special.push_back ( name1 ) ;
  m_special.push_back ( name2 ) ;
  m_special.push_back ( name3 ) ;
  m_special.push_back ( name4 ) ;
}
// ============================================================================
// constructor from vector of "special" decicions
// ============================================================================
LoKi::HLT::DecisionBut::DecisionBut 
( const LoKi::HLT::DecisionBut::Names& names ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_special ( names ) 
{}
// ============================================================================
// constructor from vector of "special" decicions
// ============================================================================
LoKi::HLT::DecisionBut::DecisionBut 
( const std::vector<std::string>& names ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_special ( names.begin() , names.end() ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::DecisionBut::result_type 
LoKi::HLT::DecisionBut::operator() 
  ( LoKi::HLT::DecisionBut::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  //
  typedef LHCb::HltDecReports::Container::const_iterator IT ;
  IT _end = a -> end   () ;
  size_t nPositive = 0 ;
  for ( IT it = a -> begin () ; _end != it ; ++it ) 
  {
    // the decision 
    if (it->second.decision() == 0) { continue ; }              // CONTINUE
    ++nPositive ;
    if ( nPositive > m_special.size () )     { return true ; }  // RETURN 
    if ( m_special.end() == 
         std::find ( m_special.begin () , 
                     m_special.end   () , 
                     it->first ) )           { return true ; }   // RETURN 
  }
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::HLT::DecisionBut::fillStream 
( std::ostream& s ) const 
{
  s << " HLT_PASSIGNORING(" ;
  switch ( m_special.size() ) 
  {
  case 1 : 
    s << "'"  << m_special[0] << "'"    ; break ;
  case 2 : 
    s << "'"  << m_special[0] << "'" 
      << ",'" << m_special[1] << "'"    ; break ;
  case 3 : 
    s << "'"  << m_special[0] << "'" 
      << ",'" << m_special[1] << "'"
      << ",'" << m_special[2] << "'"    ; break ;
  case 4 : 
    s << "'"  << m_special[0] << "'" 
      << ",'" << m_special[1] << "'"
      << ",'" << m_special[2] << "'"
      << ",'" << m_special[3] << "'"    ; break ;
  default : 
    s << Gaudi::Utils::toString ( m_special ) ;
  }
  //
  return s << ") " ;
}



// ============================================================================
// constructor 
// ============================================================================
LoKi::HLT::HasDecisionSubString::HasDecisionSubString
( const std::string& substr ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate()
  , m_substr ( substr ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::HLT::HasDecisionSubString::~HasDecisionSubString(){}
// ============================================================================
// clone method 
// ============================================================================
LoKi::HLT::HasDecisionSubString*
LoKi::HLT::HasDecisionSubString::clone() const 
{ return new LoKi::HLT::HasDecisionSubString(*this) ; }
// ============================================================================
// the actual evaluation 
// ============================================================================
LoKi::HLT::HasDecisionSubString::result_type
LoKi::HLT::HasDecisionSubString::operator() 
  ( LoKi::HLT::HasDecisionSubString::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // loop over all selecttions and match the names 
  for ( LHCb::HltDecReports::Container::const_iterator isel = a->begin() ; 
        a->end() != isel ; ++isel ) 
  { if ( std::string::npos != isel->first.find( substr() ) ) { return true ; } }
  return false ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& 
LoKi::HLT::HasDecisionSubString::fillStream ( std::ostream& s ) const 
{ return s << " HLT_HAS_SUBSTR('" << substr() << "') " ; }



// ============================================================================
// constructor 
// ============================================================================
LoKi::HLT::PassDecisionSubString::PassDecisionSubString
( const std::string& substr ) 
  : LoKi::HLT::HasDecisionSubString( substr ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::HLT::PassDecisionSubString::~PassDecisionSubString(){}
// ============================================================================
// clone method 
// ============================================================================
LoKi::HLT::PassDecisionSubString*
LoKi::HLT::PassDecisionSubString::clone() const 
{ return new LoKi::HLT::PassDecisionSubString(*this) ; }
// ============================================================================
// the actual evaluation 
// ============================================================================
LoKi::HLT::PassDecisionSubString::result_type
LoKi::HLT::PassDecisionSubString::operator() 
  ( LoKi::HLT::PassDecisionSubString::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // loop over all selecttions and match the names 
  for ( LHCb::HltDecReports::Container::const_iterator isel = a->begin() ; 
        a->end() != isel ; ++isel ) 
  { 
    if ( isel->second.decision() && std::string::npos != isel->first.find( substr() ) ) 
    { return true ; }
  }
  return false ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& 
LoKi::HLT::PassDecisionSubString::fillStream ( std::ostream& s ) const 
{ return s << " HLT_PASS_SUBSTR('" << substr() << "') " ; }




// ============================================================================
// constructor 
// ============================================================================
LoKi::HLT::DecisionButSubString::DecisionButSubString
( const std::string& substr ) 
  : LoKi::HLT::PassDecisionSubString( substr ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::HLT::DecisionButSubString::~DecisionButSubString(){}
// ============================================================================
// clone method 
// ============================================================================
LoKi::HLT::DecisionButSubString*
LoKi::HLT::DecisionButSubString::clone() const 
{ return new LoKi::HLT::DecisionButSubString(*this) ; }
// ============================================================================
// the actual evaluation 
// ============================================================================
LoKi::HLT::DecisionButSubString::result_type
LoKi::HLT::DecisionButSubString::operator() 
  ( LoKi::HLT::DecisionButSubString::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // loop over all selections and match the names 
  for ( LHCb::HltDecReports::Container::const_iterator isel = a->begin() ; 
        a->end() != isel ; ++isel ) 
  { 
    if ( isel->second.decision() && std::string::npos == isel->first.find( substr() ) ) 
    { return true ; }
  }
  return false ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& 
LoKi::HLT::DecisionButSubString::fillStream ( std::ostream& s ) const 
{ return s << " HLT_PASSIGNORING_SUBSTR('" << substr() << "') " ; }


// ============================================================================
// constructor 
// ============================================================================
LoKi::HLT::HasDecisionRegex::HasDecisionRegex
( const std::string& substr ) 
  : LoKi::HLT::HasDecisionSubString( substr ) 
  , m_expression ( substr )
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::HLT::HasDecisionRegex::~HasDecisionRegex(){}
// ============================================================================
// clone method 
// ============================================================================
LoKi::HLT::HasDecisionRegex*
LoKi::HLT::HasDecisionRegex::clone() const 
{ return new LoKi::HLT::HasDecisionRegex(*this) ; }
// ============================================================================
// the actual evaluation 
// ============================================================================
LoKi::HLT::HasDecisionRegex::result_type
LoKi::HLT::HasDecisionRegex::operator() 
  ( LoKi::HLT::HasDecisionRegex::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // loop over all selections and match the names 
  boost::smatch match ;
  for ( LHCb::HltDecReports::Container::const_iterator isel = a->begin() ; 
        a->end() != isel ; ++isel ) 
  { if ( boost::regex_match ( isel->first , expression() ) ) { return true ; } }
  //
  return false ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& 
LoKi::HLT::HasDecisionRegex::fillStream ( std::ostream& s ) const 
{ return s << " HLT_HAS_RE('" << substr() << "') " ; }



// ============================================================================
// constructor 
// ============================================================================
LoKi::HLT::PassDecisionRegex::PassDecisionRegex
( const std::string& substr ) 
  : LoKi::HLT::HasDecisionRegex( substr ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::HLT::PassDecisionRegex::~PassDecisionRegex(){}
// ============================================================================
// clone method 
// ============================================================================
LoKi::HLT::PassDecisionRegex*
LoKi::HLT::PassDecisionRegex::clone() const 
{ return new LoKi::HLT::PassDecisionRegex(*this) ; }
// ============================================================================
// the actual evaluation 
// ============================================================================
LoKi::HLT::PassDecisionRegex::result_type
LoKi::HLT::PassDecisionRegex::operator() 
  ( LoKi::HLT::PassDecisionRegex::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // loop over all selections and match the names 
  for ( LHCb::HltDecReports::Container::const_iterator isel = a->begin() ; 
        a->end() != isel ; ++isel ) 
  { 
    if ( isel->second.decision() && 
         boost::regex_match ( isel->first , expression() ) ) { return true ; }
  }
  return false ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& 
LoKi::HLT::PassDecisionRegex::fillStream ( std::ostream& s ) const 
{ return s << " HLT_PASS_RE('" << substr() << "') " ; }



// ============================================================================
// constructor 
// ============================================================================
LoKi::HLT::DecisionButRegex::DecisionButRegex
( const std::string& substr ) 
  : LoKi::HLT::PassDecisionRegex( substr ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::HLT::DecisionButRegex::~DecisionButRegex(){}
// ============================================================================
// clone method 
// ============================================================================
LoKi::HLT::DecisionButRegex*
LoKi::HLT::DecisionButRegex::clone() const 
{ return new LoKi::HLT::DecisionButRegex(*this) ; }
// ============================================================================
// the actual evaluation 
// ============================================================================
LoKi::HLT::DecisionButRegex::result_type
LoKi::HLT::DecisionButRegex::operator() 
  ( LoKi::HLT::DecisionButRegex::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // loop over all selections and match the names 
  for ( LHCb::HltDecReports::Container::const_iterator isel = a->begin() ; 
        a->end() != isel ; ++isel ) 
  { 
    if ( isel->second.decision() && 
         boost::regex_match( isel->first , expression() ) ) { return true ; }
  }
  return false ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& 
LoKi::HLT::DecisionButRegex::fillStream ( std::ostream& s ) const 
{ return s << " HLT_PASSIGNORING_RE('" << substr() << "') " ; }
// ============================================================================

// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::ErrorBits::ErrorBits ( const std::string& name ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_key ( name ) 
{}
// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::ErrorBits::ErrorBits ( const Gaudi::StringKey& name ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_key ( name ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::HLT::ErrorBits::~ErrorBits () {}
// ============================================================================
// MANDATORY: clone method ( "virtual constructor")
// ============================================================================ 
LoKi::HLT::ErrorBits* LoKi::HLT::ErrorBits::clone () const 
{ return new LoKi::HLT::ErrorBits ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methor 
// ============================================================================
LoKi::HLT::ErrorBits::result_type
LoKi::HLT::ErrorBits::operator() ( LoKi::HLT::ErrorBits::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // find the selection by name 
  LHCb::HltDecReports::Container::const_iterator isel = a->find ( m_key ) ;
  if ( a->end() == isel ) 
  {
    Error ( "No decision has been found for '" + m_key.str() + "', return -1" ) ;
    return -1 ;
  }
  //
  return isel->second.errorBits() ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::HLT::ErrorBits::fillStream ( std::ostream& s ) const 
{ return s << " HLT_ERRORBITS('" + m_key.str() + ") " ; }
// ============================================================================



// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::ExecutionStage::ExecutionStage ( const std::string& name ) 
  : LoKi::HLT::ErrorBits ( name ) 
{}
// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::ExecutionStage::ExecutionStage ( const Gaudi::StringKey& name ) 
  : LoKi::HLT::ErrorBits ( name ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::HLT::ExecutionStage::~ExecutionStage () {}
// ============================================================================
// MANDATORY: clone method ( "virtual constructor")
// ============================================================================ 
LoKi::HLT::ExecutionStage* LoKi::HLT::ExecutionStage::clone () const 
{ return new LoKi::HLT::ExecutionStage ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methor 
// ============================================================================
LoKi::HLT::ExecutionStage::result_type
LoKi::HLT::ExecutionStage::operator() 
  ( LoKi::HLT::ExecutionStage::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // find the selection by name 
  LHCb::HltDecReports::Container::const_iterator isel = a->find ( channel()  ) ;
  if ( a->end() == isel ) 
  {
    Error ( "No decision has been found for '" + 
            channel().str() + "', return -1" ) ;
    return -1 ;
  }
  //
  return isel->second.executionStage () ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::HLT::ExecutionStage::fillStream ( std::ostream& s ) const 
{ return s << " HLT_EXECUTIONSTAGE('" + channel().str() + ") " ; }
// ============================================================================

// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::NumberOfCandidates::NumberOfCandidates ( const std::string& name ) 
  : LoKi::HLT::ErrorBits ( name ) 
{}
// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::NumberOfCandidates::NumberOfCandidates ( const Gaudi::StringKey& name ) 
  : LoKi::HLT::ErrorBits ( name ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::HLT::NumberOfCandidates::~NumberOfCandidates () {}
// ============================================================================
// MANDATORY: clone method ( "virtual constructor")
// ============================================================================ 
LoKi::HLT::NumberOfCandidates* LoKi::HLT::NumberOfCandidates::clone () const 
{ return new LoKi::HLT::NumberOfCandidates ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methor 
// ============================================================================
LoKi::HLT::NumberOfCandidates::result_type
LoKi::HLT::NumberOfCandidates::operator() ( LoKi::HLT::NumberOfCandidates::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // find the selection by name 
  LHCb::HltDecReports::Container::const_iterator isel = a->find ( channel () ) ;
  if ( a->end() == isel ) 
  {
    Error ( "No decision has been found for '" 
            + channel().str() + "', return -1" ) ;
    return -1 ;
  }
  //
  return isel->second.numberOfCandidates () ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::HLT::NumberOfCandidates::fillStream ( std::ostream& s ) const 
{ return s << " HLT_NCANDIDATES('" + channel().str() + ") " ; }
// ============================================================================



// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::Saturated::Saturated
( const std::string&      name )
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate()
  , m_key ( name ) 
{}
// ============================================================================
// constructor from the channel name 
// ============================================================================
LoKi::HLT::Saturated::Saturated
( const Gaudi::StringKey&  name )
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate()
  , m_key ( name ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::HLT::Saturated::~Saturated() {}
// ============================================================================
// MANDATORY: clone method ( "virtual constructor")
// ============================================================================
LoKi::HLT::Saturated*
LoKi::HLT::Saturated::clone() const { return new Saturated(*this) ; }
// ============================================================================
// MANDATORY: the only one essential methor 
// ============================================================================
LoKi::HLT::Saturated::result_type
LoKi::HLT::Saturated::operator() 
  ( LoKi::HLT::Saturated::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // find the selection by name 
  LHCb::HltDecReports::Container::const_iterator isel = a->find ( channel () ) ;
  if ( a->end() == isel ) 
  {
    Error ( "No decision has been found for '" 
            + channel().str() + "', return false" ) ;
    return false ;
  }
  //
  return isel->second.numberOfCandidates () == 
    LHCb::HltDecReport::saturatedNumberOfCandidates() ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::HLT::Saturated::fillStream ( std::ostream& s ) const 
{ return s << " HLT_SATURATED('" + channel().str() + ") " ; }
// ============================================================================

// ============================================================================
// constructor from the list of lines & mask 
// ============================================================================
LoKi::HLT::CountErrorBits::CountErrorBits
( const std::vector<std::string>&      lines , 
  const unsigned int                   mask  ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_lines ( lines.begin() , lines.end() ) 
  , m_mask  ( mask )
{}
// ============================================================================
// constructor from the list of lines & mask 
// ============================================================================
LoKi::HLT::CountErrorBits::CountErrorBits
( const std::vector<Gaudi::StringKey>&  lines , 
  const unsigned int                    mask  ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_lines ( lines ) 
  , m_mask  ( mask  )
{}
// ============================================================================
// constructor from the lines & mask 
// ============================================================================
LoKi::HLT::CountErrorBits::CountErrorBits
( const std::string& line1 , 
  const std::string& line2 , 
  const unsigned int mask  ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_lines () 
  , m_mask  ( mask  )
{
  m_lines.push_back ( line1 ) ;
  m_lines.push_back ( line2 ) ;  
}  
// ============================================================================
// constructor from the lines & mask 
// ============================================================================
LoKi::HLT::CountErrorBits::CountErrorBits
( const std::string& line1 , 
  const std::string& line2 , 
  const std::string& line3 , 
  const unsigned int mask  ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_lines () 
  , m_mask  ( mask  )
{
  m_lines.push_back ( line1 ) ;
  m_lines.push_back ( line2 ) ;  
  m_lines.push_back ( line3 ) ;  
}
// ============================================================================
// constructor from the lines & mask 
// ============================================================================
LoKi::HLT::CountErrorBits::CountErrorBits
( const std::string& line1 , 
  const std::string& line2 , 
  const std::string& line3 , 
  const std::string& line4 , 
  const unsigned int mask  ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_lines () 
  , m_mask  ( mask  )
{
  m_lines.push_back ( line1 ) ; 
  m_lines.push_back ( line2 ) ;  
  m_lines.push_back ( line3 ) ;  
  m_lines.push_back ( line4 ) ;  
}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::HLT::CountErrorBits::~CountErrorBits () {}
// ============================================================================
// MANDATORY: clone method ( "virtual constructor")
// ============================================================================
LoKi::HLT::CountErrorBits*
LoKi::HLT::CountErrorBits::clone() const
{ return new CountErrorBits ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methor 
// ============================================================================
LoKi::HLT::CountErrorBits::result_type 
LoKi::HLT::CountErrorBits::operator() 
  ( LoKi::HLT::CountErrorBits::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  // loop over the lines 
  size_t result = 0 ;
  for ( std::vector<Gaudi::StringKey>::const_iterator iline = 
          m_lines.begin() ; m_lines.end() != iline ; ++iline ) 
  {
    // find the selection by name 
    LHCb::HltDecReports::Container::const_iterator isel = a->find ( *iline ) ;
    if ( a->end() == isel ) 
    {
      Error ( "No decision found for '" + iline->str() + "'" ) ;
      continue ;
    }
    if ( isel->second.errorBits() & m_mask ) { ++result ; }
  }
  //
  return result  ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::HLT::CountErrorBits::fillStream ( std::ostream& s ) const 
{ 
  s << " HLT_COUNT_ERRORBITS( " ;
  //
  switch ( m_lines.size() ) 
  {
  case 2 : 
    return s << "'"   << m_lines[0] 
             << "','" << m_lines[1] 
             << "',"  << m_mask     << ") " ;
  case 3 : 
    return s << "'"   << m_lines[0] 
             << "','" << m_lines[1] 
             << "','" << m_lines[2] 
             << "',"  << m_mask     << ") " ;
  case 4 : 
    return s << "'"   << m_lines[0] 
             << "','" << m_lines[1] 
             << "','" << m_lines[2] 
             << "','" << m_lines[3] 
             << "',"  << m_mask     << ") " ;
  default :
    break ;
  }
  //
  Gaudi::Utils::toStream ( m_lines , s ) ;
  return s << "," << m_mask << ") " ;
}
// ============================================================================

// ============================================================================
// constructor from the regular expression & mask
// ============================================================================
LoKi::HLT::CountErrorBitsRegex::CountErrorBitsRegex 
( const std::string&  expression , 
  const unsigned int  mask       )
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Function () 
  , m_pattern    ( expression ) 
  , m_expression ( expression ) 
  , m_mask       ( mask ) 
{}
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::HLT::CountErrorBitsRegex::~CountErrorBitsRegex () {}
// ============================================================================  
// MANDATORY: clone method ( "virtual constructor")
// ============================================================================
LoKi::HLT::CountErrorBitsRegex*
LoKi::HLT::CountErrorBitsRegex::clone() const 
{ return new CountErrorBitsRegex ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methor 
// ============================================================================
LoKi::HLT::CountErrorBitsRegex::result_type 
LoKi::HLT::CountErrorBitsRegex::operator() 
  ( LoKi::HLT::CountErrorBitsRegex::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  //
  size_t result = 0 ;
  boost::smatch match ;
  for ( LHCb::HltDecReports::Container::const_iterator isel = a->begin() ; 
        a->end() != isel ; ++isel ) 
  { 
    if ( boost::regex_match ( isel->first , expression() ) )
    { if ( isel->second.errorBits() & m_mask ) { ++result ; } }
  }
  //
  return result ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::HLT::CountErrorBitsRegex::fillStream ( std::ostream& s ) const 
{ return s << " HLT_COUNT_ERRORBITS_RE('" << m_pattern << "' ) "; }
// ============================================================================


// ============================================================================
// constructor from routing bits 
// ============================================================================
LoKi::HLT::HltRoutingBits::HltRoutingBits
( const LoKi::HLT::RoutingBits& bits ) 
  :  LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate() 
  , m_bits ( bits ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::HLT::HltRoutingBits::~HltRoutingBits () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::HLT::HltRoutingBits*
LoKi::HLT::HltRoutingBits::clone() const 
{ return new HltRoutingBits ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::HltRoutingBits::result_type 
LoKi::HLT::HltRoutingBits::operator() 
  ( LoKi::HLT::HltRoutingBits::argument /* a */ ) const 
{
  return m_bits() ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::HLT::HltRoutingBits::fillStream ( std::ostream& s ) const 
{ return s << " HLT_" << m_bits ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
