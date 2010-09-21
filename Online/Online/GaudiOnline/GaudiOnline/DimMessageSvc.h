// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/DimMessageSvc.h,v 1.4 2010-09-21 14:28:24 frankb Exp $
#ifndef GAUDIONLINE_DIMMESSAGESVC_H
#define GAUDIONLINE_DIMMESSAGESVC_H

// Framework include files
#include "GaudiOnline/IErrorLogger.h"
#include "GaudiOnline/OnlineMessageSvc.h"

// Forward declarations
class DimService;

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {
  static const size_t SRCSIZE = 256;
  struct DimMessage  {
    int typ, size;
    char src[SRCSIZE];
    char msg[1];
  };
  class DimBuffer  {
  public:
    DimBuffer();
    virtual ~DimBuffer();
    void *makeBuffer (int typ, const std::string& src, const std::string& message);
    char *buffer ()   { return m_data.m_buffer;   }
    int   size()      { return m_size;            }

  private:
    union {
      DimMessage* m_msg;
      char*       m_buffer;
    } m_data;
    int   m_size;
    int   m_length;
  };

  /**@class DimMessageSvc DimMessageSvc.h
    *
    * Description: The DimMessageSvc service implements the IMessageSvc 
    *              interface and provides the forwarding of the messages
    *              to a DIM based error logger as used in the LHCb online 
    *              system.
    *
    * Author:      B.Jost / M.Frank
    */
  class DimMessageSvc : public OnlineMessageSvc,
                        virtual public IErrorLogger
  {
  public:
    /// Default constructor.
    DimMessageSvc(const std::string& name, ISvcLocator* svcloc);
    /// Destructor.
    virtual ~DimMessageSvc();
    /// IInterface implementation : queryInterface
    StatusCode queryInterface(const InterfaceID& riid, void** ppIf);
    /// Implementation of IService::initialize()
    virtual StatusCode initialize();
    /// Implementation of IService::finalize()
    virtual StatusCode finalize();
    /// Error logger implementation: report message
    virtual void report(int typ, const std::string& src, const std::string& msg);

  protected:
    /// Implementation of IMessageSvc::reportMessage()
    virtual void i_reportMessageEx(const Message& message, int lvl);

  private:
    /// Pointer to DIM service to pass messages
    DimService *m_logger;
    /// Reusable DIM message buffer
    DimBuffer   m_mess;
  };
}       // End namespace LHCb
#endif  // GAUDIONLINE_DIMMESSAGESVC_H
