// $Id: Shifts.h,v 1.1 2010-10-18 06:25:08 ocallot Exp $
#ifndef SHIFTDB_H 
#define SHIFTDB_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/gregorian/formatters.hpp"

/** @class Shifts Shifts.h
 *  
 *  Shift DB interface for presenter
 *
 *  @author Patrick Robbe
 *  @date   2010-03-01
 */

class Shifts {
 public: 
  /// Standard constructor
  Shifts( ) ; 

  virtual ~Shifts( ); ///< Destructor

  /// load the data from the web. Date in form YYYYMMDD
  bool loadFromWeb( std::string date );

  /// Get name of people on shift in a given slot
  std::string name( const std::string & function ) ;

  /// Get name of people on shift in a given slot
  const char* name( const std::string & function, int slot ) ;

protected:

private:

  /// Map of people on shift
  std::map< std::string , std::vector< std::string > > m_shifters ;

};
#endif // SHIFTDB_H
