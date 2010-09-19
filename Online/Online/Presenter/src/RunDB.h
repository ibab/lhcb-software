// $Id: RunDB.h,v 1.2 2010-09-19 18:49:53 robbep Exp $
#ifndef PRESENTER_RUNDB_H 
#define PRESENTER_RUNDB_H 1

// Include files
#include <string>
#include <vector>

// forward declarations
namespace boost {
  namespace posix_time {
    class ptime ;
  };
};

/** @class RunDB RunDB.h
 *  
 *  Interface to run db for presenter
 *
 *  @author Patrick Robbe
 *  @date   2010-05-10
 */
class RunDB {
public: 
  /// Standard constructor
  RunDB( std::string address );

  virtual ~RunDB( ); ///< Destructor

  /// Get last run number (for all destination or all partition, give empty string
  int getLastRun( ) ;

  /// Get next run number (compared to current run number) 
  int getNextRun( ) ;

  /// Get previous run number (compared to current run number) 
  int getPreviousRun( ) ;

  /// Get list of runs for a given partition (+/- 10 runs)
  std::vector< int > getListOfRuns( int runNumber ) ;

  /// Get current run's start time
  std::string getCurrentStartTime( ) { return m_currentRunStartTime ; } ;

  /// Get current run's end time
  std::string getCurrentEndTime( ) { return m_currentRunEndTime ; } ;
  
  /// Get current run's duration
  std::string getCurrentRunDuration( ) { return m_currentRunDuration ; } ;

  /// Get current run number (cached)
  int getCurrentRunNumber( ) { return m_currentRunNumber ; } ;

  /// Check if this run exists
  bool checkRun( int runNumber ) ;

  /// Get partition
  std::string getPartition() { return m_partition ; } ;
    
  /// Set Partition
  void setPartition( const std::string & partition ) {
    m_partition = partition ; } ;

  /// Get destination
  std::string getDestination() { return m_destination ; } ;

  /// Set Destination
  void setDestination( const std::string & destination ) {
    m_destination = destination ; } ;

private:
  /// RunDB Web server address
  std::string m_address;

  /// Current (cached) run number
  int m_currentRunNumber ;

  /// Current run's start time
  std::string m_currentRunStartTime ;

  /// Current run's end time
  std::string m_currentRunEndTime ;

  /// Current run's duration 
  std::string m_currentRunDuration ;

  /// Partition
  std::string m_partition ;

  /// Destination
  std::string m_destination ;

  /// Convert to convenient string format
  std::string convertRunTimeToString( boost::posix_time::ptime * st ) const ;

};
#endif // PRESENTER_RUNDB_H
