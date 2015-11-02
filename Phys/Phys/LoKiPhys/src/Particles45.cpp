// Include files 

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "Kernel/RelatedInfoNamed.h"

// local
#include "LoKi/Particles45.h"
#include "LoKi/ILoKiSvc.h"


//-----------------------------------------------------------------------------
// Implementation file for class : Particles45
//
// 2014-07-02 : Vanya Belyaev
//-----------------------------------------------------------------------------
/// constructor from 
LoKi::Particles::RelatedInfo::RelatedInfo 
( const std::string& location , 
  const short        index    , 
  const double       bad      ) 
  : LoKi::AuxFunBase( std::tie ( location , index , bad ) )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_location ( location ) 
  , m_index    ( index    ) 
  , m_bad      ( bad      ) 
  , m_table    ( 0        ) 
{}
//
LoKi::Particles::RelatedInfo::RelatedInfo 
( const std::string& location , 
  const std::string& variable , 
  const double       bad      ) 
  : LoKi::AuxFunBase( std::tie ( location , variable , bad ) )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_location ( location ) 
  , m_bad      ( bad      ) 
  , m_table    ( 0        ) 
{
  short index = RelatedInfoNamed::indexByName( variable ); 
  if ( index == RelatedInfoNamed::UNKNOWN ) {
    Warning("RelatedInfo variable " + variable + " unknown"); 
  }
  m_index = index;
}
//
LoKi::Particles::RelatedInfo::RelatedInfo 
( const LoKi::Particles::RelatedInfo& right ) 
  : LoKi::AuxFunBase ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function( right )
  , m_location ( right.m_location ) 
  , m_index    ( right.m_index    ) 
  , m_bad      ( right.m_bad      ) 
  , m_table    ( 0        ) 
{}
//
LoKi::Particles::RelatedInfo*
LoKi::Particles::RelatedInfo::clone() const 
{ return new LoKi::Particles::RelatedInfo(*this) ; }
//
LoKi::Particles::RelatedInfo::~RelatedInfo(){}
// MANDATORY: the only one essential method 
LoKi::Particles::RelatedInfo::result_type 
LoKi::Particles::RelatedInfo::operator () 
  ( LoKi::Particles::RelatedInfo::argument p ) const 
{
  if ( !p ) 
  {
    Error("Invalid particle, return ...") ;
    return -1000 ;
  }
  //
  if ( 0 == m_table  || !sameEvent() ) 
  {
    SmartIF<IDataProviderSvc> ds ( lokiSvc().getObject() ) ;
    SmartDataPtr<IMAP> data ( ds , m_location ) ;
    if ( !data ) 
    {
      Warning( "No table at location " + m_location ) ;
      return -2000 ;
    } 
    m_table = data ;
    setEvent () ;  
  }
  //
  IMAP::Range r = m_table->relations( p ) ;
  if ( r.empty() )  
  {
    Warning ( "No entry for particle" ) ;
    return m_bad ; 
  }
  if ( 1 != r.size() )  
  {
    Warning ( "No entry for particle" ) ;
    return m_bad ; 
  }
  const LHCb::RelatedInfoMap& m = r[0].to() ;
  //
  return m.info ( m_index , -2000 ) ;
}
/// OPTIONAL: the specific printout
std::ostream& LoKi::Particles::RelatedInfo::fillStream( std::ostream& s ) const 
{
  return 
    s << "RELINFO('"
      << m_location << "',"
      << m_index    << "," << m_bad << ")" ;
}



