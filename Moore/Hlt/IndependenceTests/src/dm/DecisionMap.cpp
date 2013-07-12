// $Id: $
// Include files 
#include <iostream>
#include <exception>

// STL
#include <vector>

// boost
#include <boost/foreach.hpp>

// local
#include <Independence/DecisionMap.h>

//-----------------------------------------------------------------------------
// Implementation file for class : DecisionMap
//
// 2010-03-26 : Roel Aaij
//-----------------------------------------------------------------------------

using std::cout;
using std::string;
using std::endl;
using std::vector;
using std::runtime_error;

//=============================================================================
DecisionMap::DecisionMap()
   : m_runNumber( 0 ), m_eventNumber( 0 )
{
   m_map = new decisionMap_t;
   m_cached = m_map->end();
}

//=============================================================================
DecisionMap::~DecisionMap()
{
   if ( m_map ) delete m_map;
} 

//=============================================================================
bool DecisionMap::decision( const unsigned int runNumber,
                            const unsigned int eventNumber,
                            const std::string& lineName ) const
{
   decisionMap_t::iterator it;
   if ( runNumber == m_runNumber && eventNumber == m_eventNumber ) {
      it = m_cached;
   } else {
      eventKey_t key( runNumber, eventNumber );
      it = m_map->find( key );
      if ( it == m_map->end() ) return false;
      //throw runtime_error( "Event not in map" );
      m_runNumber = runNumber;
      m_eventNumber = eventNumber;
      m_cached = it;
   }
   return it->second.count( lineName );
}

//=============================================================================
void DecisionMap::addDecision( const unsigned int runNumber,
                               const unsigned int eventNumber,
                               const std::string& lineName,
                               const bool value )
{
   if ( !value ) return;

   if ( runNumber == m_runNumber && eventNumber == m_eventNumber ) {
      m_cached->second.insert( lineName );
   } else {
      eventKey_t key( runNumber, eventNumber );
      decisionMap_t::iterator it = m_map->find( key );
      if ( it != m_map->end() ) {
         it->second.insert( lineName );
         m_cached = it;
      } else {
         decisions_t set;
         set.insert( lineName );
         m_map->insert( make_pair( key, set ) );
         m_cached = m_map->find( key );
      }
      m_runNumber = runNumber;
      m_eventNumber = eventNumber;
   }
}

//=============================================================================
vector< DecisionMap::eventKey_t > DecisionMap::events() const
{
   vector< eventKey_t > evts;
   evts.reserve( m_map->size() );
   BOOST_FOREACH( const decisionMap_t::value_type& entry, *m_map ) {
      evts.push_back( entry.first );
   }
   return evts;
}

//=============================================================================
bool DecisionMap::compare( const unsigned int runNumber,
                           const unsigned int eventNumber,
                           const std::string& first_line,
                           const std::string& second_line ) const
{
   return decision( runNumber, eventNumber, first_line ) ==
      decision( runNumber, eventNumber, second_line );
}
