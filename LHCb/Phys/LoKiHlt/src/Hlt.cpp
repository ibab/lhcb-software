// $Id: Hlt.cpp,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $
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
// constructor from the selection name 
// ============================================================================
LoKi::HLT::HasSelection::HasSelection 
( const std::string& name  ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names ( 1 , name ) 
{}
// ============================================================================
// constructor from the selection names 
// ============================================================================
LoKi::HLT::HasSelection::HasSelection 
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
}
// ============================================================================
// constructor from the selection names 
// ============================================================================
LoKi::HLT::HasSelection::HasSelection 
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
// constructor from the selection names 
// ============================================================================
LoKi::HLT::HasSelection::HasSelection 
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
// constructor form the selection names ("OR") 
// ============================================================================
LoKi::HLT::HasSelection::HasSelection 
( const LoKi::HLT::HasSelection::Names& names ) 
  : LoKi::BasicFunctors<const LHCb::HltDecReports*>::Predicate () 
  , m_names ( names ) 
{}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::HLT::HasSelection::result_type
LoKi::HLT::HasSelection::operator() 
  ( LoKi::HLT::HasSelection::argument a ) const 
{
  Assert ( 0 != a , "const LHCb::HltDecReports* points to NULL!" ) ;
  for ( Names::const_iterator iname = begin() ; end() != iname ; ++iname ) 
  { if ( a -> hasSelectionName ( *iname ) ) { return true ; } } // RETURN
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::HLT::HasSelection::print 
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
LoKi::HLT::PassSelection::result_type
LoKi::HLT::PassSelection::operator() 
  ( LoKi::HLT::PassSelection::argument a ) const 
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
// The END 
// ============================================================================
