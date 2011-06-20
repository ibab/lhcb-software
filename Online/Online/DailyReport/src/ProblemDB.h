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
  ProblemDB( std::string address, std::string port = "http" );

  virtual ~ProblemDB( ); ///< Destructor

  /// get list of problems in database, if run number set to 0, give list of opened problems
  void getListOfProblems( std::vector< std::vector< std::string > > & problems , 
                          const std::string & system ) ;
  
protected:
  std::string urlEncode( std::string src );
  
private:
  std::string m_address;
  std::string m_port;
  std::vector<std::string> m_monthName;
};
#endif // PROBLEMDB_H
