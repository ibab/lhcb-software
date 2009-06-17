// $Id: Hlt.cpp,v 1.5 2009-06-17 12:02:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
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
             <<   names().front()    << "')" ;  // RETURN 
  case 2 : 
    return s << name << "('"
             << *(names().begin()  ) << "','"
             << *(names().begin()+1) << "')" ;  // RETURN 
  case 3 : 
    return s << name << "('"
             << *(names().begin()  ) << "','"
             << *(names().begin()+1) << "','"
             << *(names().begin()+2) << "')" ;  // RETURN 
  case 4 : 
    return s << name << "('" 
             << *(names().begin()  ) << "','"
             << *(names().begin()+1) << "','"
             << *(names().begin()+2) << "','"
             << *(names().begin()+3) << "')" ;  // RETURN 
    
  default:
    return s << name << "(" 
             << Gaudi::Utils::toString ( names() ) << "')" ;  // RETURN
  }
  return s << name << "(" 
           << Gaudi::Utils::toString ( names() ) << "')" ;  // RETURN
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
    const bool decision = it->second.decision () ;
    if ( !decision ) { continue ; }                    // CONTINUE ;
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
  s << "HLT_PASSIGNORING(" ;
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
  return s << ")" ;
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
{ return s << "HLT_HAS_SUBSTR('" << substr() << "')" ; }



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
{ return s << "HLT_PASS_SUBSTR('" << substr() << "')" ; }




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
{ return s << "HLT_PASSIGNORING_SUBSTR('" << substr() << "')" ; }


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
{ return s << "HLT_HAS_RE('" << substr() << "')" ; }



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
{ return s << "HLT_PASS_RE('" << substr() << "')" ; }



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
{ return s << "HLT_PASSIGNORING_RE('" << substr() << "')" ; }


// ============================================================================
// The END 
// ============================================================================
