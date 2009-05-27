// $Id: Hlt.cpp,v 1.4 2009-05-27 06:40:02 graven Exp $
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
// The END 
// ============================================================================
