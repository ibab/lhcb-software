//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistDBEnv.cpp,v 1.1 2007-09-28 15:46:07 ggiacomo Exp $
#include "OnlineHistDB/OnlineHistDBEnv.h"

OnlineHistDBEnv::OnlineHistDBEnv(std::string User) 
  : m_user(toUpper(User)), m_debug(0), m_excLevel(1) {}

OnlineHistDBEnv::OnlineHistDBEnv(Connection* Conn, 
				 std::string User, 
				 int ExcLevel) 
  : m_conn(Conn), m_user(toUpper(User)), m_debug(0), m_excLevel(ExcLevel) {}

OnlineHistDBEnv::OnlineHistDBEnv(OnlineHistDBEnv &m) {
  m_env = m.m_env; m_conn= m.m_conn; m_user=m.m_user;
  m_debug = m.debug(); m_excLevel = m.excLevel();
}


void OnlineHistDBEnv::dumpError(SQLException& ex,std::string MethodName) const {
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

void OnlineHistDBEnv::errorMessage(std::string Error) const {
  if (m_debug > -1) {
    cout << "------- WARNING: ----------"  <<endl;
    cout<< Error <<endl;
  }     
}

std::string OnlineHistDBEnv::toUpper(string str) {
  for(unsigned int i=0;i<str.length();i++)
    str[i] = toupper(str[i]);
  return str;
}

std::string OnlineHistDBEnv::PagenameSyntax(std::string fullname, std::string &folder) {
  std::string outname;
  Statement *astmt=m_conn->createStatement
    ("begin :1 := OnlineHistDB.PagenameSyntax(theName => '"+fullname+"',Folder => :2); end;");
  try{
    astmt->registerOutParam(1,OCCISTRING,350);
    astmt->registerOutParam(2,OCCISTRING,300);
    astmt->execute();
    outname= astmt->getString(1);
    folder = astmt->getString(2);
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistPage::PagenameSyntax");
    }
  m_conn->terminateStatement (astmt); 
  return outname;
}
