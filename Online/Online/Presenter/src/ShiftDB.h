// $Id: ShiftDB.h,v 1.1 2010-03-10 11:01:49 robbep Exp $
#ifndef SHIFTDB_H 
#define SHIFTDB_H 1

// Include files
#include <string>

// Forward declarations
namespace oracle {
  namespace occi {
    class Environment ;
    class Connection ;
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
  oracle::occi::Environment * m_env ;
  oracle::occi::Connection  * m_conn ;
};
#endif // SHIFTDB_H
