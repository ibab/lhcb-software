// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDBEnv.h,v 1.6 2007-07-09 17:23:23 ggiacomo Exp $
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
    "H1D", "H2D", "P1D", "P2D", "CNT"
  };  
  static const unsigned int maxSNsize = 130; 
  static const unsigned int maxANsize = 100;
  static const unsigned int maxTNsize = 250;
  static const unsigned int maxHTsize = 3;
  static const std::string DB="lbora01:1528/HISTOGRAMDB";
  static const std::string ACCOUNT="HIST_WRITER";
}




class OnlineHistDBEnv {
 public:
  typedef enum { H1D, H2D, P1D, P2D, CNT} HistType;
  int debug() const { return m_debug;}
  void setDebug(int DebugLevel) { m_debug=DebugLevel;}
  /// exception level: 0 means never throw exc. to client code, 1 means only severe errors, 2 means throw all exceptions
  int excLevel() const {return m_excLevel;}
  void setExcLevel(int ExceptionLevel) {m_excLevel=ExceptionLevel;}
 protected:
  OnlineHistDBEnv(std::string User) 
    : m_user(toUpper(User)), m_debug(0), m_excLevel(1) {}
  OnlineHistDBEnv(Connection* Conn, 
		  std::string User, 
		  int ExcLevel) 
    : m_conn(Conn), m_user(toUpper(User)), m_debug(0), m_excLevel(ExcLevel) {}
  OnlineHistDBEnv(OnlineHistDBEnv &m) {
    m_env = m.m_env; m_conn= m.m_conn; m_user=m.m_user;
    m_debug = m.debug(); m_excLevel = m.excLevel();
  }
  virtual ~OnlineHistDBEnv() {};
  //Connection* conn() const {return m_conn;}
  
  void dumpError(SQLException& ex,std::string MethodName) const {
    if (m_debug > -1) {
      cout << ( (ex.getErrorCode() < -20500) ? "------- FATAL ERROR: ----------" :
		"------- WARNING: ----------" ) <<endl;
      cout<<"Exception thrown from "<< MethodName <<endl;
      cout<<"Error number: "<<  ex.getErrorCode() << endl;
      cout<<ex.getMessage() << endl;
    }     
    if(m_excLevel >1  || (m_excLevel == 1 && ex.getErrorCode() < -20500))
      throw(ex);
  }
  void errorMessage(std::string Error) const {
    if (m_debug > -1) {
      cout << "------- WARNING: ----------"  <<endl;
      cout<< Error <<endl;
    }     
  }
  Environment* m_env;
  Connection* m_conn;
  std::string m_user;
 private:
  std::string toUpper(string str) {
    for(unsigned int i=0;i<str.length();i++)
      str[i] = toupper(str[i]);
    return str;
  }
  int m_debug;
  int m_excLevel;
};

#endif // ONLINEHISTDBENV_H
