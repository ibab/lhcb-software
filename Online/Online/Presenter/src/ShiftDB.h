// $Id: ShiftDB.h,v 1.2 2010-03-18 20:17:47 robbep Exp $
#ifndef SHIFTDB_H 
#define SHIFTDB_H 1

// Include files
#include <string>
#include <map>
#include <vector>

// Forward declarations
namespace boost {
  namespace gregorian {
    class date ;
  };
};

/** @class ShiftDB ShiftDB.h
 *  
 *  Shift DB interface for presenter
 *
 *  @author Patrick Robbe
 *  @date   2010-03-01
 */

class ShiftDB {
 public: 
  /// Standard constructor
  ShiftDB( ) ; 

  virtual ~ShiftDB( ); ///< Destructor

  /// Name of the data manager on shift now
  std::string getCurrentDataManager() ;

protected:

private:
  /// Get name of people on shift in a given slot
  std::string getShifter( const std::string & function , 
			  const int slot ) ;

  /// Read and interpret web file obtained from ShiftDB web server 
  void readWebFile( const boost::gregorian::date * day ) ;

  /// Pointer to shift db web script file
  bool m_open ;

  /// Map of people on shift
  std::map< std::string , std::vector< std::string > > m_shifters ;
};
#endif // SHIFTDB_H
