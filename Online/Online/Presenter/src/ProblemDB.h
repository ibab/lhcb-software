// $Id: ProblemDB.h,v 1.1 2010-03-23 12:51:41 ocallot Exp $
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
  
protected:
  std::string urlEncode( std::string src );
  
private:
  std::string m_address;
  std::string m_reference;
};
#endif // PROBLEMDB_H
