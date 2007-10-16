// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDBEnv.h,v 1.10 2007-10-16 12:16:10 ggiacomo Exp $
#ifndef ONLINEHISTDBENV_H
#define ONLINEHISTDBENV_H 1
/** @class  OnlineHistDBEnv OnlineHistDBEnv.h OnlineHistDB/OnlineHistDBEnv.h
 * 
 *  environment class for C++ interface to the Online Monitoring Histogram DB
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 1/12/2006
 */

#include <iostream>
#include <sstream>
#include <cctype>
#include <occi.h>
using namespace oracle::occi;
using namespace std;

namespace OnlineHistDBEnv_constants {
  static const char HistTypeName[][4] = {
    "H1D", "H2D", "P1D", "P2D", "CNT", "SAM"
  };  
  static const unsigned int maxSNsize = 130; 
  static const unsigned int maxANsize = 100;
  static const unsigned int maxTNsize = 250;
  static const unsigned int maxHTsize = 3;
  static const std::string DB="lbora01:1528/HISTOGRAMDB";
  static const std::string ACCOUNT="HIST_WRITER";
  static const std::string m_SetSeparator = "_$";
}




class OnlineHistDBEnv {
 public:
  /// get verbosity level (0 for no debug messages, up to 3)
  typedef enum { H1D, H2D, P1D, P2D, CNT, SAM} HistType;
  int debug() const { return m_debug;}
  /// set verbosity level
  void setDebug(int DebugLevel) { m_debug=DebugLevel;}
  /// exception level: 0 means never throw exc. to client code, 1 means only
  /// severe errors (default value), 2 means throw all exceptions.
  /// The default value is 1, meaning that exceptions are thrown only in
  /// case of severe DB inconsistency. All other errors, e.g. syntax errors,
  /// can be checked from the method return values and from the warning
  /// messages on the standard output.
  /// exception can catched using {\it catch(SQLException ex)}
  int excLevel() const {return m_excLevel;}
  ///
  void setExcLevel(int ExceptionLevel) {m_excLevel=ExceptionLevel;}
  /// check the syntax of the page full name, returning the correct syntax and the folder name 
  std::string PagenameSyntax(std::string fullname, std::string &folder);
  /// standard way to dump error message
  void errorMessage(std::string Error) const; 

 protected:
  OnlineHistDBEnv(std::string User); 
  OnlineHistDBEnv(Connection* Conn, 
		  std::string User, 
		  int ExcLevel);
  OnlineHistDBEnv(OnlineHistDBEnv &m);
  virtual ~OnlineHistDBEnv() {};
  
  void dumpError(SQLException& ex,std::string MethodName) const; 
  Environment* m_env;
  Connection* m_conn;
  std::string m_user;
  
 private: 
  std::string toUpper(string str);
  int m_debug;
  int m_excLevel;
};

#endif // ONLINEHISTDBENV_H
