// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineHistDBEnv.h,v 1.1.1.1 2007-01-16 15:18:12 ggiacomo Exp $
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
#include <occi.h>
using namespace oracle::occi;
using namespace std;

class OnlineHistDBEnv {
 public:
  OnlineHistDBEnv() {};
  OnlineHistDBEnv(Connection* Conn) : m_conn(Conn) {}
  virtual ~OnlineHistDBEnv() {};
  Connection* conn() const {return m_conn;}
  void dumpError(SQLException& ex,std::string MethodName) const {
    cout<<"Exception thrown for "<< MethodName <<endl;
    cout<<"Error number: "<<  ex.getErrorCode() << endl;
    cout<<ex.getMessage() << endl;
  }
 protected:
  Environment* m_env;
  Connection* m_conn;
};

#endif // ONLINEHISTDBENV_H
