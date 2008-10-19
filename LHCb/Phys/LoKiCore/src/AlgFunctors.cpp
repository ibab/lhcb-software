// $Id: AlgFunctors.cpp,v 1.1 2008-10-19 16:11:40 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include  "GaudiKernel/Bootstrap.h"
#include  "GaudiKernel/ISvcLocator.h"
#include  "GaudiKernel/IAlgManager.h"
#include  "GaudiKernel/SmartIF.h"
#include  "GaudiKernel/ToStream.h"
#include  "GaudiKernel/Algorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/AlgFunctors.h"
#include "LoKi/Report.h"
#include "LoKi/Services.h"
// ============================================================================
/** @file
 *  Implementation file for functions from namespace LoKi::Algorithms 
 *  @see LoKi::Algorithms 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-010-14 
 */ 
// ============================================================================
namespace 
{
  // ==========================================================================
  const IAlgManager* const s_ALGMANAGER = 0 ;
  const IAlgorithm*  const s_ALGORITHM  = 0 ;
  // ==========================================================================
  // get the algorithm manager
  SmartIF<IAlgManager> getAlgManager ( ISvcLocator* loc ) 
  {
    SmartIF<IAlgManager> iam ( loc ) ;
    if ( !iam  ) 
    { LoKi::Report::Error 
        ( "AlgFunctors::getAlgManager: Unable to locate Algorithm Manager" ) ; }
    return iam ;
  }
  // ==========================================================================
  // get the algorithm manager
  SmartIF<IAlgManager> getAlgManager 
  ( const LoKi::ILoKiSvc* loki )
  {
    if ( 0 == loki ) 
    { loki = LoKi::Services::instance().lokiSvc () ; }
    ISvcLocator* loc = 0  ;
    if ( 0 != loki ) { loc = loki->svcLoc() ; }
    if ( 0 == loc  ) { loc = Gaudi::svcLocator() ; }    
    if ( 0 == loc  )
    { LoKi::Report::Error 
        ( "AlgFunctors::getAlgManager: Unable to locate Service Locator" ) ; }
    return getAlgManager ( loc ) ;
  }
  // =========================================================================
  // get the algorithm 
  // =========================================================================
  LoKi::Interface<IAlgorithm> getAlgorithm 
  ( const std::string&    name , 
    SmartIF<IAlgManager>  iam  )
  {
    if ( !iam ) 
    {
      LoKi::Report::Error 
        ( "AlgFunctors::getAlgorithm: IAlgManager points to NULL" ) ; 
      return LoKi::Interface<IAlgorithm>()  ; 
    }
    IAlgorithm* _a = 0 ;
    StatusCode sc = iam->getAlgorithm ( name , _a ) ;
    if ( sc.isFailure() )
    { 
      LoKi::Report::Error 
        ( "AlgFunctors::getAlgorithm: Unable to locate Algorithm '" + name + "'" ) ; 
      return LoKi::Interface<IAlgorithm>()  ; 
    }
    if ( 0 == _a ) 
    {
      LoKi::Report::Error 
        ( "AlgFunctors::getAlgorithm: Algorithm '" + name + "' points to NULL" ) ; 
      return LoKi::Interface<IAlgorithm>()  ; 
    }
    return LoKi::Interface<IAlgorithm>( _a )  ; 
  }
  // ===========================================================================
  // get the algorithm 
  LoKi::Interface<IAlgorithm> getAlgorithm 
  ( const std::string&    name , 
    const LoKi::ILoKiSvc* loki )  
  {
    SmartIF<IAlgManager> iam = getAlgManager  ( loki ) ;
    if ( !iam ) { return 0 ; }  
    return getAlgorithm ( name , iam ) ;
  }
  // ==========================================================================
  // filter passed ?
  struct FilterPassed : public std::unary_function<const IAlgorithm*,bool>
  {
    inline bool operator() ( const IAlgorithm* ia ) const
    {
      if ( 0 == ia ) { return false ; }  // RETURN 
      const Algorithm* a = dynamic_cast<const Algorithm*> ( ia ) ;
      if ( 0 == a  ) { return false ; }  // RETURN 
      return a -> filterPassed () ;
    }
  } ;
  // ========================================================================
  // is enabled ?
  struct IsEnabled : public std::unary_function<const IAlgorithm*,bool>
  {
    inline bool operator() ( const IAlgorithm* ia ) const
    {
      if ( 0 == ia ) { return false ; }  // RETURN 
      const Algorithm* a = dynamic_cast<const Algorithm*> ( ia ) ;
      if ( 0 == a  ) { return false ; }  // RETURN 
      return a -> isEnabled () ;
    }
  } ;
  // ==========================================================================
  // is executed ?
  struct IsExecuted : public std::unary_function<const IAlgorithm*,bool>
  {
    inline bool operator() ( const IAlgorithm* ia ) const
    {
      if ( 0 == ia ) { return false ; }  // RETURN 
      return ia -> isExecuted () ;
    }
  } ;
  // ==========================================================================
}
// ============================================================================
// get the algorithm 
// ============================================================================
const LoKi::Interface<IAlgorithm>& 
LoKi::Algorithms::Passed::getAlgorithm 
( const std::string& name ) const  // get the algorithm 
{
  const IAlgorithm* a = ::getAlgorithm ( name , lokiSvc () ) ;
  Assert ( 0 != a , "Unable to locate algorithm '" + name + "'" ) ;
  m_algorithm = a ;
  return algorithm () ;
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::Passed::Passed 
( const std::string& name ) 
  : LoKi::BasicFunctors<void>::Predicate () 
  , m_name       ( name ) 
  , m_algorithm  ( s_ALGORITHM ) 
{
  this->getAlgorithm ( algName() ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::Passed::result_type 
LoKi::Algorithms::Passed::operator() () const 
{
  //
  if ( !algorithm().validPointer() ) { this->getAlgorithm ( algName() ) ; }
  //
  FilterPassed fun ;
  return fun ( algorithm() ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::Passed::fillStream( std::ostream& s ) const 
{ return s << "ALG_PASSED(" << Gaudi::Utils::toString( algName() ) << ")" ; }
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::Enabled::Enabled ( const std::string& name ) : Passed ( name ){}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::Enabled::result_type 
LoKi::Algorithms::Enabled::operator() () const 
{
  //
  if ( !algorithm().validPointer() ) { this->getAlgorithm ( algName() ) ; }
  //
  IsEnabled fun ;
  return fun ( algorithm() ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::Enabled::fillStream( std::ostream& s ) const 
{ return s << "ALG_ENABLED(" << Gaudi::Utils::toString( algName() ) << ")" ; }

// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::Executed::Executed 
( const std::string& name ) : Passed ( name ){}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::Executed::result_type 
LoKi::Algorithms::Executed::operator() () const 
{
  //
  if ( !algorithm().validPointer() ) { this->getAlgorithm ( algName() ) ; }
  // 
  IsExecuted fun ;
  return fun ( algorithm() ) ;
  //
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::Executed::fillStream( std::ostream& s ) const 
{ return s << "ALG_EXECUTED(" << Gaudi::Utils::toString( algName() ) << ")" ; }

// ============================================================================
// get the algorithm 
// ============================================================================
LoKi::Interface<IAlgorithm>
LoKi::Algorithms::AnyPassed::getAlgorithm 
( const std::string& name ) const  // get the algorithm 
{
  LoKi::Interface<IAlgorithm> a = ::getAlgorithm ( name , lokiSvc () ) ;
  Assert ( a.validPointer() , "Unable to locate algorithm '" + name + "'" ) ;
  return a ;
}
// ============================================================================
// get the algorithms
// ============================================================================
void LoKi::Algorithms::AnyPassed::getAlgorithms () const  // get the algorithm 
{
  for ( std::vector<std::string>::const_iterator iname = 
          m_names.begin() ; m_names.end() != iname ; ++iname ) 
  {
    const IAlgorithm* a = this->getAlgorithm ( *iname ) ;
    m_algorithms.push_back ( a ) ;
  }
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::string& name1 ,
  const std::string& name2 )
  : LoKi::BasicFunctors<void>::Predicate () 
  , m_names      () 
  , m_algorithms () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  //
  getAlgorithms() ;
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::string& name1 ,
  const std::string& name2 ,
  const std::string& name3 )
  : LoKi::BasicFunctors<void>::Predicate () 
  , m_names      () 
  , m_algorithms () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  m_names.push_back ( name3 ) ;
  //
  getAlgorithms() ;
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::string& name1 ,
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 )
  : LoKi::BasicFunctors<void>::Predicate () 
  , m_names      () 
  , m_algorithms () 
{
  m_names.push_back ( name1 ) ;
  m_names.push_back ( name2 ) ;
  m_names.push_back ( name3 ) ;
  m_names.push_back ( name4 ) ;
  //
  getAlgorithms() ;
}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::AnyPassed::AnyPassed 
( const std::vector<std::string>& names ) 
  : LoKi::BasicFunctors<void>::Predicate () 
  , m_names     ( names ) 
  , m_algorithms ( ) 
{
  getAlgorithms() ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AnyPassed::result_type 
LoKi::Algorithms::AnyPassed::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return end() != std::find_if ( begin () , end   () , FilterPassed() ) ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Algorithms::AnyPassed::print 
( const std::string& name , std::ostream& s ) const 
{ 
  if      ( 4 == algNames().size() ) 
  {
    return s << name << "(" 
             << Gaudi::Utils::toString ( algName( 0 ) ) << ","  
             << Gaudi::Utils::toString ( algName( 1 ) ) << "," 
             << Gaudi::Utils::toString ( algName( 2 ) ) << "," 
             << Gaudi::Utils::toString ( algName( 3 ) ) << ")" ;   // RETURN 
  }
  else if ( 3 == algNames().size() ) 
  {
    return s << name << "(" 
             << Gaudi::Utils::toString ( algName( 0 ) ) << ","  
             << Gaudi::Utils::toString ( algName( 1 ) ) << "," 
             << Gaudi::Utils::toString ( algName( 2 ) ) << ")" ;   // RETURN 
  }
  else if ( 2 == algNames().size() ) 
  {
    return s << name << "(" 
             << Gaudi::Utils::toString ( algName( 0 ) ) << ","  
             << Gaudi::Utils::toString ( algName( 1 ) ) << ")" ;   // RETURN 
  }
  return s << name << "(" << Gaudi::Utils::toString( algNames() ) << ")" ; 
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AllPassed::result_type 
LoKi::Algorithms::AllPassed::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return 
    !empty()  && 
    end() == std::find_if ( begin () , end () , std::not1 ( FilterPassed() ) ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AnyEnabled::result_type 
LoKi::Algorithms::AnyEnabled::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return end() != std::find_if ( begin () , end () , IsEnabled () ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AllEnabled::result_type 
LoKi::Algorithms::AllEnabled::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return 
    !empty()  && 
    end() == std::find_if ( begin () , end () , std::not1( IsEnabled () ) ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AnyExecuted::result_type 
LoKi::Algorithms::AnyExecuted::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return end () != std::find_if ( begin () , end () , IsExecuted () ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::AllExecuted::result_type 
LoKi::Algorithms::AllExecuted::operator() () const 
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return 
    !empty()  && 
    end() == std::find_if ( begin () , end () , std::not1 ( IsExecuted () ) ) ;
}
// ============================================================================


// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::string& name1 , 
  const std::string& name2 ) 
  : LoKi::BasicFunctors<void>::Function ()
  , m_fun ( name1 , name2 ) 
{}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ) 
  : LoKi::BasicFunctors<void>::Function ()
  , m_fun ( name1 , name2 , name3 ) {}
// ============================================================================
// constructor from the algorithm name  
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::string& name1 , 
  const std::string& name2 ,
  const std::string& name3 ,
  const std::string& name4 ) 
  : LoKi::BasicFunctors<void>::Function ()
  , m_fun ( name1 , name2 , name3 , name4 ) {}
// ============================================================================
// constructor from the algorithm name 
// ============================================================================
LoKi::Algorithms::NumPassed::NumPassed
( const std::vector<std::string>& name ) 
  : LoKi::BasicFunctors<void>::Function ()
  , m_fun  ( name ) {}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::NumPassed::result_type 
LoKi::Algorithms::NumPassed::operator() () const
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return std::count_if ( begin () , end() , FilterPassed() ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::NumEnabled::result_type 
LoKi::Algorithms::NumEnabled::operator() () const
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return std::count_if ( begin () , end() , IsEnabled() ) ;
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Algorithms::NumExecuted::result_type 
LoKi::Algorithms::NumExecuted::operator() () const
{
  //
  if ( algNames().size() != algorithms().size() ) { getAlgorithms() ; }
  //
  return std::count_if ( begin () , end() , IsExecuted () ) ;
}


// ============================================================================
// The END 
// ============================================================================
