// $Id: RelationUtils.cpp,v 1.2 2005-01-27 14:48:49 cattanem Exp $
// ============================================================================
// CVS tag $name:$
// ============================================================================
// Include files
// STD & STL
#include <string>
#include <cstdio>
#include <iostream>
// GaudiKernel
#include "GaudiKernel/IInterface.h"   
#include "GaudiKernel/ClassID.h"      
// Relations
#include "Relations/RelationUtils.h"

// ============================================================================
/** @file RelationUtils.cpp
 *  
 *  Implementation file for method from namespace Relation
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 28/01/2002 
 */
// ============================================================================

// ============================================================================
/**
 *  funtion to create the uniqie relation 
 *  interfaceID from interface name, CLIDs and types 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 * 
 *  @param id       interface name
 *  @param idFrom   ID for the first ("FROM") object  
 *  @param idTo     ID for teh second("TO")   object
 *  @param weight   ID of weight class 
 *  @param major    interface major version 
 *  @param minor    interface minor version 
 */
// ============================================================================
InterfaceID Relations::interfaceID 
( const std::string&  id       ,
  const unsigned long idFrom   ,
  const unsigned long idTo     ,
  const unsigned long weight   ,
  const unsigned long major    , 
  const unsigned long minor    )
{
  std::string name( id ) ;
  char a[128];
  name     += "<" + std::string( a , a + sprintf( a , "%d" , (int) idFrom ) ) ;
  name     += "," + std::string( a , a + sprintf( a , "%d" , (int) idTo   ) ) ;
  if( 0 != weight  ) 
    { name += "," + std::string( a , a + sprintf( a , "%d" , (int) weight ) ) ;}
  name += ">" ;
  ///
  return InterfaceID( name.c_str() , major , minor );
};

// ============================================================================
/**
 *  function to create the uniqie relation 
 *  clid from object name, CLIDs and types 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 * 
 *  @param id       object name
 *  @param idFrom   ID for the first ("FROM") object  
 *  @param idTo     ID for the second("TO")   object
 *  @param weight   ID of weight class 
 *  @param major    major version 
 *  @param minor    minor version 
 */
// ============================================================================
CLID        Relations::clid        
( const std::string&  id       ,
  const unsigned long idFrom   ,
  const unsigned long idTo     ,
  const unsigned long weight   , 
  const unsigned long major    , 
  const unsigned long minor    ) 
{
  std::string name( id ) ;
  char a[128];
  name     += "<" + std::string( a , a + sprintf( a , "%d" , (int) idFrom ) ) ;
  name     += "," + std::string( a , a + sprintf( a , "%d" , (int) idTo   ) ) ;
  if( 0 != weight  ) 
    { name += "," + std::string( a , a + sprintf( a , "%d" , (int) weight ) ) ;}
  name += ">" ;
  //
  CLID cl = InterfaceID( name.c_str() , major , minor ).id() ;
  // set ObjectVector bit to NULL 
  cl = ~CLID_ObjectVector & cl ;
  // set ObjectList   bit to NULL 
  cl = ~CLID_ObjectList   & cl ;
  return cl ;
};
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
Relations::InstanceCounter::InstanceCounter() : m_counters() {}
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
Relations::InstanceCounter::~InstanceCounter() 
{ report () ; m_counters.clear() ; };
// ============================================================================

// ============================================================================
/** make a report 
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
                << " INFO \t #" 
                << record->second 
                << " objects of type \t '"
                << record->first 
                << "' \t still alive " << std::endl ;
      ++total ;
    }
  return total ;
};
// ============================================================================

// ============================================================================
/** increment the counter
 *  @param type object type 
 *  @return the current value of counter
 */
// ============================================================================
Relations::InstanceCounter::counter 
Relations::InstanceCounter::increment ( const std::string& type ) 
{ return ++m_counters[type] ;}    
// ============================================================================

// ============================================================================
/** decrement the counter
 *  @param type object type 
 *  @return the current value of counter
 */
// ============================================================================
Relations::InstanceCounter::counter 
Relations::InstanceCounter::decrement ( const std::string& type ) 
{ return --m_counters[type] ;}    
// ============================================================================


// ============================================================================
/** get the current value of the counter
 *  @param type object type 
 *  @return the current value of counter
 */
// ============================================================================
Relations::InstanceCounter::counter 
Relations::InstanceCounter::count     ( const std::string& type ) const 
{ return   m_counters[type] ;}
// ============================================================================

// ============================================================================
// the accessor to static instance
// ============================================================================
Relations::InstanceCounter&
Relations::InstanceCounter::instance() 
{
  static Relations::InstanceCounter s_counter ;
  return s_counter ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
