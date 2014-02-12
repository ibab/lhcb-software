#ifndef GAUDIONLINE_FMCMESSAGESVC_H
#define GAUDIONLINE_FMCMESSAGESVC_H
#include <string>

// Framework include files
#include "GaudiOnline/IErrorLogger.h"
#include "GaudiOnline/OnlineMessageSvc.h"


/*
 * LHCb namespace declaration
 */
namespace LHCb  {

  /***************************************************************************/
  class UpiMessageSvc : public OnlineMessageSvc, virtual public IErrorLogger  {
  public:
    /// Default constructor.
    UpiMessageSvc(const std::string& name, ISvcLocator* svcloc);

    /// Destructor.
    virtual ~UpiMessageSvc();

    /// IInterface implementation : queryInterface
    StatusCode queryInterface(const InterfaceID& riid,void** ppIf);

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

  private:
    StringProperty m_fifoPath;
  };
  /***************************************************************************/
}       // End namespace LHCb
#endif  // GAUDIONLINE_FMCMESSAGESVC_H

#include "GaudiKernel/SvcFactory.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,UpiMessageSvc)

extern "C" int upic_write_message(const char*, const char*);

/// Constructor
LHCb::UpiMessageSvc::UpiMessageSvc(const std::string& name,ISvcLocator* svcloc)
  : OnlineMessageSvc(name,svcloc)
{
  setErrorLogger(this);
}

/// Destructor.
LHCb::UpiMessageSvc::~UpiMessageSvc()   {
  setErrorLogger(0);
}

/// IInterface implementation : queryInterface
StatusCode LHCb::UpiMessageSvc::queryInterface(const InterfaceID& riid, void **ppIf)  {
  if( IErrorLogger::interfaceID().versionMatch(riid) )  {
    *ppIf=(IErrorLogger*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineMessageSvc::queryInterface(riid,ppIf);
}

/// Restart Service 
StatusCode LHCb::UpiMessageSvc::restart()  {
  return StatusCode::SUCCESS;
}

void LHCb::UpiMessageSvc::report(int typ,const std::string& src,const std::string& msg)
{
  if ( typ < m_outputLevel ) return;
  else if ( typ==MSG::ALWAYS && !m_printAlways ) return;
  else {
    char text[1024];
    const char *sl[8]={"[NIL]  ","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
		       "[FATAL]", "[ALWAYS]"};
    typ = (typ>=int(sizeof(sl)/sizeof(sl[0]))) ? (sizeof(sl)/sizeof(sl[0]))-1 : (typ<0 ? 0 : typ);
    ::snprintf(text,sizeof(text),"%s %s %s",sl[typ],src.c_str(),msg.c_str());
    ::upic_write_message(text,"");
  }
}

void LHCb::UpiMessageSvc::i_reportMessageEx(const Message& msg,int typ)  {
  int typM = msg.getType();
  if (  typM < m_outputLevel || typ < m_outputLevel ) return;
  if ( (typM==MSG::ALWAYS || typ==MSG::ALWAYS) && !m_printAlways) return;
  report(typM,msg.getSource(),msg.getMessage());
}

/// Implementation of IMessageSvc::ouputLevel()
int LHCb::UpiMessageSvc::outputLevel()   const {
  return m_outputLevel;
}

/// Implementation of IMessageSvc::ouputLevel(source)
int LHCb::UpiMessageSvc::outputLevel(const std::string& source) const {
  ThresholdMap::const_iterator it = m_thresholdMap.find(source);
  int lvl = (it == m_thresholdMap.end()) ? int(m_outputLevel) : (*it).second;
  if ( lvl > m_outputLevel ) return lvl;
  return m_outputLevel;
}
