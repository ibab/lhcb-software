// $Id: DimMessageSvc.cpp,v 1.10 2010-09-22 13:38:19 frankb Exp $
#include "GaudiKernel/Service.h"
#define NO_LONGLONG_TYPEDEF
#include "dis.hxx"
#include "RTL/rtl.h"
#include "GaudiOnline/DimMessageSvc.h"

LHCb::DimBuffer::DimBuffer()  {
  m_size = 0;
  m_length = 0;
  m_data.m_buffer = 0;
}

LHCb::DimBuffer::~DimBuffer()   {
  if (m_data.m_buffer != 0) free(m_data.m_buffer);
  m_data.m_buffer = 0;
  m_length = 0;
  m_size = 0;
}

void * LHCb::DimBuffer::makeBuffer (int typ, const std::string& src, const std::string& msg)
{
  size_t src_len = src.length() >= SRCSIZE ? SRCSIZE-1 : src.length();
  m_size = sizeof(DimMessage)+msg.length();
  if (m_length < m_size)  {
    m_data.m_buffer = (char*)::realloc(m_data.m_buffer,m_length=m_size);
  }
  DimMessage* m = m_data.m_msg;
  m->typ  = typ;
  m->size = msg.length();
  strncpy(m->src, src.c_str(), src_len);
  strncpy(m->msg, msg.c_str(), m->size);
  m->msg[m->size] = 0;
  m->src[src_len] = 0;
  return m_data.m_buffer;
}

/// Constructor
LHCb::DimMessageSvc::DimMessageSvc( const std::string& name, ISvcLocator* svcloc )
: OnlineMessageSvc( name, svcloc ), m_logger(0)
{
  setErrorLogger(this);
  std::cout << "============ DIM MessageService constructed:" << name << std::endl;
}

/// Destructor.
LHCb::DimMessageSvc::~DimMessageSvc() {
  setErrorLogger(0);
}

/// IInterface implementation : queryInterface
StatusCode LHCb::DimMessageSvc::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IErrorLogger::interfaceID().versionMatch(riid) )  {
    *ppIf = (IErrorLogger*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineMessageSvc::queryInterface(riid, ppIf);
}

/// Initialize Service
StatusCode LHCb::DimMessageSvc::initialize() {
  StatusCode sc = OnlineMessageSvc::initialize();
  if( sc.isFailure() ) return sc;
  std::string srvcname = /* "/" + RTL::nodeNameShort() + "/" + */ RTL::processName() + "/Output";
  m_mess.makeBuffer(MSG::INFO,srvcname,"DIM Service Starting up");
  m_logger = new DimService(srvcname.c_str(),(char*)"I:2;C:256;C",(void*)m_mess.buffer(),m_mess.size());
  return StatusCode::SUCCESS;
}

/// Finalize Service
StatusCode LHCb::DimMessageSvc::finalize() {
  if ( m_logger ) delete m_logger;
  m_logger = 0;
  return OnlineMessageSvc::finalize();
}

/// Error logger implementation: report message
void LHCb::DimMessageSvc::report(int typ, const std::string& src, const std::string& msg)  {
  m_mess.makeBuffer(typ,src,msg);
  if ( m_logger )  {
    m_logger->updateService(m_mess.buffer(),m_mess.size());
  }
}

/// Dispatch a message to the relevant streams.
void LHCb::DimMessageSvc::i_reportMessageEx(const Message& msg, int )    {
  i_reportMessage(msg);
  report(msg.getType(), msg.getSource(), msg.getMessage());
}

