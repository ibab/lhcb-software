// $Id: ShiftDB.cpp,v 1.1 2010-03-10 11:01:49 robbep Exp $
// Include files

// local
#include "ShiftDB.h"

// STL
#include <iostream>
#include <ctime>

// Boost
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/gregorian/formatters.hpp"

// Oracle
#include "occiControl.h" 

//-----------------------------------------------------------------------------
// Implementation file for class : ShiftDB
//
// 2010-03-01 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ShiftDB::ShiftDB(  ) : m_env( 0 ) , m_conn( 0 ) {
  // Create environment
  m_env = oracle::occi::Environment::createEnvironment() ;
  
  // Create connection
  try {
    m_conn = m_env -> createConnection( "shiftdb_reader" , "reader" , "shiftdb" ) ;
  } catch ( std::exception & exc ) {
    std::cerr << "Error opening database : " << exc.what() << std::endl ;
    m_conn = 0 ;
  }
}

//=============================================================================
// Destructor
//=============================================================================
ShiftDB::~ShiftDB() {
  // Close connection
  if ( 0 != m_conn ) m_env -> terminateConnection( m_conn ) ;

  // Terminate environment
  oracle::occi::Environment::terminateEnvironment( m_env ) ;
}

//=============================================================================
// Name of data manager on shift
//=============================================================================
std::string ShiftDB::getCurrentDataManager( ) {
  if ( 0 == m_conn ) return "Data Manager" ;

  std::stringstream sql_statement ;
  std::string result ;
  sql_statement << "SELECT PEOPLE.FIRST_NAME, PEOPLE.NAME, SHIFTS.FN_SLOT " ;
  sql_statement << "FROM SHIFTS, PEOPLE " ;
  sql_statement << "WHERE (SHIFTS.START_TIME=TO_CHAR(TO_DATE('" ;

  boost::posix_time::ptime now = 
    boost::posix_time::second_clock::local_time() ;
  boost::gregorian::date now_day = now.date() ;

  int slot = 0 ;

  if ( now.time_of_day().hours() <= 7 ) {
    now_day = now.date() - boost::gregorian::date_duration( 1 ) ;
    slot = 4 ;
  } else if ( now.time_of_day().hours() <= 15 ) slot = 1 ;
  else if ( now.time_of_day().hours() <= 23 ) slot = 2 ;
  else slot = 4 ;

  sql_statement << boost::gregorian::to_simple_string( now_day ) ;

  sql_statement << "','YYYY-Mon-DD')) AND SHIFTS.PID=PEOPLE.ID " 
		<< "AND SHIFTS.FN_ID=2 AND SHIFTS.FN_SLOT="
		<< slot << ")" ;
  
  try {
    oracle::occi::Statement * stmt = 
      m_conn -> createStatement( sql_statement.str().c_str() ) ; 
    oracle::occi::ResultSet * rs = stmt -> executeQuery() ;
    rs -> next() ;
    result = ( rs -> getString( 1 ) + " " + rs -> getString( 2 ) ) ; 
    stmt -> closeResultSet( rs ) ;
    m_conn -> terminateStatement( stmt ) ;
  } catch ( std::exception & exc ) {
    std::cerr << "Error in SQL statement : " << exc.what() << std::endl ;
    return "Data Manager" ;
  }

  return result ;
}
