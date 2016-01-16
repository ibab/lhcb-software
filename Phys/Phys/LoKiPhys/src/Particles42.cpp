// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKiUtils 
// ============================================================================
#include "LoKi/GetTools.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/Particles42.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-06-06
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// constructor from the lines and the tool 
// ============================================================================
LoKi::Particles::TisTosTobDec::TisTosTobDec
( const std::string&    lines  ,
  const ITriggerTisTos* tistos ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_lines  ( lines  ) 
  , m_tistos ( tistos ) 
{
  Assert ( !(!m_tistos) , "Invalid ITriggerTisTos tool!" );
  //
  if ( std::string::npos != lines         .find("Hlt1") &&
       std::string::npos != tistos->name().find("Hlt2") ) 
  { Error("Wrong TisTos tool is used!") ; }     
  if ( std::string::npos != lines         .find("Hlt2") &&
       std::string::npos != tistos->name().find("Hlt1") ) 
  { Error("Wrong TisTos tool is used!") ; }     
}
// ============================================================================
// MANDATORY : virtual destructor 
// ============================================================================
LoKi::Particles::TisTosTobDec::~TisTosTobDec ()
{
  if ( m_tistos && !gaudi() ) 
  {
    // Warning("ITriggerTisTos: manual reset") ;
    m_tistos.reset() ;
  }
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TisTosTobDec*
LoKi::Particles::TisTosTobDec::clone() const 
{ return new LoKi::Particles::TisTosTobDec ( *this ) ; }
// ============================================================================
// protected  constructor without tool 
// ============================================================================
LoKi::Particles::TisTosTobDec::TisTosTobDec
( const std::string&    lines )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_lines  ( lines  ) 
  , m_tistos () 
{}
// ============================================================================
// the major method 
// ============================================================================
ITisTos::TisTosTob 
LoKi::Particles::TisTosTobDec::tistos ( const LHCb::Particle* p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "Invalid LHCb::Particle, return ITisTos::TisTosTob()" ) ;
    return ITisTos::TisTosTob () ;
  }
  //
  if ( !m_tistos ) 
  {
    Error ( "Invalid ITriggerTisTos, return ITisTos::TisTosTob()" ) ;
    return ITisTos::TisTosTob () ;
  }
  //
  m_tistos -> setOfflineInput ( *p       ) ;
  m_tistos -> setTriggerInput ( m_lines  ) ;
  //
  return m_tistos -> tisTosTobTrigger () ;
}
// ============================================================================
// MANDATORY: the only one important method 
// ============================================================================
LoKi::Particles::TisTosTobDec::result_type 
LoKi::Particles::TisTosTobDec::operator()
  ( LoKi::Particles::TisTosTobDec::argument p ) const 
{
  // 
  ITisTos::TisTosTob r = tistos ( p ) ;
  //
  return r.value() ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::TisTosTobDec::fillStream
( std::ostream& s ) const
{ return s << " TISTOSTOBDEC('" << lines () << "')" ; }
// ============================================================================
// define/reset the tool
// ============================================================================
void LoKi::Particles::TisTosTobDec::setTool ( const ITriggerTisTos*  tool   ) 
{
  m_tistos = tool ;
  Assert ( !(!m_tistos) , "Invalid ITriggerTisTos tool!" );  
}
// ============================================================================


// ============================================================================
// constructor from the lines and the tool 
// ============================================================================
LoKi::Particles::TisTosTobbing::TisTosTobbing
( const std::string&    lines ,
  const std::string&    tool  ) 
  : LoKi::AuxFunBase ( std::tie ( lines , tool ) ) 
  , LoKi::Particles::TisTosTobDec::TisTosTobDec ( lines ) 
  , m_nick ( tool ) 
{
  const ITriggerTisTos* tt = LoKi::GetTools::triggerTisTos( *this , tool ) ;
  setTool ( tt ) ;
}
// ============================================================================
// MANDATORY : virtual destructor 
// ============================================================================
LoKi::Particles::TisTosTobbing::~TisTosTobbing () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TisTosTobbing*
LoKi::Particles::TisTosTobbing::clone() const 
{ return new LoKi::Particles::TisTosTobbing ( *this ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Particles::TisTosTobbing::fillStream
( std::ostream& s ) const
{ return s << " TISTOSTOB('" << lines() << ","  << toolName () << "')" ; }
// ============================================================================


// ============================================================================
// constructor from the lines and the tool 
// ============================================================================
LoKi::Particles::Tis::Tis
( const std::string&    lines ,
  const std::string&    tool  ) 
  : LoKi::AuxFunBase ( std::tie ( lines , tool ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_fun ( lines , tool ) 
{}
// ============================================================================
// MANDATORY : virtual destructor 
// ============================================================================
LoKi::Particles::Tis::~Tis () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::Tis*
LoKi::Particles::Tis::clone() const 
{ return new LoKi::Particles::Tis ( *this ) ; }
// ============================================================================
// MANDATORY: the only one important method 
// ============================================================================
LoKi::Particles::Tis::result_type 
LoKi::Particles::Tis::operator()
  ( LoKi::Particles::Tis::argument p ) const 
{
  // 
  ITisTos::TisTosTob r = tistos ( p ) ;
  //
  return r.tis() ;
}
// ============================================================================
std::ostream& LoKi::Particles::Tis::fillStream
( std::ostream& s ) const
{ return s << " TIS('" << lines() << ","  << toolName () << "')" ; }
// ============================================================================


// ============================================================================
// constructor from the lines and the tool 
// ============================================================================
LoKi::Particles::Tos::Tos
( const std::string&    lines ,
  const std::string&    tool  ) 
  : LoKi::AuxFunBase ( std::tie ( lines , tool ) ) 
  , LoKi::Particles::Tis ( lines , tool ) 
{}
// ============================================================================
// MANDATORY : virtual destructor 
// ============================================================================
LoKi::Particles::Tos::~Tos (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::Tos*
LoKi::Particles::Tos::clone() const 
{ return new LoKi::Particles::Tos ( *this ) ; }
// ============================================================================
// MANDATORY: the only one important method 
// ============================================================================
LoKi::Particles::Tos::result_type 
LoKi::Particles::Tos::operator()
  ( LoKi::Particles::Tos::argument p ) const 
{
  // 
  ITisTos::TisTosTob r = tistos ( p ) ;
  //
  return r.tos() ;
}
// ============================================================================
std::ostream& LoKi::Particles::Tos::fillStream
( std::ostream& s ) const
{ return s << " TOS('" << lines() << ","  << toolName () << "')" ; }
// ============================================================================

// ============================================================================
// constructor from the lines and the tool 
// ============================================================================
LoKi::Particles::Tps::Tps
( const std::string&    lines ,
  const std::string&    tool  ) 
  : LoKi::AuxFunBase ( std::tie ( lines , tool ) ) 
  , LoKi::Particles::Tos ( lines , tool ) 
{}
// ============================================================================
// MANDATORY : virtual destructor 
// ============================================================================
LoKi::Particles::Tps::~Tps (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::Tps*
LoKi::Particles::Tps::clone() const 
{ return new LoKi::Particles::Tps ( *this ) ; }
// ============================================================================
// MANDATORY: the only one important method 
// ============================================================================
LoKi::Particles::Tps::result_type 
LoKi::Particles::Tps::operator()
  ( LoKi::Particles::Tos::argument p ) const 
{
  // 
  ITisTos::TisTosTob r = tistos ( p ) ;
  //
  return r.tps() ;
}
// ============================================================================
std::ostream& LoKi::Particles::Tps::fillStream
( std::ostream& s ) const
{ return s << " TPS('" << lines() << ","  << toolName () << "')" ; }
// ============================================================================

// ============================================================================
// constructor from the lines and the tool 
// ============================================================================
LoKi::Particles::Tus::Tus
( const std::string&    lines ,
  const std::string&    tool  ) 
  : LoKi::AuxFunBase ( std::tie ( lines , tool ) ) 
  , LoKi::Particles::Tps ( lines , tool ) 
{}
// ============================================================================
// MANDATORY : virtual destructor 
// ============================================================================
LoKi::Particles::Tus::~Tus (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::Tus*
LoKi::Particles::Tus::clone() const 
{ return new LoKi::Particles::Tus ( *this ) ; }
// ============================================================================
// MANDATORY: the only one important method 
// ============================================================================
LoKi::Particles::Tus::result_type 
LoKi::Particles::Tus::operator()
  ( LoKi::Particles::Tus::argument p ) const 
{
  // 
  ITisTos::TisTosTob r = tistos ( p ) ;
  //
  return r.tus() ;
}
// ============================================================================
std::ostream& LoKi::Particles::Tus::fillStream
( std::ostream& s ) const
{ return s << " TUS('" << lines() << ","  << toolName () << "')" ; }
// ============================================================================

  
// ============================================================================
// The END 
// ============================================================================
