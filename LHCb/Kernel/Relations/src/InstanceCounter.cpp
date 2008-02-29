// $Id: InstanceCounter.cpp,v 1.3 2008-02-29 17:46:30 ibelyaev Exp $ 
// ======================================================================
// Include files 
// ======================================================================
// ST D& STL 
// ======================================================================
#include <iostream>
// ======================================================================
#include "Relations/RelationUtils.h"
// ======================================================================
/** @file
 *  implementation file forthe class Relations::InstanceCounter
 *  @see Relations:InstanceCounter
 *  @author Vanya BELYAEV ibelyaev@phsyics.syr.edu
 *  @date 2006-02-05
 */
// ============================================================================
// constructor 
// ============================================================================
Relations::InstanceCounter::InstanceCounter() : m_counters() {}
// ============================================================================
// destructor 
// ============================================================================
Relations::InstanceCounter::~InstanceCounter() 
{ report () ; m_counters.clear() ; }
// ============================================================================
/*  make a report 
 *  @return the total number of alive objects 
 */
// ============================================================================
Relations::InstanceCounter::counter 
Relations::InstanceCounter::report () const 
{
  counter total = 0 ;
  for( Counters::iterator record = m_counters.begin() ; 
       m_counters.end() != record ; ++record ) 
  {
    if ( 0 == record->second ) { continue ; }  
    std::cout << "RelationUtils::InstanceCounter " 
              << " WARNING \t #" 
              << record->second 
              << " objects of type \t '"
              << record->first 
              << "' \t still alive " << std::endl ;
    ++total ;
  }
  return total ;
}
// ============================================================================
/*  increment the counter
 *  @param type object type 
 *  @return the current value of counter
 */
// ============================================================================
Relations::InstanceCounter::counter 
Relations::InstanceCounter::increment ( const std::string& type ) 
{ return ++m_counters[type] ;}    
// ============================================================================
/*  decrement the counter
 *  @param type object type 
 *  @return the current value of counter
 */
// ============================================================================
Relations::InstanceCounter::counter 
Relations::InstanceCounter::decrement ( const std::string& type ) 
{ return --m_counters[type] ;}    
// ============================================================================
/*  get the current value of the counter
 *  @param type object type 
 *  @return the current value of counter
 */
// ============================================================================
Relations::InstanceCounter::counter 
Relations::InstanceCounter::count     ( const std::string& type ) const 
{ return   m_counters[type] ;}
// ============================================================================
// the accessor to static instance
// ============================================================================
Relations::InstanceCounter&
Relations::InstanceCounter::instance() 
{
  static Relations::InstanceCounter s_counter ;
  return s_counter ;
}
// ============================================================================


// ======================================================================
// The END 
// ======================================================================
