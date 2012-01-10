// $Id: ROShifter.h,v 1.1 2010-10-18 06:25:08 ocallot Exp $
#ifndef SHIFTDB_H 
#define SHIFTDB_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/gregorian/formatters.hpp"

/** @class ROShifter ROShifter.h
 *  
 *  Shift DB interface for presenter
 *
 *  @author Patrick Robbe
 *  @date   2010-03-01
 */

class ROShifter {
 public: 
  /// Standard constructor
  ROShifter( ) ; 

  virtual ~ROShifter( ); ///< Destructor

  /// Name of the data manager on shift now
  std::string getDataManager() ;

  /// Name of the shift leader on shift now
  std::string getShiftLeader() ;

  /// Name of the Production manager on shift now
  std::string getProduction() ;

  /// Name of the SLIMOS in shutdownon shift now
  std::string getSLIMOS() ;

  /// Return true if the shift crew has changed
  bool hasChanged();

protected:

private:
  /// Get name of people on shift in a given slot
  std::string getShifter( const std::string & function ) ;

  /// Read and interpret web file obtained from ShiftDB web server 
  void readWebFile( ) ;

  /// Pointer to shift db web script file
  bool m_open ;

  /// Map of people on shift
  std::map< std::string , std::vector< std::string > > m_shifters ;

  boost::gregorian::date m_lastDay;    /// last day for which teh database was read
  int m_lastSlot;  /// index of last shift
};
#endif // SHIFTDB_H
