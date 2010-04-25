// $Id: ProblemDB.h,v 1.3 2010-04-25 17:56:00 robbep Exp $
#ifndef PROBLEMDB_H 
#define PROBLEMDB_H 1

// Include files
#include <string>

/** @class ProblemDB ProblemDB.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2010-03-22
 */
class ProblemDB {
public: 
  /// Standard constructor
  ProblemDB( std::string address );

  virtual ~ProblemDB( ); ///< Destructor

  int post( std::string system, std::string username, std::string title, std::string message, std::string logReference );
  std::string getReference() const { return m_reference; }

  /// get list of currently open problems in database
  void getListOfOpenedProblems( std::vector< std::vector< std::string > > & problems , 
				const std::string & system ) ;
  
protected:
  std::string urlEncode( std::string src );
  
private:
  std::string m_address;
  std::string m_reference;
};
#endif // PROBLEMDB_H
