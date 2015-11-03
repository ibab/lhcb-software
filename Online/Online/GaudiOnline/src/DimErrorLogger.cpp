// $Id: DimErrorLogger.cpp,v 1.18 2010-09-22 13:38:19 frankb Exp $
#include "GaudiKernel/Service.h"
#define NO_LONGLONG_TYPEDEF

#include "dis.hxx"
#include "GaudiOnline/DimMessageSvc.h"
#include "GaudiOnline/DimErrorLogger.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/Message.h"
#include "RTL/strdef.h"
#include "RTL/Lock.h"

// Instantiation of a static factory class used by clients to create instances of this service

namespace LHCb  {
  class DnsInfo : public DimInfo  {
    DimErrorLogger& m_logger;
    std::string     m_node;
    std::string     m_process;
    void getServiceNode(char* s, std::string& svc, std::string& node)  {
      char* at = strchr(s,'@');
      *at = 0;
      svc = s;
      node = at+1;
    }
    /// Add handler for a given message source
    void addHandler(const std::string& svc, const std::string& node) {
      if ( strncasecmp(m_node.c_str(),node.c_str(),m_node.length()) == 0 )  {
        if ( svc != "DIS_DNS" && svc != m_process )  {
          std::string s = /* "/"+node+"/"+ */ svc+"/Output";
          m_logger.addHandler(s);
        }
      }
      else {
        std::cout << "Ignore service:" << node << "::" << svc << std::endl;
      }
    }
    /// Remove handler for a given message source
    void removeHandler(const std::string& svc, const std::string& node) {
      if ( strncasecmp(m_node.c_str(),node.c_str(),m_node.length()) == 0 )  {
        std::string s = /* "/"+node+"/"+ */ svc+"/Output";
        m_logger.removeHandler(s);
      }
    }
    void infoHandler()  {
      char *msg = getString();
      std::string svc, node;
      switch(msg[0]) {
        case '+':
          getServiceNode(++msg,svc,node);
          addHandler(svc,node);
          break;
        case '-':
          getServiceNode(++msg,svc,node);
          removeHandler(svc,node);
          break;
        default:
          if ( *(int*)msg != *(int*)"DEAD" )  {
            char *at, *p = msg, *last = msg;
            while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
              last = strchr(at,'|');
              if ( last ) *last = 0;
              getServiceNode(p,svc,node);
              addHandler(svc,node);
              p = last+1;
            }
          }
          else  {
            std::cout << "DIM nameserver died...." << std::endl;
          }
          break;
      }
      //std::cout << "Received : " << getString() << std::endl;
    }
  public :
    DnsInfo(DimErrorLogger& l) : DimInfo((char*)"DIS_DNS/SERVER_LIST",(char*)"DEAD"), m_logger(l) {
      m_process = RTL::processName();
      m_node = "";//RTL::nodeNameShort();
    }
    virtual ~DnsInfo()  {}
  };
}


// Standard Constructor
LHCb::DimErrorLogger::DimErrorLogger(const std::string& nam, ISvcLocator* svcLoc)
: OnlineRunable(nam, svcLoc), m_dns(0)
{
  declareProperty("OutputFormat",          m_outputFmt  = "% F%20W%S%7W%R%T %0W%M");
  declareProperty("AcceptVerboseMessages", m_verboseMsg = false);
  declareProperty("AcceptDebugMessages",   m_debugMsg   = false);
  declareProperty("AcceptInfoMessages",    m_infoMsg    = false);
  declareProperty("AcceptWarningMessages", m_warningMsg = true);
  declareProperty("AcceptErrorMessages",   m_errorMsg   = true);
  declareProperty("AcceptFatalMessages",   m_fatalMsg   = true);
  declareProperty("AcceptSuccessMessages", m_successMsg = true);
  declareProperty("PrintService",          m_printSvc   = true);
  declareProperty("OutputMessage",         m_outputMsg  = false);
  declareProperty("MatchNodeName",         m_matchNode  = false);
  declareProperty("AcceptedSources",       m_acceptedSources);
  declareProperty("RefusedSources",        m_refusedSources);
  declareProperty("AcceptedClients",       m_acceptedClients);
  declareProperty("RefusedClients",        m_refusedClients);
  declareProperty("RefusedMessages",       m_refusedMessages);
  m_acceptedSources.push_back("*");
  m_acceptedClients.push_back("*");
}

// Standard Destructor
LHCb::DimErrorLogger::~DimErrorLogger()
{
}

// IInterface implementation : queryInterface
StatusCode LHCb::DimErrorLogger::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IErrorLogger::interfaceID().versionMatch(riid) )
    *ppIf = (IErrorLogger*)this;
  else
    return OnlineRunable::queryInterface(riid, ppIf);
  addRef();
  return StatusCode::SUCCESS;
}

// Add handler for a given message source
void LHCb::DimErrorLogger::addHandler(const std::string& nam)    {
  const std::string node = RTL::nodeNameShort();
  if ( m_matchNode && !::str_match_wild(nam.c_str(),std::string("*"+node+"*").c_str()) ) {
    report(MSG::DEBUG, RTL::nodeNameShort()+"::"+name(), node+"> Messages from client "+nam+" denied.");
    return;
  }
  std::vector<std::string>::iterator i=m_refusedClients.begin();
  for(; i != m_refusedClients.end(); ++i)  {
    if ( ::str_match_wild(nam.c_str(), (*i).c_str()) )  {
      report(MSG::DEBUG, RTL::nodeNameShort()+"::"+name(), node+"> Messages from client "+nam+" denied.");
      return;
    }
  }
  std::vector<std::string>::iterator j=m_acceptedClients.begin();
  for(; j != m_acceptedClients.end(); ++j)  {
    if ( ::str_match_wild(nam.c_str(), (*j).c_str()) )  {
      dim_lock();
      Clients::iterator ic=m_clients.find(nam);
      if ( ic == m_clients.end() )  {
        char def[32];
        memset(def,0,sizeof(def));
        DimInfo* info = new DimInfo(nam.c_str(),(void*)def,sizeof(def),this);
        // std::cout << "Create DimInfo:" << (void*)info << std::endl;
        m_clients.insert(std::make_pair(nam, info));
        report(MSG::INFO, RTL::nodeNameShort()+"::"+name(), node+"> Added error handler for:"+nam);
	dim_unlock();
        return;
      }
      dim_unlock();
    }
  }
}

// Remove handler for a given message source
void LHCb::DimErrorLogger::removeHandler(const std::string& nam)   {
  dim_lock();
  Clients::iterator i=m_clients.find(nam);
  if ( i != m_clients.end() ) {
    // std::cout << "Delete DimInfo:" << (void*)(*i).second << std::endl;
    delete (*i).second;
    m_clients.erase(i);
    report(MSG::INFO, RTL::nodeNameShort()+"::"+name(), RTL::nodeNameShort()+"> Removed error handler for:"+nam);
  }
  dim_unlock();
}

// IService implementation: initialize the service
StatusCode LHCb::DimErrorLogger::initialize()   {
  StatusCode sc = OnlineRunable::initialize();
  if ( !sc.isSuccess() )     {
    msgSvc()->reportMessage(name(), MSG::ERROR, "Failed to initialize service base class.");
    return sc;
  }
  m_dns = new DnsInfo(*this);
  return sc;
}

// IService implementation: finalize the service
StatusCode LHCb::DimErrorLogger::finalize()     {
  dim_lock();
  for (Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    delete (*i).second;
  }
  m_clients.clear();
  dim_unlock();
  if ( m_dns ) delete m_dns;
  m_dns = 0;
  return OnlineRunable::finalize();
}

// IRunable implementation : Run the class implementation
StatusCode LHCb::DimErrorLogger::run()   {
  return OnlineRunable::run();
}

void LHCb::DimErrorLogger::reportMessage(int typ, const std::string& src, const std::string& msg)  {
  msgSvc()->reportMessage(src, typ, msg);
  if ( m_outputMsg ) {
    Message m(src,typ,msg);
    m.setFormat(m_outputFmt);
    std::cout << "Log:" << m << std::endl;
  }
}

void LHCb::DimErrorLogger::report(int typ, const std::string& src, const std::string& msg)  {
  switch(typ)  {
    case MSG::WARNING:
      if ( !m_warningMsg  ) return;
      break;
    case MSG::ERROR:
      if ( !m_errorMsg    ) return;
      break;
    case MSG::FATAL:
      if ( !m_fatalMsg    ) return;
      break;
    case MSG::VERBOSE:
      if ( !m_verboseMsg  ) return;
      break;
    case MSG::DEBUG:
      if ( !m_debugMsg    ) return;
      break;
    case MSG::INFO:
      if ( !m_infoMsg     ) return;
      break;
    case MSG::ALWAYS:
      if ( !m_successMsg  ) return;
      break;
    default:
      break;
  }
  std::vector<std::string>::iterator i=m_refusedSources.begin();
  for(; i != m_refusedSources.end(); ++i)  {
    if ( ::str_match_wild(src.c_str(), (*i).c_str()) )  {
      return;
    }
  }
  i=m_acceptedSources.begin();
  for(; i != m_acceptedSources.end(); ++i)  {
    if ( ::str_match_wild(src.c_str(), (*i).c_str()) )  {
      // Check if this message should be refused generally....
      i=m_refusedMessages.begin();
      for(; i != m_refusedMessages.end(); ++i)  {
        if ( ::str_match_wild(msg.c_str(), (*i).c_str()) )  {
          return;
        }
      }
      reportMessage(typ,src,msg);
      return;
    }
  }
}

void LHCb::DimErrorLogger::infoHandler() {
  const char* info = getInfo()->getName();
  if ( info ) {
    static const std::string node = RTL::nodeNameShort();
    static const std::string match = "*"+RTL::nodeNameShort()+"*";
    DimMessage* m = (DimMessage*)getInfo()->getData();
    if ( m_matchNode && !::str_match_wild(info,match.c_str()) ) {
      report(MSG::DEBUG, RTL::nodeNameShort()+"::"+name(), node+"> Spurious message from client "+std::string(info)+" received.");
      return;
    }
    std::vector<std::string>::iterator i=m_refusedClients.begin();
    for(; i != m_refusedClients.end(); ++i)  {
      if ( ::str_match_wild(info, (*i).c_str()) )  {
	report(MSG::DEBUG, RTL::nodeNameShort()+"::"+name(), node+"> Spurious message from client "+std::string(info)+" received.");
        return;
      }
    }
    if ( m->size > 0 )  {
      std::string task;
      if ( m_printSvc )  {
        task = info;
        size_t idx = task.find_last_of("/");
        task = task.substr(0,idx);
        m->msg[m->size] = 0;
        task += "::";
      }
      task += m->src;
      report(m->typ, task, m->msg);
    }
  }
}
