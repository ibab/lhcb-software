// $Id: Hlt1.cpp,v 1.1 2008-11-13 13:14:42 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrUpgrade.h"
// ============================================================================
// local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  Implementation file for functions fro namespace LoKi::Hlt 
 *  @date 2008-11-10 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrSelection::TrSelection 
( const Hlt::TSelection<LHCb::Track>* selection ) 
  : LoKi::BasicFunctors<LHCb::Track*>::Source () 
  , m_selection ( selection )
{
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;
  m_selName = m_selection->id() ;
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrSelection::TrSelection 
( const Hlt::Selection* sel ) 
  : LoKi::BasicFunctors<LHCb::Track*>::Source () 
  , m_selection ( 0 ) 
{
  Assert ( 0 != sel , "Hlt::Selection* point to NULL!" ) ;
  m_selection = LoKi::Hlt1::Utils::cast<LHCb::Track> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;
  m_selName = m_selection->id() ;
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrSelection::TrSelection 
( const stringKey&    selection ) 
  : LoKi::BasicFunctors<LHCb::Track*>::Source () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  m_selection = 
    LoKi::Hlt1::Utils::getTSelection<LHCb::Track> ( selName() , *this ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrSelection::TrSelection 
( const std::string&    selection ) 
  : LoKi::BasicFunctors<LHCb::Track*>::Source () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  m_selection = 
    LoKi::Hlt1::Utils::getTSelection<LHCb::Track> ( selName() , *this ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::TrSelection::result_type
LoKi::Hlt1::TrSelection::operator() () const 
{
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;  
  return result_type ( m_selection->begin () , m_selection->end() ) ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::TrSelection::fillStream ( std::ostream& s ) const 
{ return s << "TrSELECTION('" << selName().str() << "')" ; }
// ============================================================================



// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrRegister::TrRegister 
( const stringKey&    selection ) 
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  SmartIF<LoKi::IHltUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "LoKi::IHltUnit* point to NULL" ) ;
  // delcare selection  
  m_selection = unit->declareSelection<LHCb::Track> ( selName() ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrRegister::TrRegister 
( const std::string&    selection ) 
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  SmartIF<LoKi::IHltUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "LoKi::IHltUnit* point to NULL" ) ;
  // delcare selection  
  m_selection = unit->declareSelection<LHCb::Track> ( selName() ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::TrRegister::result_type
LoKi::Hlt1::TrRegister::operator() 
  ( LoKi::Hlt1::TrRegister::argument a ) const 
{
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* point to NULL!" ) ;  
  //
  m_selection->insert ( m_selection->end() , a.begin() , a.end() ) ;
  //
  return a ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::TrRegister::fillStream ( std::ostream& s ) const 
{ return s << "TrREGISTER('" << selName().str() << "')" ; }
// ============================================================================



// ==========================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvSelection::RvSelection 
( const Hlt::TSelection<LHCb::RecVertex>* selection ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_selection ( selection )
{
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::RecVertex>* points to NULL!" ) ;
  m_selName = m_selection->id() ;
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvSelection::RvSelection 
( const Hlt::Selection* sel ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_selection ( 0 ) 
{
  Assert ( 0 != sel , "Hlt::Selection* point to NULL!" ) ;
  m_selection = LoKi::Hlt1::Utils::cast<LHCb::RecVertex> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::RecVertex>* points to NULL!" ) ;
  m_selName = m_selection->id() ;
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvSelection::RvSelection 
( const stringKey&    selection ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  m_selection = LoKi::Hlt1::Utils::getTSelection<LHCb::RecVertex> ( selName() , *this ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::Track>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvSelection::RvSelection 
( const std::string&    selection ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Source () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  m_selection = LoKi::Hlt1::Utils::getTSelection<LHCb::RecVertex> ( selName() , *this ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::RecVertex>* points to NULL!" ) ;  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::RvSelection::result_type
LoKi::Hlt1::RvSelection::operator() () const 
{
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::RecVertex>* points to NULL!" ) ;  
  return result_type ( m_selection->begin () , m_selection->end() ) ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::RvSelection::fillStream ( std::ostream& s ) const 
{ return s << "RvSELECTION('" << selName().str() << "')" ; }
// ============================================================================


// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvRegister::RvRegister 
( const stringKey&    selection ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Pipe () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  SmartIF<LoKi::IHltUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "LoKi::IHltUnit* point to NULL" ) ;
  // delcare selection  
  m_selection = unit->declareSelection<LHCb::RecVertex> ( selName() ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::RecVertex>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvRegister::RvRegister 
( const std::string&    selection ) 
  : LoKi::BasicFunctors<LHCb::RecVertex*>::Pipe () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  SmartIF<LoKi::IHltUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "LoKi::IHltUnit* point to NULL" ) ;
  // delcare selection  
  m_selection = unit->declareSelection<LHCb::RecVertex> ( selName() ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::ReVertex>* points to NULL!" ) ;  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::RvRegister::result_type
LoKi::Hlt1::RvRegister::operator() 
  ( LoKi::Hlt1::RvRegister::argument a ) const 
{
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::RecVertex>* point to NULL!" ) ;  
  //
  m_selection->insert ( m_selection->end() , a.begin() , a.end() ) ;
  //
  return a ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::RvRegister::fillStream ( std::ostream& s ) const 
{ return s << "RvREGISTER('" << selName().str() << "')" ; }
// ============================================================================





// ============================================================================
// The END 
// ============================================================================
