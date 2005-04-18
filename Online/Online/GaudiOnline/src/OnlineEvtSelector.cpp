// $Id: OnlineEvtSelector.cpp,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
//====================================================================
//	OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : GaudiOnline
//
//  Description: The GaudiOnline::EvtSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "GaudiOnline/OnlineAddress.h"
#include "GaudiOnline/StreamDescriptor.h"
#include "GaudiOnline/OnlineEvtSelector.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include <list>

static const SvcFactory<GaudiOnline::EvtSelector> s_OnlineEvtSelectorFactory;
const IFactory& OnlineEvtSelectorFactory = s_OnlineEvtSelectorFactory;

namespace GaudiOnline  {
  /** @class OnlineContext
  *
  *  @author  M.Frank
  *  @version 1.0
  */
  class OnlineContext : public IEvtSelector::Context {
  public:
    typedef std::list<StreamDescriptor*> ListDsc;
    enum StreamType { 
      STREAM_NONE, 
      STREAM_FILE, 
      STREAM_INET,
      STREAM_SFC
    };
  private:
    const EvtSelector*              m_pSelector;
    mutable ListDsc                 m_com;
    mutable ListDsc::iterator       m_recvDesc;
    mutable ListDsc::const_iterator m_readDesc;
    StreamType                      m_type;
    std::string                     m_conSpec;
    StreamDescriptor::Access        m_accessDsc;
  public:
    /// Standard constructor
    OnlineContext(size_t len, const EvtSelector* pSelector);
    /// Standard destructor 
    virtual ~OnlineContext();
    const StreamDescriptor* nextReadDescriptor() const  {
      if ( ++m_readDesc==m_com.end() )  {
        m_readDesc = m_com.begin();
      }
      return *m_readDesc;
    }
    StreamDescriptor* nextReceiveDescriptor() const  {
      if ( ++m_recvDesc==m_com.end() )  {
        m_recvDesc = m_com.begin();
      }
      return *m_recvDesc;
    }
    /// IEvtSelector::Context overload; context identifier
    virtual void* identifier() const {
      return (void*)m_pSelector;
    }
    void setCriteria(const std::string& crit);
    /// Receive event and update communication structure
    StatusCode receiveEvent(StreamDescriptor* dsc);
    /// Set connection
    StatusCode connect();
    /// close connection
    void close();
  };
}

GaudiOnline::OnlineContext::OnlineContext(size_t len, const EvtSelector* pSelector)
  : m_pSelector(pSelector), m_type(STREAM_NONE)
{
  m_com.resize(len);
  for (ListDsc::iterator i=m_com.begin(); i != m_com.end(); ++i)  {
    (*i) = new StreamDescriptor();
  }
  m_recvDesc = m_com.begin();
  m_readDesc = m_com.begin();
}

GaudiOnline::OnlineContext::~OnlineContext() {
  for (ListDsc::iterator i=m_com.begin(); i != m_com.end(); ++i)  {
    delete (*i);
    (*i) = 0;
  }
  m_com.clear();
  m_recvDesc = m_com.begin();
  m_readDesc = m_com.begin();
  close();
}

/// Receive event and update communication structure
StatusCode GaudiOnline::OnlineContext::receiveEvent(StreamDescriptor* dsc)  {
  dsc->setLength(0);
  if ( m_accessDsc.ioDesc > 0 )  {
    int len = 0;
    if ( StreamDescriptor::readLength(m_accessDsc,len) )  {
      if ( len > dsc->max_length() )  {
        dsc->allocate(len+4);
      }
      if ( StreamDescriptor::read(m_accessDsc,dsc->data(),len) )  {
        dsc->setLength(len);
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// Set connection
StatusCode GaudiOnline::OnlineContext::connect()  {
  switch(m_type)  {
    case STREAM_FILE:
    case STREAM_INET:
    case STREAM_SFC:
      m_accessDsc = StreamDescriptor::connect(m_conSpec);
      return m_accessDsc.ioDesc > 0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

/// close connection
void GaudiOnline::OnlineContext::close()  {
  StreamDescriptor::close(m_accessDsc);
  m_type = STREAM_NONE;
}


void GaudiOnline::OnlineContext::setCriteria(const std::string& crit) {
  Tokenizer tok(true);
  std::string recl;
  int buff_len = 10*1024;
  tok.analyse(crit," ","","","=","'","'");
  for(Tokenizer::Items::iterator i=tok.items().begin(); i!=tok.items().end();i++) {
    std::string tmp = (*i).tag().substr(0,3);
    switch(::toupper(tmp[0])) {
      case 'T': // TYPE='DAQ'
        if ( (*i).value() == 'DAQ' || (*i).value() == 'L2' || (*i).value()=='L3' )
          m_dataType = StreamDescriptor::DAQ_BUFFER;
        else if ( (*i).value() == 'L1' )
          m_dataType = StreamDescriptor::L1_BUFFER;
        break;
      case 'L': // LENGTH='1024'
        ::sscanf((*i).value().c_str(), "%d", &buff_len);
        break;
      case 'D': // DATA='...'
        m_conSpec = (*i).value();
        switch(::toupper(m_conSpec[0])) {
          case 'F':          //  DATA='file://C:/Data/myfile.dat'
            m_type = STREAM_FILE;
            break;
          case 'I':          //  DATA='ip://137.138.142.82:8000'
            m_type = STREAM_INET;
            break;
          case 'S':          //  DATA='sfc://137.138.142.82:8000'
            m_type = STREAM_SFC;
            break;
          default:
            m_type = STREAM_NONE;
            break;
        }
        break;
    }
  }
  for (ListDsc::iterator i=m_com.begin(); i != m_com.end(); ++i)  {
    (*i)->allocate(buff_len);
    (*i)->setType(m_dataType);
  }
}

GaudiOnline::EvtSelector::EvtSelector(const std::string& nam, ISvcLocator* svcloc)
: Service( nam, svcloc), m_rootCLID(CLID_NULL), m_dataType(StreamBuffer::NONE)
{
  declareProperty("CnvService",  m_cnvSvcName);
  declareProperty("NumBuffers",  m_numBuffers   = 2);
  declareProperty("LENGTH",      m_buffLen      = "8192");
  declareProperty("TYPE",        m_dataTypeName = "L1");
}

GaudiOnline::EvtSelector::~EvtSelector() {
}

// IInterface::queryInterface
StatusCode GaudiOnline::EvtSelector::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IID_IEvtSelector )  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface( riid, ppvIf );
}

/// IService implementation: Db event selector override
StatusCode GaudiOnline::EvtSelector::initialize()    {
  // Initialize base class
  StatusCode status = Service::initialize();
  MsgStream log(messageService(), name());
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "Error initializing base class Service!" << endreq;
    return status;
  }
  // Retrieve conversion service handling event iteration
  //status = serviceLocator()->service(m_cnvSvcName, m_dbMgr);
  if( !status.isSuccess() ) {
    log << MSG::ERROR 
        << "Unable to localize interface IID_IOnlineMgr from service:" 
        << m_cnvSvcName << endreq;
    return status;
  }
  // Get DataSvc
  IDataManagerSvc* eds = 0;
  status = serviceLocator()->service("EventDataSvc", eds);
  if( !status.isSuccess() ) {
    log << MSG::ERROR 
      << "Unable to localize interface IID_IDataManagerSvc "
      << "from service EventDataSvc"
	    << endreq;
    return status;
  }
  m_rootCLID = eds->rootCLID();
  eds->release();
  if ( m_dataTypeName == 'DAQ' || m_dataTypeName == 'L2' || m_dataTypeName=='L3' )
    m_dataType = StreamDescriptor::DAQ_BUFFER;
  else if ( m_dataTypeName == 'L1' )
    m_dataType = StreamDescriptor::L1_BUFFER;
  log << MSG::DEBUG << "Selection CLID:" << m_rootCLID 
    << "  Data type:" << m_dataTypeName << "(" << m_dataType << ")" << endreq;
  return status;
}

/// IService implementation: Service finalization
StatusCode GaudiOnline::EvtSelector::finalize()    {
  // Initialize base class
  MsgStream log(messageService(), name());
  StatusCode status = Service::finalize();
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "Error initializing base class Service!" << endreq;
  }
  return status;
}

StatusCode
GaudiOnline::EvtSelector::createContext(Context*& refpCtxt) const 
{
  OnlineContext* pCtxt = new OnlineContext(m_numBuffers, this);
  refpCtxt = pCtxt;
  return StatusCode::SUCCESS;
}

/// Access last item in the iteration
StatusCode GaudiOnline::EvtSelector::last(Context& /*refContext*/) const  {
  return StatusCode::FAILURE;
}

StatusCode
GaudiOnline::EvtSelector::next(Context& ctxt) const 
{
  OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(&ctxt);
  if ( pCtxt != 0 )   {
    StreamDescriptor* dsc = pCtxt->nextReceiveDescriptor();
    return pCtxt->receiveEvent(dsc);
  }
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::next(Context& ctxt, int jump) const 
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = next(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::previous(Context& /* ctxt */) const 
{
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL 
      << " EventSelector Iterator, operator -- not supported " << endreq;
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::previous(Context& ctxt, int jump) const 
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = previous(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::rewind(Context& ctxt) const 
{
  OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(&ctxt);
  if ( pCtxt ) {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const
{
  const OnlineContext* pctxt = dynamic_cast<const OnlineContext*>(&ctxt);
  if ( pctxt ) {
    const StreamDescriptor* dsc = pctxt->nextReadDescriptor();
    if ( dsc->hasData() )  {
      Address* pA = new Address(TEST_StorageType, m_rootCLID, dsc);
      pAddr = pA;
      return StatusCode::SUCCESS;
    }
  }
  pAddr = 0;
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::releaseContext(Context*& ctxt) const
{
  OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(ctxt);
  if ( pCtxt ) {
    pCtxt->close();
    delete pCtxt;
    pCtxt = 0;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::resetCriteria(const std::string& criteria,
                                 Context& context)  const
{
  MsgStream log(messageService(), name());
  std::string crit = criteria;
  OnlineContext* ctxt = dynamic_cast<OnlineContext*>(&context);
  if ( ctxt )  {
    ctxt->close();
    crit.replace(0,5,"DATA='");
    crit += "' LENGTH='"+m_buffLen+"'";
    ctxt->setCriteria(crit);
    StatusCode sc = ctxt->connect();
    if ( !sc.isSuccess() )  {
      log << MSG::ERROR << "Failed to connect to:" << crit << endreq;
    }
    return sc;
  }
  log << MSG::ERROR << "Invalid iteration context." << endreq;
  return StatusCode::FAILURE;
}

