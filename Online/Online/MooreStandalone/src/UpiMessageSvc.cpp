#ifndef GAUDIONLINE_GENERICMESSAGESVC_H
#define GAUDIONLINE_GENERICMESSAGESVC_H
#include <string>

// Framework include files
#include "GaudiOnline/IErrorLogger.h"
#include "GaudiOnline/OnlineMessageSvc.h"


/*
 * LHCb namespace declaration
 */
namespace LHCb  {

  class GenericMessageSvc : public OnlineMessageSvc, virtual public IErrorLogger  {
  public:
    /// Default constructor.
    GenericMessageSvc(const std::string& name, ISvcLocator* svcloc);

    /// Destructor.
    virtual ~GenericMessageSvc();

    /// IInterface implementation : queryInterface
    StatusCode queryInterface(const InterfaceID& riid,void** ppIf);

    /// Implementation of IService::initialize()
    virtual StatusCode initialize();

    /// Implementation of IService::restart()
    virtual StatusCode restart();

    /// Error logger implementation: report message
    virtual void report(int typ,const std::string& src,const std::string& msg);

    /// Implementation of IMessageSvc::ouputLevel()
    virtual int outputLevel()   const;

    /// Implementation of IMessageSvc::ouputLevel(source)
    virtual int outputLevel(const std::string& source)   const;

  protected:
    /// Implementation of IMessageSvc::reportMessage()
    virtual void i_reportMessageEx(const Message& message,int lvl);

    typedef int(*func_t)(const char*);
    std::string m_logFuncName;
    func_t m_logFunc;
  };
}       // End namespace LHCb
#endif  // GAUDIONLINE_GENERICMESSAGESVC_H

#include "GaudiKernel/SvcFactory.h"
#include <iostream>
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,GenericMessageSvc)

using namespace std;
using namespace LHCb;

/// Constructor
GenericMessageSvc::GenericMessageSvc(const string& name,ISvcLocator* svcloc)
  : OnlineMessageSvc(name,svcloc)
{
  setErrorLogger(this);
  declareProperty("LoggerFunction",m_logFuncName);
}

/// Destructor.
GenericMessageSvc::~GenericMessageSvc()   {
  setErrorLogger(0);
}

/// IInterface implementation : queryInterface
StatusCode GenericMessageSvc::queryInterface(const InterfaceID& riid, void **ppIf)  {
  if( IErrorLogger::interfaceID().versionMatch(riid) )  {
    *ppIf=(IErrorLogger*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineMessageSvc::queryInterface(riid,ppIf);
}

/// Implementation of IService::initialize()
StatusCode GenericMessageSvc::initialize()   {
  StatusCode sc = OnlineMessageSvc::initialize();
  m_logFunc = 0;
  if ( 1 != System::getProcedureByName(0,m_logFuncName,(System::EntryPoint*)&m_logFunc) )  {
    cout << "Failed to load logger function:" << m_logFuncName << endl;
    return StatusCode::FAILURE;
  }
  report(MSG::ALWAYS,name(),"Successfully resolved logger function:"+m_logFuncName);
  return sc;
}

/// Restart Service 
StatusCode GenericMessageSvc::restart()  {
  return StatusCode::SUCCESS;
}

void GenericMessageSvc::report(int typ,const string& src,const string& msg)   {
  if ( typ < m_outputLevel ) return;
  else if ( typ==MSG::ALWAYS && !m_printAlways ) return;
  else {
    char text[1024];
    const char *sl[8]={"[NIL]  ","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
		       "[FATAL]", "[ALWAYS]"};
    typ = (typ>=int(sizeof(sl)/sizeof(sl[0]))) ? (sizeof(sl)/sizeof(sl[0]))-1 : (typ<0 ? 0 : typ);
    ::snprintf(text,sizeof(text),"%s %s %s",sl[typ],src.c_str(),msg.c_str());
    if ( m_logFunc )  {
      (*m_logFunc)(text);
      return;
    }
    throw runtime_error("Failed to access logger function:"+m_logFuncName);
  }
}

void GenericMessageSvc::i_reportMessageEx(const Message& msg,int typ)  {
  int typM = msg.getType();
  if (  typM < m_outputLevel || typ < m_outputLevel ) return;
  if ( (typM==MSG::ALWAYS || typ==MSG::ALWAYS) && !m_printAlways) return;
  report(typM,msg.getSource(),msg.getMessage());
}

/// Implementation of IMessageSvc::ouputLevel()
int GenericMessageSvc::outputLevel()   const {
  return m_outputLevel;
}

/// Implementation of IMessageSvc::ouputLevel(source)
int GenericMessageSvc::outputLevel(const string& source) const {
  ThresholdMap::const_iterator it = m_thresholdMap.find(source);
  int lvl = (it == m_thresholdMap.end()) ? int(m_outputLevel) : (*it).second;
  if ( lvl > m_outputLevel ) return lvl;
  return m_outputLevel;
}
