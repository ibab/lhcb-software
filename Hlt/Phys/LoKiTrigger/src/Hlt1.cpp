// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrUpgrade.h"
#include "LoKi/apply.h"
#include "LoKi/select.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltCandidates.h"
// ============================================================================
// local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  Implementation file for functions from namespace LoKi::Hlt 
 *  @date 2008-11-10 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const Hlt::TSelection<Hlt::Candidate>* selection ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( selection )
  , m_selName   ()
  , m_cut ( LoKi::Constant<const Hlt::Candidate*,bool> ( true ) )
  , m_trivial ( true ) 
{
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Selection>* points to NULL!" ) ;
  m_selName = m_selection->id() ;
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const Hlt::Selection* sel ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( 0 ) 
  , m_selName   ( )
  , m_cut       ( LoKi::Constant<const Hlt::Candidate*,bool> ( true ) )
  , m_trivial   ( true ) 
{
  Assert ( 0 != sel , "Hlt::Selection* points to NULL!" ) ;
  m_selection = LoKi::Hlt1::Utils::cast<Hlt::Candidate> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::HltCandidate>* points to NULL!" ) ;
  m_selName = m_selection->id() ;
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const std::string&    selection ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( 0         )
  , m_selName   ( selection ) 
  , m_cut       ( LoKi::Constant<const Hlt::Candidate*,bool> ( true ) )
  , m_trivial   ( true ) 
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* points to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection*                  points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<Hlt::Candidate> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const std::string&                                           selection ,
  const LoKi::BasicFunctors<const Hlt::Candidate*>::Predicate& cut       )
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( 0         )
  , m_selName   ( selection ) 
  , m_cut       ( cut )
  , m_trivial   ( false ) 
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* point to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection                   points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<Hlt::Candidate> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const std::string&                                           selection ,
  const LoKi::BasicFunctors<const Hlt::Stage*>::Predicate&     cut       , 
  const int                                                    slot      ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( 0         )
  , m_selName   ( selection ) 
  , m_cut       ( LoKi::Candidates::StageCut ( cut , slot ) )
  , m_trivial   ( false ) 
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* points to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection*                  points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<Hlt::Candidate> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const std::string&                                           selection  ,
  const LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate& cut , 
  const int                                                    slot       ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( 0         )
  , m_selName   ( selection ) 
  , m_cut       ( LoKi::Candidates::SlotCut ( cut , slot ) )
  , m_trivial   ( false ) 
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* points to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection*                  points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<Hlt::Candidate> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const std::string&                                           selection  ,
  const LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate& cut , 
  const int                                                    slot       ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( 0         )
  , m_selName   ( selection ) 
  , m_cut       ( LoKi::Candidates::SlotCut ( cut , slot ) )
  , m_trivial   ( false ) 
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* points to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection*                  points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<Hlt::Candidate> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
}
// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Selection::Selection 
( const std::string&                                           selection  ,
  const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    cut        , 
  const int                                                    slot       ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_selection ( 0         )
  , m_selName   ( selection ) 
  , m_cut       ( LoKi::Candidates::SlotCut ( cut , slot ) )
  , m_trivial   ( false ) 
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* points to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection*                  points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<Hlt::Candidate> ( sel ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::Selection::result_type
LoKi::Hlt1::Selection::operator() () const 
{
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
  //
  // trivial cut ?
  if ( m_trivial ) 
  { return result_type ( m_selection->begin () , m_selection->end   () ) ; }
  //
  result_type result_;
  result_.reserve ( m_selection->size() ) ;
  //
  LoKi::select ( m_selection -> begin ()        , 
                 m_selection -> end   ()        , 
                 std::back_inserter ( result_ ) , m_cut ) ;
  //
  return result_;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::Selection::fillStream ( std::ostream& s ) const 
{ 
  s << "TC_SELECTION('" << selName() << "'" ;
  if ( !m_trivial ) { s << "," << m_cut ; }
  return s << ")" ;
}
// ============================================================================


// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::Sink::Sink
( const std::string&    selection ) 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* point to NULL" ) ;
  // declare the selection  
  m_selection = unit->declareOutput<Hlt::Candidate> ( selName(), *this ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<Hlt::Candidate>* points to NULL!" ) ;  
}
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::Sink::result_type
LoKi::Hlt1::Sink::operator() 
  ( LoKi::Hlt1::Sink::argument a ) const 
{
  Assert ( 0 != m_selection , 
           "Hlt::TSelection<Hlt::Candidate>* point to NULL!" ) ;  
  //
  for ( Hlt::Candidate::ConstVector::const_iterator ia = a.begin() ; 
        a.end() != ia ; ++ia ) 
  {
    const Hlt::Candidate* t = *ia ;
    if ( 0 == t ) { continue ; }
    m_selection->push_back ( t ) ;
  }
  //
  return a ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::Sink::fillStream ( std::ostream& s ) const 
{ return s << "TC_SINK('" << selName() << "')" ; }
// ============================================================================




// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrSelection::TrSelection 
( const Hlt::TSelection<LHCb::Track>* selection ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Source () 
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
  : LoKi::BasicFunctors<const LHCb::Track*>::Source () 
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
( const std::string&    selection ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Source () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* point to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection                points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<LHCb::Track> ( sel ) ;
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
{ return s << "TrSELECTION('" << selName() << "')" ; }
// ============================================================================



// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::TrRegister::TrRegister 
( const std::string&    selection ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Pipe () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* point to NULL" ) ;
  // declare the selection  
  m_selection = unit->declareOutput<LHCb::Track> ( selName(), *this ) ;
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
  for ( LHCb::Track::ConstVector::const_iterator ia = a.begin() ; 
        a.end() != ia ; ++ia ) 
  {
    const LHCb::Track* t = *ia ;
    if ( 0 == t ) { continue ; }
    m_selection->push_back ( t ) ;
  }
  //
  return a ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::TrRegister::fillStream ( std::ostream& s ) const 
{ return s << "TrREGISTER('" << selName() << "')" ; }
// ============================================================================



// ==========================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvSelection::RvSelection 
( const Hlt::TSelection<LHCb::RecVertex>* selection ) 
  : LoKi::BasicFunctors<const LHCb::RecVertex*>::Source () 
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
  : LoKi::BasicFunctors<const LHCb::RecVertex*>::Source () 
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
( const std::string&    selection ) 
  : LoKi::BasicFunctors<const LHCb::RecVertex*>::Source () 
  , m_selection ( 0 )
  , m_selName ( selection )
{
  // get selection form the Hlt Unit/Hlt Data service 
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* point to NULL" ) ;
  const Hlt::Selection* sel = unit->declareInput ( selection , *this ) ;
  Assert ( 0 != sel         , "Hlt::Selection                points to NULL!" ) ;  
  m_selection = LoKi::Hlt1::Utils::cast<LHCb::RecVertex> ( sel ) ;
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
{ return s << "RvSELECTION('" << selName() << "')" ; }
// ============================================================================


// ============================================================================
// constructor from the selection 
// ============================================================================
LoKi::Hlt1::RvRegister::RvRegister 
( const std::string&    selection ) 
  : LoKi::BasicFunctors<const LHCb::RecVertex*>::Pipe () 
  , m_selection ( 0 )
  , m_selName ( selection ) 
{
  SmartIF<Hlt::IUnit> unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!unit) , "Hlt::IUnit* point to NULL" ) ;
  // declare the selection  
  m_selection = unit->declareOutput<LHCb::RecVertex> ( selName() , *this ) ;
  Assert ( 0 != m_selection , "Hlt::TSelection<LHCb::RecVertex>* points to NULL!" ) ;  
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
{ return s << "RvREGISTER('" << selName() << "')" ; }
// ============================================================================



// ============================================================================
// constructor from the key and cuts 
// ============================================================================
LoKi::Hlt1::TrTESInput::TrTESInput
( const std::string&         key  , 
  const LoKi::Types::TrCuts& cuts ) 
  : LoKi::BasicFunctors<const LHCb::Track*>::Source () 
  , m_unit (      ) 
  , m_key  ( key  ) 
  , m_cut  ( cuts ) 
{
  // get the unit :
  SmartIF<Hlt::IUnit> _unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!_unit) , "Hlt::IUnit* points to NULL" ) ;
  m_unit = _unit ;
  // register TESInput input location 
  StatusCode sc = m_unit->registerTESInput ( m_key , *this ) ;
  Assert ( sc.isSuccess() , "Unable to registerTESInputInput" ) ;
  //
}
// ============================================================================
// constructor from the key and cuts 
// ============================================================================
LoKi::Hlt1::TrTESInput::TrTESInput
( const LoKi::Types::TrCuts& cuts ,
  const std::string&         key  )
  : LoKi::BasicFunctors<const LHCb::Track*>::Source () 
  , m_unit (      ) 
  , m_key  ( key  ) 
  , m_cut  ( cuts ) 
{
  // get the unit :
  SmartIF<Hlt::IUnit> _unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!_unit) , "Hlt::IUnit* points to NULL" ) ;
  m_unit = _unit ;
  // register TESInput input location 
  StatusCode sc = m_unit->registerTESInput ( m_key , *this ) ;
  Assert ( sc.isSuccess() , "Unable to registerTESInput" ) ;
  //
}
// ============================================================================
// constructor from the key and 
// ============================================================================
LoKi::Hlt1::TrTESInput::TrTESInput
( const std::string&         key  )
  : LoKi::BasicFunctors<const LHCb::Track*>::Source () 
  , m_unit (      ) 
  , m_key  ( key  ) 
  , m_cut  ( LoKi::Constant<const LHCb::Track*,bool>( true ) ) 
{
  // get the unit :
  SmartIF<Hlt::IUnit> _unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!_unit) , "Hlt::IUnit* points to NULL" ) ;
  m_unit = _unit ;
  // register TESInput input location 
  StatusCode sc = m_unit->registerTESInput ( m_key , *this ) ;
  Assert ( sc.isSuccess() , "Unable to registerTESInput" ) ;
  //
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Hlt1::TrTESInput::~TrTESInput () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::TrTESInput* LoKi::Hlt1::TrTESInput::clone() const 
{ return new TrTESInput ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::TrTESInput::result_type 
LoKi::Hlt1::TrTESInput::operator() () const 
{
  // get the data from unit 
  typedef LHCb::Track::Container TRACKS ;
  const TRACKS* tracks = m_unit -> tesData<TRACKS>( *this , m_key ) ;
  Assert ( 0 != tracks , "Unable to pick up data from TESInput" ) ;
  result_type output ;
  output.reserve ( tracks->size() ) ; 
  /// select the tracks
  LoKi::apply_filter  
    ( tracks -> begin () , 
      tracks -> end   () , 
      m_cut.func ()      , 
      std::back_inserter ( output ) ) ;
  // 
  return output ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::TrTESInput::fillStream ( std::ostream& s ) const 
{ return s << "TrINPUT('" << key() << "," << cuts() << "')" ; }
// ============================================================================


// ============================================================================
// constructor from the key and cuts 
// ============================================================================
LoKi::Hlt1::RvTESInput::RvTESInput
( const std::string&         key  , 
  const LoKi::Types::RVCuts& cuts ) 
  : LoKi::BasicFunctors<const LHCb::RecVertex*>::Source () 
  , m_unit (      ) 
  , m_key  ( key  ) 
  , m_cut  ( cuts ) 
{
  // get the unit :
  SmartIF<Hlt::IUnit> _unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!_unit) , "Hlt::IUnit* points to NULL" ) ;
  m_unit = _unit ;
  // register TESInput input location 
  StatusCode sc = m_unit->registerTESInput ( m_key , *this ) ;
  Assert ( sc.isSuccess() , "Unable to registerTESInputInput" ) ;
  //
}
// ============================================================================
// constructor from the key and cuts 
// ============================================================================
LoKi::Hlt1::RvTESInput::RvTESInput
( const LoKi::Types::RVCuts& cuts ,
  const std::string&         key  )
  : LoKi::BasicFunctors<const LHCb::RecVertex*>::Source () 
  , m_unit (      ) 
  , m_key  ( key  ) 
  , m_cut  ( cuts ) 
{
  // get the unit :
  SmartIF<Hlt::IUnit> _unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!_unit) , "Hlt::IUnit* points to NULL" ) ;
  m_unit = _unit ;
  // register TESInput input location 
  StatusCode sc = m_unit->registerTESInput ( m_key , *this ) ;
  Assert ( sc.isSuccess() , "Unable to registerTESInputInput" ) ;
  //
}
// ============================================================================
// constructor from the key and 
// ============================================================================
LoKi::Hlt1::RvTESInput::RvTESInput
( const std::string&         key  )
  : LoKi::BasicFunctors<const LHCb::RecVertex*>::Source () 
  , m_unit (      ) 
  , m_key  ( key  ) 
  , m_cut  ( LoKi::Constant<LHCb::RecVertex,bool>( true ) ) 
{
  // get the unit :
  SmartIF<Hlt::IUnit> _unit = LoKi::Hlt1::Utils::getUnit ( *this ) ;
  Assert ( !(!_unit) , "Hlt::IUnit* points to NULL" ) ;
  m_unit = _unit ;
  // register TESInput input location 
  StatusCode sc = m_unit->registerTESInput ( m_key , *this ) ;
  Assert ( sc.isSuccess() , "Unable to registerTESInputInput" ) ;
  //
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Hlt1::RvTESInput::~RvTESInput () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::RvTESInput* LoKi::Hlt1::RvTESInput::clone() const 
{ return new RvTESInput ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Hlt1::RvTESInput::result_type 
LoKi::Hlt1::RvTESInput::operator() () const 
{
  // get the data from unit 
  typedef LHCb::RecVertex::Container VERTICES ;
  const VERTICES* vertices = m_unit -> tesData<VERTICES>( *this , m_key ) ;
  Assert ( 0 !=   vertices , "Unable to pick up data from TESInput" ) ;
  result_type output ;
  output.reserve ( vertices->size() ) ; 
  /// select the tracks
  LoKi::apply_filter  
    ( vertices -> begin () , 
      vertices -> end   () , 
      m_cut.func ()      , 
      std::back_inserter ( output ) ) ;
  // 
  return output ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Hlt1::RvTESInput::fillStream ( std::ostream& s ) const 
{ return s << "RvINPUT('" << key() << "," << cuts() << "')" ; }

// ============================================================================
// The END 
// ============================================================================
