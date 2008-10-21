#include "ROLogger/MessageLine.h"
#include <iostream>
#include <cstring>

using namespace ROLogger;
using namespace std;

MessageLine::MessageLine(const MessageLine& c) 
: m_buff(c.m_buff), m_type(c.m_type), m_comp(c.m_comp), m_mess(c.m_mess)
{
}

int MessageLine::analyze() const {
  size_t idp, idq, idx = m_buff.find("(",NOD_START);
  if ( idx != string::npos && idx<32+NOD_START ) {
    m_utgid = idx+1;
    idq = m_buff.find("):",++idx);
    if ( idq != string::npos && idq>idx ) {
      idp = m_buff.find(":",idq+3);
      if ( idp != string::npos ) {
        m_mess = idp+2;
        m_comp = idq+3;
        m_type = idx-1;
        return m_type;
      }
    }
  }
  m_mess = m_buff.find(":",NOD_START);
  if ( m_mess != (int)string::npos ) m_mess += 2;
  return m_type=OTHER;
}

void MessageLine::dump()  const {
  cout << "Line:" << m_buff << endl
    << "  Type:" << type();
  switch(type()) {
  case NONE:    
    cout << "[NONE]";
    break;
  case OTHER:
    cout << "[OTHER]";
    break;
  default:
    cout << "[TASK]";
    break;
  }
  cout << " Severity:" << severity()
    << " Node:" << node();
  if ( type() > 0 ) {
    cout << " Utgid:" << utgid() << " Component:" << component();
  }
  cout << endl << "  Message:" << message() << endl;
}

/// Retrieve message severity from DIM logger message
int MessageLine::msgSeverity() const {
  return MessageLine::msgSeverity(m_buff.c_str());
}

/// Retrieve message severity from DIM logger message
int MessageLine::msgSeverity(const char* msg) {
  if ( msg ) {
    int len = ::strlen(msg);
    if      ( len>18 && !::strncmp(msg+12,"[VERB]",6)   ) return Msg_Verbose;
    else if ( len>19 && !::strncmp(msg+12,"[DEBUG]",7)  ) return Msg_Debug;
    else if ( len>18 && !::strncmp(msg+12,"[INFO]",6)   ) return Msg_Info;
    else if ( len>18 && !::strncmp(msg+12,"[WARN]",6)   ) return Msg_Warning;
    else if ( len>19 && !::strncmp(msg+12,"[ERROR]",7)  ) return Msg_Error;
    else if ( len>19 && !::strncmp(msg+12,"[FATAL]",7)  ) return Msg_Fatal;
    else if ( len>19 && !::strncmp(msg+12,"[ALWAYS]",8) ) return Msg_Always;
  }
  return 3;
}

/// Convert severity string to enum
int MessageLine::severityLevel(const string& severity) {
  switch(::toupper(severity[0])) {
  case 'V':
    return Msg_Verbose;
    break;
  case 'D':
    return Msg_Debug;
    break;
  case 'I':
    return Msg_Info;
    break;
  case 'W':
    return Msg_Warning;
    break;
  case 'E':
    return Msg_Error;
    break;
  case 'F':
    return Msg_Always;
    break;
  default:
    return Msg_Info;
  }
}


extern "C" int test_MessageLine(int,char**) {
  const char* lines[] = {
    "May08-133647[INFO] mona0803: Gaudi.exe(LHCb_MONA0803_MonEvents_00): StoreExplorerAlg: | | +--> /DAQ [Address: CLID=0x1 Type=0xa000000]  DataObject",
      "May08-133709[DEBUG]mona0804: LogGaudiSrv(/mona0804/LogGaudiSrv): main(): -- MARK --",
      "May08-133808[INFO] mona08: Hello"
  };
  for(size_t i=0; i<sizeof(lines)/sizeof(lines[0]);++i) {
    MessageLine line(lines[i]);
    line.dump();
    cout << endl;
  }
  return 1;
}

