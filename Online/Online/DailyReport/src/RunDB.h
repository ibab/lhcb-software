// $Id: $
#ifndef RUNDB_H 
#define RUNDB_H 1

// Include files
#include <string>
#include <vector>

/** @class RunDB RunDB.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2011-05-17
 */
struct Fill {
  int number;
  std::string time;
  double lumiTot;
  double lumiHV;
  double lumiVelo;
  double lumiRunning;
  double lumiLogged;
};


class RunDB {
public: 
  /// Standard constructor
  RunDB( std::string address ); 

  virtual ~RunDB( ); ///< Destructor

  void fillInformation( int fillNb );

  int nextFill() const { return m_nextFill; }

  void setMinDate( int year, int month, int day, int hour ) {
    m_minDate = codeDate( year, month, day, hour );
  }

  void publish( FILE* web );
  
protected:
  int codeDate(int year, int month, int day, int hour ) {
    return hour + 100 * ( day + 100 * ( month + 100 * (year-2010) ) );
  }
 
private:
  std::string m_address;
  std::vector<Fill> m_fills;
  int m_nextFill;
  int m_minDate;
};
#endif // RUNDB_H
