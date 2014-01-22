// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <ctime>
#include <fstream>
#include <set>
#include <algorithm>
// ============================================================================
// Local
// ============================================================================
#include "Service.h"
// ============================================================================
/** @file
 *  Implementation file for class Hlt::Service
 *  The methods from Hlt::IInspector are implemented here
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-18
 */
// ============================================================================
/** get the producer for the given selection
 *  @param selection the selection
 *  @return the producer
 */
// ============================================================================
const IAlgorithm*  Hlt::Service::producer
( const Hlt::Selection*    selection ) const
{
  if  ( !selection )
  {
    if ( m_pedantic || msgLevel ( MSG::DEBUG ) )
    { Error ( "producer: invalid selection, return NULL" ) ; }
    return nullptr ;                                                        // RETURN
  }
  return producer ( selection -> id () ) ;
}
// ============================================================================
/*  get the producer for the given selection
 *  @param selection the selection
 *  @return the producer
 */
// ============================================================================
const IAlgorithm*  Hlt::Service::producer
( const std::string& key ) const
{
  if ( !inMap ( key ) )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ("producer: unknown selection'" + key + "', return NULL") ; }
    return nullptr ;
  }
  // ==========================================================================
  for ( const auto & out : m_outputs )
  {
    const SelMap& sels = out.second ;
    SelMap::iterator ifind = sels.find ( key );
    if ( sels.end() != ifind ) { return out.first ; } // RETUTRN
  }
  // ==========================================================================
  if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
  { Warning ("producer: no producer for selection'" + key +
             "', return NULL") ; }
  return nullptr ;
}
// ============================================================================
/*  get all nominal consumers for the given selection
 *  @param selection the selection
 *  @param alglist (OUTPUT) list of consumers
 *  @param number of nominal consumers
 */
// ============================================================================
size_t Hlt::Service::consumers
( const Hlt::Selection* selection ,
  AlgList&              alglist   ) const
{
  alglist.clear() ;
  // ==========================================================================
  if  ( !selection )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "consumers: invalid selection, return empty list") ; }
    return 0 ;                                                        // RETURN
  }
  return consumers ( selection -> id () , alglist ) ;
}
// ============================================================================
/*  get all nominal consumers for the given selection
 *  @param selection the selection
 *  @param alglist (OUTPUT) list of consumers
 *  @param number of nominal consumers
 */
// ============================================================================
size_t Hlt::Service::consumers
( const std::string&    key       ,
  AlgList&              alglist   ) const
{
  alglist.clear() ;
  // ==========================================================================
  if ( !inMap ( key ) )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ("consumers: unknown selection'" + key + "', return empty list") ; }
    return 0 ;
  }
  // ==========================================================================
  for ( const auto &in : m_inputs )
  {
    const SelMap& sels = in.second ;
    SelMap::iterator ifind = sels.find ( key );
    if ( sels.end() != ifind ) { alglist.push_back ( in.first ) ; }
  }
  // ==========================================================================
  if ( alglist.empty() )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ("consumers: no consumers for selection'" + key +
               "', return empty list") ; }
  }
  return alglist.size() ;
}
// ============================================================================
/*  get the inputs for the given algorithm
 *  @param algorithm the algorithm (consumer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of input selections
 */
// ============================================================================
size_t Hlt::Service::inputs
( const IAlgorithm*  algorithm ,
  SelList&           selections ) const
{
  selections.clear() ;
  if ( !algorithm )
  {
    Error ("inputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return inputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the inputs for the given algorithm
 *  @param algorithm the algorithm (consumer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of input selections
 */
// ============================================================================
size_t Hlt::Service::inputs
( const IAlgorithm*  algorithm ,
  KeyList&           selections ) const
{
  selections.clear() ;
  if ( !algorithm )
  {
    Error ("inputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return inputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the inputs for the given algorithm
 *  @param algorithm the algorithm (consumer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of input selections
 */
// ============================================================================
size_t Hlt::Service::inputs
( const std::string& algorithm  ,
  SelList&           selections ) const
{
  selections.clear() ;
  // =========================================================================
  for ( auto &inp : m_inputs )
  {
    if ( algorithm != inp.first->name() ) { continue ; }
    for ( auto& sel : inp.second ) { selections.push_back ( sel.second ) ; }
  }
  // ==========================================================================
  if ( selections.empty() )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ( "inputs: no inputs for algorithm '" + algorithm +
                "', return empty list") ; }
  }
  return selections.size() ;
}
// ============================================================================
/*  get the inputs for the given algorithm
 *  @param algorithm the algorithm (consumer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of input selections
 */
// ============================================================================
size_t Hlt::Service::inputs
( const std::string& algorithm  ,
  KeyList&           selections ) const
{
  selections.clear() ;
  // =========================================================================
  for ( auto& inp : m_inputs )
  {
    if ( algorithm != inp.first->name() ) { continue ; }
    for ( auto& sel : inp.second ) { selections.push_back ( sel.first ) ; }
  }
  // ==========================================================================
  if ( selections.empty() )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ( "inputs: no inputs for algorithm '" + algorithm +
                "', return empty list") ; }
  }
  // ==========================================================================
  return selections.size() ;
}
// ============================================================================
/*  get the outputs for the given algorithm
 *  @param algorithm the algorithm (producer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of output selections
 */
// ============================================================================
size_t Hlt::Service::outputs
( const IAlgorithm*  algorithm ,
  SelList&           selections ) const
{
  selections.clear() ;
  if ( !algorithm )
  {
    Error ("outputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return outputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the outputs for the given algorithm
 *  @param algorithm the algorithm (producer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of output selections
 */
// ============================================================================
size_t Hlt::Service::outputs
( const IAlgorithm*  algorithm ,
  KeyList&           selections ) const
{
  selections.clear() ;
  if ( !algorithm )
  {
    Error ("outputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return outputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the outputs for the given algorithm
 *  @param algorithm the algorithm (producer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of output selections
 */
// ============================================================================
size_t Hlt::Service::outputs
( const std::string&  algorithm ,
  SelList&            selections ) const
{
  selections.clear() ;
  // ==========================================================================
  for ( auto&  out : m_outputs)
  {
    if ( algorithm != out.first->name() ) { continue ; }
    for ( auto&  sel : out.second ) { selections.push_back ( sel.second ) ; }
  }
  // ==========================================================================
  if ( selections.empty() )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ( "outputs: no outputs for algorithm '" + algorithm +
                "', return empty list") ; }
  }
  // ===========================================================================
  return selections.size() ;
}
// ============================================================================
/*  get the outputs for the given algorithm
 *  @param algorithm the algorithm (producer)
 *  @param selections (OUTPUT) the list of input selections
 *  @return number of output selections
 */
// ============================================================================
size_t Hlt::Service::outputs
( const std::string&  algorithm ,
  KeyList&            selections ) const
{
  selections.clear() ;
  // ==========================================================================
  for ( auto& out : m_outputs )
  {
    if ( algorithm != out.first->name() ) { continue ; }
    for ( auto& sel : out.second ) { selections.push_back ( sel.first ) ; }
  }
  // ==========================================================================
  if ( selections.empty() )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ( "outputs: no outputs for algorithm '" + algorithm +
                "', return empty list") ; }
  }
  // ===========================================================================
  return selections.size() ;
}
// ============================================================================
// registered algorithm?
// ============================================================================
bool Hlt::Service::hasAlgorithm ( const IAlgorithm*  alg ) const
{
  if ( !alg )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasAlgorithm: invalid algorithm, return false" ) ; }
    return false ;                                                    // RETURN
  }
  return
    ( m_outputs .end () != m_outputs .find ( alg ) ) ||
    ( m_inputs  .end () != m_inputs  .find ( alg ) ) ;
}
// ============================================================================
// registered algorithm?
// ============================================================================
bool Hlt::Service::hasAlgorithm ( const std::string& alg ) const
{
  using value_t =  std::pair< const IAlgorithm*
                            , GaudiUtils::VectorMap<Gaudi::StringKey
                                                   , Hlt::Selection*>>;
  auto pred = [&](const value_t& o ){ return o.first->name()==alg; };
  return std::any_of( m_outputs.begin(), m_outputs.end(), pred )   // producer?
     ||  std::any_of( m_inputs.begin(),  m_inputs.end(),  pred ) ; // consumer?
}
// ============================================================================
// registered producer?
// ============================================================================
bool Hlt::Service::hasProducer  ( const IAlgorithm*  alg ) const
{
  if ( !alg )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasProducer : invalid algorithm, return false" ) ; }
    return false ;                                                    // RETURN
  }
  return ( m_outputs .end () != m_outputs .find ( alg ) ) ;
}
// ============================================================================
// registered producer?
// ============================================================================
bool Hlt::Service::hasProducer  ( const std::string& alg ) const
{
  // producer?
  using value_t =  std::pair< const IAlgorithm*
                            , GaudiUtils::VectorMap<Gaudi::StringKey
                                                   , Hlt::Selection*>>;
  auto pred = [&](const value_t& o ){ return o.first->name()==alg; };
  return std::any_of(m_outputs.begin (), m_outputs.end (), pred );
}
// ============================================================================
// registered consumer
// ============================================================================
bool Hlt::Service::hasConsumer  ( const IAlgorithm*  alg ) const
{
  if ( !alg )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasConsumer : invalid algorithm, return false" ) ; }
    return false ;                                                    // RETURN
  }
  return ( m_inputs  .end () != m_inputs  .find ( alg ) ) ;
}
// ============================================================================
// registered producer?
// ============================================================================
bool Hlt::Service::hasConsumer  ( const std::string& alg ) const
{
  // consumer ?
  using value_t =  std::pair< const IAlgorithm*
                            , GaudiUtils::VectorMap<Gaudi::StringKey
                                                   , Hlt::Selection*>>;
  auto pred = [&](const value_t& o ){ return o.first->name()==alg; };
  return std::any_of( m_inputs.begin  (),  m_inputs.end  (), pred );
}
// ============================================================================
// registered selection ?
// ============================================================================
bool Hlt::Service::hasSelection ( const Hlt::Selection* selection ) const
{
  //
  if ( !selection )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasSelection: invalid selection, return false" ) ; }
    return false ;                                                   // RETURN
  }
  //
  return std::any_of(m_selections.begin(),m_selections.end(),
                     [&](const std::pair<Gaudi::StringKey,Hlt::Selection*>& sel)
                     { return sel.second == selection; } );
}
// ============================================================================
namespace
{
  // ==========================================================================
  struct _AlgCmp
    : public std::binary_function<const IAlgorithm*,const IAlgorithm*,bool>
  {
    inline bool operator ()
      ( const IAlgorithm* alg1 ,
        const IAlgorithm* alg2 ) const
    {
      return
        alg1 == alg2 ? false :
               !alg1 ? true  :
               !alg2 ? false : alg1->name() < alg2->name() ;
    }
  };
  // ==========================================================================
}
// ============================================================================
/*  get all algorithms
 *  @param algs (OUTPUT) the list of algorithms
 *  @return number of algorithms
 */
// ============================================================================
size_t Hlt::Service::algorithms ( Hlt::IInspector::AlgList& algs ) const
{
  algs.clear() ;
  std::set<const IAlgorithm*,_AlgCmp> myalgs ;
  for  ( InputMap::const_iterator ii = m_inputs .begin() ;
         m_inputs .end() != ii ; ++ii ) { myalgs.insert ( ii -> first ) ; }
  for  ( InputMap::const_iterator io = m_outputs.begin() ;
         m_outputs.end() != io ; ++io ) { myalgs.insert ( io -> first ) ; }
  // sor
  algs.insert ( algs.end() , myalgs.begin() , myalgs.end() ) ;
  //
  return algs.size () ;
}
// ============================================================================
/*  get all selections
 *  @param keys (OUTPUT) the list of selections
 *  @return number of selections
 */
// ============================================================================
size_t Hlt::Service::selections ( Hlt::IInspector::KeyList& keys ) const
{
  keys.clear() ;
  for ( SelMap::const_iterator ikey = m_selections.begin() ;
        m_selections.end () != ikey ; ++ikey ) { keys.push_back ( ikey->first ) ; }
  //
  std::sort( keys.begin() , keys.end() ) ;
  //
  return keys.size() ;
}
// ============================================================================
/* get the input TES locations for the given reader
 * @param reader the algorithm
 * @param locations (OUTPUT) the list of input TES locations
 * @return numebr of locations
 */
// ============================================================================
size_t Hlt::Service::readTES
( const IAlgorithm*         reader    ,
  Hlt::IInspector::KeyList& locations ) const
{
  //
  locations.clear() ;
  if ( !reader )
  {
    Error ( "read : invalid algorithm!" ) ;
    return 0 ;                                                       // RETURN
  }
  //
  TESMap::const_iterator intes = m_tesmap.find ( reader ) ;
  if ( m_tesmap.end () == intes )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ( "readTES(1):  no TES-inputs for reader '" + reader -> name() + "'") ; }
    return 0 ;
  }
  //
  locations.insert
    ( locations.end ()         ,
      intes -> second.begin () ,
      intes -> second.end   () ) ;
  //
  return locations.size() ;                                          // RETURN
}
// ============================================================================
/* get the input TES locations for the given reader
 * @param reader the algorithm
 * @param locations (OUTPUT) the list of input TES locations
 * @return numebr of locations
 */
// ============================================================================
size_t Hlt::Service::readTES
( const std::string&        reader    ,
  Hlt::IInspector::KeyList& locations ) const
{
  locations.clear() ;
  for ( TESMap::const_iterator imap = m_tesmap.begin() ;
        m_tesmap.end() != imap ; ++imap )
  {
    const IAlgorithm* ialg = imap->first ;
    if ( !ialg ) { continue ; }                               // CONTINUE
    if ( ialg->name() == reader )
    { return readTES ( ialg , locations ) ; }                      // RETURN
  }
  //
  if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
  { Warning ( "readTES(2) :  no TES-inputs for reader '" + reader + "'") ; }
  //
  return locations.size() ;
}
// ============================================================================
/*  get the "readers" for the given TES-location
 *  @param location TES-location
 *  @param alglist (OUTPUT) the list of readers
 *  @return number of readers
 */
// ============================================================================
size_t Hlt::Service::readers
( const std::string&    location  ,
  AlgList&              alglist   ) const
{
  alglist.clear() ;
  for ( TESMap::const_iterator imap = m_tesmap.begin() ;
        m_tesmap.end() != imap ; ++imap )
  {
    TESLocs::const_iterator it = imap->second.find ( location ) ;
    if ( imap->second.end () == it ) { continue ; }              // CONTINUE
    alglist.push_back  ( imap->first ) ;
  }
  if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
  {
    if ( alglist.empty() )
    { Warning ( "readers:  not readers for location '" + location + "'") ; }
  }
  //
  return alglist.size () ;                                            // RETURN
}
// ============================================================================
/*  get all readers
 *  @param alglist (OUTPUT) the list of readers
 *  @return number of readers
 */
// ============================================================================
size_t Hlt::Service::allReaders ( AlgList& alglist   ) const
{
  alglist.clear() ;
  for ( TESMap::const_iterator imap = m_tesmap.begin() ;
        m_tesmap.end() != imap ; ++imap ) { alglist.push_back ( imap->first ) ; }
  //
  if ( alglist.empty() )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ( "allReaders:  not readers" ) ; }
  }
  return alglist.size() ;
}
// ============================================================================
/** get all TES-inptu locations
 *  @param locations (OUTPUT) the list of input TES locations
 *  @return number of locations
 */
// ============================================================================
size_t Hlt::Service::allTES ( KeyList&              locations ) const
{
  locations.clear() ;
  std::set<std::string> tmp ;
  for ( TESMap::const_iterator imap = m_tesmap.begin() ;
        m_tesmap.end() != imap ; ++imap )
  {
    const TESLocs& locs = imap->second ;
    for ( TESLocs::const_iterator it = locs.begin() ; locs.end( )!= it ; ++it )
    { tmp.insert ( *it ) ; }
  }
  //
  locations.insert ( locations.end() , tmp.begin () , tmp.end () ) ;
  //
  if ( locations.empty() )
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Warning ( "allTES:  now locations" ) ; }
  }
  return locations.size() ;
  //
}
// ============================================================================
// The END
// ============================================================================
