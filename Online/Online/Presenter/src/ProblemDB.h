// $Id: ProblemDB.h,v 1.4 2010-09-19 18:49:53 robbep Exp $
#ifndef PROBLEMDB_H 
#define PROBLEMDB_H 1

// Include files
#include <string>
#include <vector>

/** @class ProblemDB ProblemDB.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2010-03-22
 */
class ProblemDB {
public: 
  /// Standard constructor
  ProblemDB( std::string address, std::string rundbAddress );

  virtual ~ProblemDB( ); ///< Destructor

  int post( std::string system, std::string username, std::string title, std::string message, 
            std::string logReference, std::string severity );
  std::string reference() const { return m_reference; }

  /// get list of problems in database, if run number set to 0, give list of opened problems
  std::vector< std::vector< std::string > > listOfProblems( const std::string & system ,
                                                            int runNumber ) ;

  void getSystems();
  
protected:
  std::string urlEncode( std::string src );
  
private:
  std::string m_address;
  std::string m_port;
  std::string m_rundbAddress;
  std::string m_reference;
  std::vector<std::string> m_systems;
};
#endif // PROBLEMDB_H
