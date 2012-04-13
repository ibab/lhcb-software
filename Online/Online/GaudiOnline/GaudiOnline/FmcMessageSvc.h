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
  class FmcMessageSvc : public OnlineMessageSvc, virtual public IErrorLogger  {
  public:
    /// Default constructor.
    FmcMessageSvc(const std::string& name, ISvcLocator* svcloc);

    /// Destructor.
    virtual ~FmcMessageSvc();

    /// IInterface implementation : queryInterface
    StatusCode queryInterface(const InterfaceID& riid,void** ppIf);

    /// Implementation of IService::initialize()
    virtual StatusCode initialize();

    /// Implementation of IService::start()
    virtual StatusCode start();

    /// Implementation of IService::restart()
    virtual StatusCode restart();

    /// Implementation of IService::finalize()
    virtual StatusCode finalize();

    /// Error logger implementation: report message
    virtual void report(int typ,const std::string& src,const std::string& msg);

    /// Number of dropped messages (in congestion-proof mode)
    virtual int getDroppedN();

    /// True if last messages was dropped (in congestion-proof mode)
    virtual bool isDropped();

    /// Implementation of IMessageSvc::ouputLevel()
    virtual int outputLevel()   const;

    /// Implementation of IMessageSvc::ouputLevel(source)
    virtual int outputLevel(const std::string& source)   const;


  protected:
    /// Implementation of IMessageSvc::reportMessage()
    virtual void i_reportMessageEx(const Message& message,int lvl);

  private:
    bool           m_noDrop;
    StringProperty m_fifoPath;
    int            m_tryN;
    int            fifoFD;
    int            dfltFifoFD;
    int droppedN;
    bool dropped;
    StatusCode openFifo();
    void changeFifo(Property& prop);
    const char* getPName();
    virtual int printM(int out,int severity,const char* fName,const char *format,...);
    virtual int dimLoggerMsgSend(char *buf,int fifoFD);
    virtual int stdErrMsgSend(char *buf);
    virtual int sysLogMsgSend(char *buf,int severity);
  };
  /***************************************************************************/
}       // End namespace LHCb
#endif  // GAUDIONLINE_FMCMESSAGESVC_H
