// $Id: TrackUse.cpp,v 1.3 2006-06-20 20:00:43 erodrigu Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/AlgTool.h"
// ============================================================================
#include "Event/Track.h"
#include "Event/TrackUse.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

// ============================================================================
/** @file 
 *  The implementation file for class : TrackUse
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date   2004-10-27
 */
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
TrackUse::TrackUse() 
  : m_check        ( true  ) // perform check ? 
  , m_skipClones   ( true  )    
  , m_skipInvalid  ( true  )  
  , m_skipBackward ( true  )
  // accepted fit status 
  , m_fitstatus () 
  // accepted type 
  , m_type   () 
  // rejected history
  , m_history () 
{
  m_fitstatus.push_back (  LHCb::Track::Fitted     ) ;
  //
  m_type.push_back   (  LHCb::Track::Long       ) ;
  m_type.push_back   (  LHCb::Track::Upstream   ) ;
  m_type.push_back   (  LHCb::Track::Downstream ) ;
  m_type.push_back   (  LHCb::Track::Ttrack     ) ;
  //
} ;
// ============================================================================
/// destructor
// ============================================================================
TrackUse::~TrackUse() {}
// ============================================================================
/// the basic method fofr delegation of properties 
// ============================================================================
template <class TYPE>
inline StatusCode 
TrackUse::i_declareProperties ( TYPE* object ) 
{
  if ( 0 == object ) { return StatusCode::FAILURE ; }
  // 
  object -> declareProperty ( "CheckTracks"      , m_check       ) ;
  //
  object -> declareProperty ( "SkipClones"       , m_skipClones  ) ;
  object -> declareProperty ( "SkipInvalid"      , m_skipInvalid ) ;
  object -> declareProperty ( "SkipBackward"     , m_skipBackward) ;  
  //
  object -> declareProperty ( "AcceptedType"     , m_type        ) ;
  object -> declareProperty ( "AcceptedFitStatus", m_fitstatus   ) ;
  object -> declareProperty ( "RejectedHistory"  , m_history     ) ;
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================
/// declare 'own' properties for the algorithm 
// ============================================================================
StatusCode TrackUse::declareProperties ( Algorithm* alg )
{ return i_declareProperties ( alg ) ; };
// ============================================================================
/// declare 'own' properties for the service 
// ============================================================================
StatusCode TrackUse::declareProperties ( Service*  svc ) 
{ return i_declareProperties ( svc ) ; };
// ============================================================================
/// declare 'own' properties for the tool 
// ============================================================================
StatusCode TrackUse::declareProperties ( AlgTool*  tool ) 
{ return i_declareProperties ( tool ) ; };
// ============================================================================
/// get the list of accepted status
// ============================================================================
size_t TrackUse::acceptedFitStatus  ( std::vector<LHCb::Track::FitStatus>& s ) const
{
  for ( Shorts::const_iterator iv = m_fitstatus.begin() ; 
        m_fitstatus.end() != iv ; ++iv ) 
  { s.push_back( (LHCb::Track::FitStatus) (*iv) ) ; }
  return m_fitstatus.size() ;
} ;  
// ============================================================================
/// get the list of accepted types  
// ============================================================================
size_t TrackUse::acceptedType    ( std::vector<LHCb::Track::Types>&   t ) const 
{
  for ( Shorts::const_iterator iv = m_type.begin() ; 
        m_type.end() != iv ; ++iv ) 
  { t.push_back( (LHCb::Track::Types) (*iv) ) ; }
  return m_type.size() ;
} ;  
// ============================================================================
/// get the list of rejected history
// ============================================================================
size_t TrackUse::rejectedHistory ( std::vector<LHCb::Track::History>& h ) const 
{
  for ( Shorts::const_iterator iv = m_history.begin() ; 
        m_history.end() != iv ; ++iv ) 
  { h.push_back( (LHCb::Track::History) (*iv) ) ; }
  return m_history.size() ;
} ;  
// ============================================================================
/// printout of the track into the stream 
// ============================================================================
MsgStream& TrackUse::print 
( MsgStream& stream , const LHCb::Track* track ) const 
{
  if ( !stream.isActive() ) { return stream                ; }   // RETURN 
  if ( 0 == track         ) { return stream << "<INVALID>" ; }   // RETURN
  // Key 
  if ( track->hasKey() ) { stream << "Key: " << track->key() << " " ; }
  // Flag 
  stream << "Flags: "<< track->flags() << " [";
  if ( track->checkFlag ( LHCb::Track::Invalid     ) ) 
  { stream << " '" << LHCb::Track::Invalid      << "' "; }
  if ( track->checkFlag ( LHCb::Track::Clone       ) ) 
  { stream << " '" << LHCb::Track::Clone        << "' "; }
  if ( track->checkFlag ( LHCb::Track::Backward    ) ) 
  { stream << " '" << LHCb::Track::Backward     << "' "; }
  if ( track->checkFlag ( LHCb::Track::IPSelected  ) ) 
  { stream << " '" << LHCb::Track::IPSelected   << "' "; }
  if ( track->checkFlag ( LHCb::Track::PIDSelected ) ) 
  { stream << " '" << LHCb::Track::PIDSelected  << "' "; }
  if ( track->checkFlag ( LHCb::Track::Used ) ) 
  { stream << " '" << LHCb::Track::Used  << "' "; }
  stream << "] ";
  // Type
  stream << "Type:"       << " '" << track->type       () << "' " ;
  // Status 
  stream << "FitStatus:"  << " '" << track->fitStatus  () << "' " ;
  // History
  stream << "History:"    << " '" << track->history    () << "' " ;
  // HistoryFit 
  stream << "FitHistory:" << " '" << track->fitHistory () << "' " ;
  //
  return stream ; 
}
// ============================================================================
/// printout of the track into the staream 
// ============================================================================
MsgStream& TrackUse::fillStream ( MsgStream& s ) const 
{
  s << " Check Tracks: " ;
  //
  if  ( check() ) {        s << "YES" ; } 
  else            { return s << "NO"  ; }                          // RETURN 
  //
  s << " Rejected flags: [ " ;
  if ( m_skipInvalid  ) { s << " '" << LHCb::Track::Invalid  << "' " ;}
  if ( m_skipClones   ) { s << " '" << LHCb::Track::Clone    << "' " ;}
  if ( m_skipBackward ) { s << " '" << LHCb::Track::Backward << "' " ;}
  s << "]" ;
  
  typedef Shorts::const_iterator iterator ;
  {
    s << " Accepted FitStatus: [" ;
    for ( iterator i = m_fitstatus.begin() ; m_fitstatus.end() != i ; ++i ) 
    { s << " '" << (LHCb::Track::FitStatus)(*i) << "' " ; }
    s << "];" ;
  }
  {
    s << " Accepted Types: [" ;
    for ( iterator i = m_type.begin() ; m_type.end() != i ; ++i ) 
    { s << " '" << (LHCb::Track::Types)(*i) << "' " ; }
    s << "];" ;
  }
  {
    s << " Rejected History: [" ;
    for ( iterator i = m_history.begin() ; m_history.end() != i ; ++i ) 
    { s << " '" << (LHCb::Track::History)(*i) << "' " ; }
    s << "];" ;
  }  
  return s ;
} ;
// ============================================================================
/// The END 
// ============================================================================

