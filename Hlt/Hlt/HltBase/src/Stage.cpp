// $Id: Stage.cpp,v 1.7 2010-08-02 18:15:38 ibelyaev Exp $ 
// ============================================================================
// Include files 
// ============================================================================
// HltBase 
// ============================================================================
#include "Event/HltStage.h"
#include "Event/HltCandidate.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
/** @file 
 *  Implementation file for class Htl::Stage 
 *  @see Htl::Stage 
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @varCLID_Stage 
   *  Class ID definition
   */
  const CLID CLID_Stage = 7562;
  // ==========================================================================
}
// ============================================================================
// Retrieve pointer to class definition structure (static)
// ============================================================================
const CLID& Hlt::Stage::classID() { return CLID_Stage ; }
// ============================================================================
// Default Destructor
// ============================================================================
Hlt::Stage::~Stage() {}
// ============================================================================
Hlt::Stage::Lock::Lock (  Stage* stage , const INamedInterface* locker) 
{ 
  if (0 == stage || 0 == locker) 
  { 
    throw GaudiException("Stage or locker is null","Stage::Lock::Lock",
                         StatusCode::FAILURE);
  }
  m_stage = stage;
  m_locker = const_cast<INamedInterface*>(locker);
  stage->_lock(locker) ;
}
// ============================================================================
Hlt::Stage::Lock::~Lock () { m_stage->_unlock (m_locker); }
// ============================================================================
const Hlt::Stage::History& 
Hlt::Stage::Lock::addToHistory ( const std::string& entry ) 
{
  History& _history = m_stage->m_history ; 
  _history.push_back ( entry ) ;
  //
  return m_stage->history() ;
}
// ============================================================================
const Hlt::Stage::History& 
Hlt::Stage::Lock::addToHistory 
( const Hlt::Stage::History& history ) 
{
  History& _history = m_stage->m_history ; 
  _history.insert( _history.end() , history.begin() , history.end() );
  //
  return m_stage->history() ;
}
// ============================================================================
void Hlt::Stage::_checkLock() const 
{
  if (!locked()) 
  {
    throw GaudiException("Not locked","Stage::_checkLock",StatusCode::FAILURE);
  }
}
// ============================================================================
void Hlt::Stage::SetAllToNull() 
{
  _checkLock();
  m_track = 0;
  m_rec_vertex = 0;
  m_l0_calo_candidate = 0;
  m_l0_muon_candidate = 0;
  m_multitrack = 0;
  m_l0_dimuon_candidate = 0;
  m_candidate = 0;
  m_stage = 0;
}
// ============================================================================
const ContainedObject* Hlt::Stage::_get() const 
{
  if (is<LHCb::Track> ()) return get<LHCb::Track>();
  if (is<LHCb::RecVertex> ()) return get<LHCb::RecVertex>();
  if (is<LHCb::L0CaloCandidate> ()) return get<LHCb::L0CaloCandidate>();
  if (is<LHCb::L0MuonCandidate> ()) return  get<LHCb::L0MuonCandidate>();
  if (is<Hlt::MultiTrack> ()) return  get<Hlt::MultiTrack>();
  if (is<Hlt::L0DiMuonCandidate> ()) return get<Hlt::L0DiMuonCandidate>();
  if (is<Hlt::Candidate> ()) return get<Hlt::Candidate>();
  if (is<Hlt::Stage> ()) return get<Hlt::Stage>();
  return 0;
}
// ============================================================================
void Hlt::Stage::_lock ( const INamedInterface* locker ) 
{
  if (locked()) 
  { throw GaudiException("Stage already locked","Stage::_lock", StatusCode::FAILURE); }
  //
  m_locker = const_cast<INamedInterface*> (locker) ;
  // m_history.push_back (locker->name());
}
// ============================================================================
void Hlt::Stage::_unlock (const INamedInterface* locker) 
{
  //
  if (!locked()) 
  { throw GaudiException ( "Stage  not locked","Stage::_unlock", StatusCode::FAILURE); }
  //
  if (locker != locked()) 
  { throw GaudiException ( "Wrong locker","Stage::_unlock", StatusCode::FAILURE); }
  //
  m_history.push_back ( locker->name() );
  //
  m_locker = 0 ;
}
// ============================================================================
std::ostream& Hlt::Stage::fillStream(std::ostream& s) const 
{
  if (is<LHCb::Track> ())
    s << "Track/"             << get<LHCb::Track>            () ;
  else  if (is<LHCb::RecVertex> ())
    s << "RecVertex/"         << get<LHCb::RecVertex>        () ;
  else if (is<LHCb::L0CaloCandidate> ())
    s << "L0CaloCandidate/"   << get<LHCb::L0CaloCandidate>  () ;
  else if (is<LHCb::L0MuonCandidate> ())
    s << "L0MuonCandidate/"   << get<LHCb::L0MuonCandidate>  () ;
  else if (is<Hlt::MultiTrack> ())
    s << "MultiTrack/"        << get<Hlt::MultiTrack>        () ;
  else if (is<Hlt::L0DiMuonCandidate> ())
    s << "L0DiMuonCandidate/" << get<Hlt::L0DiMuonCandidate> () ;
  else if (is<Hlt::Candidate> ())
    s << "Candidate/"         << (void*) get<Hlt::Candidate> () ;
  else if (is<Hlt::Stage> ())
    s << "Stage/"             << (void*) get<Hlt::Stage>     () ;
  else s << "NULL?";
  //
  s << ", " << m_cache;
  //
  s << ", History: " ;
  Gaudi::Utils::toStream ( m_history , s  ) ;
  //
  return s;
}
// ============================================================================
// set own candidate 
// ============================================================================
void  Hlt::Stage::setOwner ( const Hlt::Candidate* c ) 
{
  //
  if ( 0 == c ) 
  {
    throw GaudiException ( "Invalid Hlt::Candidate" ,
                           "Stage::setOwner"        ,
                           StatusCode::FAILURE      ) ;
  }
  //
  if ( 0 != m_owner && c != m_owner ) 
  {
    throw GaudiException ( "Owner is already set"   ,
                           "Stage::setOwner"        ,
                           StatusCode::FAILURE      ) ;
  } 
  //
  m_owner = c ;
}
// ============================================================================
Hlt::Stage::Type Hlt::Stage::stageType() const 
{
  if      ( is<LHCb::L0MuonCandidate>  () ) { return Hlt::Stage::L0Muon        ; }
  else if ( is<Hlt::L0DiMuonCandidate> () ) { return Hlt::Stage::L0DiMuon      ; }
  else if ( is<LHCb::L0CaloCandidate>  () ) { return Hlt::Stage::L0Calo        ; }
  else if ( is<LHCb::Track>            () ) { return Hlt::Stage::HltTrack      ; }
  else if ( is<LHCb::RecVertex>        () ) { return Hlt::Stage::HltVertex     ; }
  else if ( is<Hlt::MultiTrack>        () ) { return Hlt::Stage::HltMultiTrack ; }
  else if ( is<Hlt::Stage>             () ) { return Hlt::Stage::HltStage      ; }
  else if ( is<Hlt::Candidate>         () ) { return Hlt::Stage::HltCandidate  ; }
  //
  return Hlt::Stage::Unknown ;
}
// ============================================================================
// printout 
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const Hlt::Stage* c , std::ostream& s ) 
{
  if ( 0 == c ){ return s << "<NULL>" ; }
  return c->fillStream ( s ) ;
}
// ============================================================================
// The END 
// ============================================================================
