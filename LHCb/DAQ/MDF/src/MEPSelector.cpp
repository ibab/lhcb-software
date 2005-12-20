// $Id: MEPSelector.cpp,v 1.1 2005-12-20 16:33:39 frankb Exp $
//====================================================================
//	OnlineMDFEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : MDF
//
//  Description: The LHCb::MEPSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//	Author     : M.Frank
//====================================================================

// Include files
#include "MDF/StreamDescriptor.h"
#include "MDF/StorageTypes.h"
#include "MDF/MEPSelector.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "MDF/MDFHeader.h"

using LHCb::MDFHeader;
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPSelector);

namespace LHCb  {
  /** @class MEPContext
  *
  *  @author  M.Frank
  *  @version 1.0
  */
  class MEPContext : public IEvtSelector::Context {
  public:
    enum StreamType { 
      STREAM_NONE, 
      STREAM_FILE, 
      STREAM_INET,
    };
  private:
    typedef StreamDescriptor  DSC;
    const MEPSelector*        m_sel;
    int                       m_total;
    int                       m_current;
    DSC                       m_descriptor;
    StreamType                m_type;
    std::string               m_conSpec;
    DSC::Access               m_bindDsc;
    DSC::Access               m_accessDsc;
  public:
    /// Standard constructor
    MEPContext(const MEPSelector* pSelector)
    : m_sel(pSelector), m_type(STREAM_NONE)   {     }
    /// Standard destructor 
    virtual ~MEPContext()          { close();             }
    const DSC& descriptor() const  { return m_descriptor; }
    //DSC& descriptor()              { return m_descriptor; }
    /// IMDFEvtSelector::Context overload; context identifier
    virtual void* identifier() const { return (void*)m_sel; }
    void setCriteria(const std::string& crit);
    /// Receive event and update communication structure
    StatusCode receiveEvent();
    /// Set connection
    StatusCode connect();
    /// close connection
    void close()    {
      DSC::close(m_accessDsc);
      DSC::close(m_bindDsc);
    }
    /// Connection specification
    const std::string& specs() const  {
      return m_conSpec;
    }
  };
}

/// Receive event and update communication structure
StatusCode LHCb::MEPContext::receiveEvent()  {
  m_descriptor.setLength(0);
  if ( m_accessDsc.ioDesc > 0 )  {
    MDFHeader h;
    int len = 0;
    if ( StreamDescriptor::read(m_accessDsc,&h,sizeof(MDFHeader)) )  {
      if ( h.size()+sizeof(MDFHeader) > size_t(m_descriptor.max_length()) )  {
        m_descriptor.allocate(sizeof(MDFHeader) + size_t(h.size()*1.5));
      }
      MDFHeader* hdr = (MDFHeader*)m_descriptor.data();
      *hdr = h;
      if ( StreamDescriptor::read(m_accessDsc,hdr+1,h.size()) )  {
        m_descriptor.setLength(h.size()+sizeof(MDFHeader));
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// Set connection
StatusCode LHCb::MEPContext::connect()  {
  switch(m_type)  {
    case STREAM_FILE:
    case STREAM_INET:
      m_bindDsc = StreamDescriptor::bind(m_conSpec);
      if ( m_bindDsc.ioDesc > 0 )   {
        m_accessDsc = StreamDescriptor::accept(m_bindDsc);
        return m_accessDsc.ioDesc > 0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
      break;
    default:
      break;
  }
  return StatusCode::FAILURE;
}

void LHCb::MEPContext::setCriteria(const std::string& crit) {
  Tokenizer tok(true);
  std::string recl;
  int buff_len = 10*1024;
  tok.analyse(crit," ","","","=","'","'");
  for(Tokenizer::Items::iterator i=tok.items().begin(); i!=tok.items().end();i++) {
    std::string tmp = (*i).tag().substr(0,3);
    std::string val = (*i).value();
    switch(::toupper(tmp[0])) {
      case 'L': // LENGTH='1024'
        ::sscanf((*i).value().c_str(), "%d", &buff_len);
        break;
      case 'D': // DATA='...'
        m_conSpec = val;
        switch(::toupper(m_conSpec[0])) {
          case 'F':          //  DATA='file://C:/Data/myfile.dat'
            m_type = STREAM_FILE;
            break;
          case 'I':          //  DATA='ip://137.138.142.82:8000'
            m_type = STREAM_INET;
            break;
          default:
            m_type = STREAM_NONE;
            break;
        }
        break;
    }
  }
  m_descriptor.allocate(sizeof(MDFHeader) + buff_len);
}

LHCb::MEPSelector::MEPSelector(const std::string& nam, ISvcLocator* svcloc)
: Service( nam, svcloc), m_rootCLID(CLID_NULL)
{
  declareProperty("CnvService",  m_cnvSvcName);
  declareProperty("LENGTH",      m_buffLen = "700000");
}

// IInterface::queryInterface
StatusCode LHCb::MEPSelector::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IID_IEvtSelector )  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface( riid, ppvIf );
}

/// IService implementation: Db event selector override
StatusCode LHCb::MEPSelector::initialize()    {
  // Initialize base class
  StatusCode status = Service::initialize();
  MsgStream log(messageService(), name());
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "Error initializing base class Service!" << endreq;
    return status;
  }
  // Get DataSvc
  IDataManagerSvc* eds = 0;
  status = serviceLocator()->service("EventDataSvc", eds);
  if( !status.isSuccess() ) {
    log << MSG::ERROR << "Cannot get IID_IDataManagerSvc from EventDataSvc" << endmsg;
    return status;
  }
  m_rootCLID = eds->rootCLID();
  eds->release();
  log << MSG::DEBUG << "Selection CLID:" << m_rootCLID << endmsg;
  return status;
}

StatusCode LHCb::MEPSelector::createContext(Context*& refpCtxt) const {
  refpCtxt = new MEPContext(this);
  return StatusCode::SUCCESS;
}

StatusCode LHCb::MEPSelector::next(Context& ctxt) const   {
  MEPContext* pCtxt = dynamic_cast<MEPContext*>(&ctxt);
  if ( pCtxt != 0 )   {
    StatusCode sc = pCtxt->receiveEvent();
    if ( !sc.isSuccess() ) {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Failed to receieve the next event." << endmsg;
    }
    return sc;
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::MEPSelector::next(Context& ctxt, int jump) const   {
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

StatusCode LHCb::MEPSelector::previous(Context& /* ctxt */) const  {
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL << " EventSelector Iterator, operator -- not supported " << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::MEPSelector::previous(Context& ctxt, int jump) const   {
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
LHCb::MEPSelector::createAddress(const Context& ctxt, 
                                 IOpaqueAddress*& pAddr) const    {
  const MEPContext* pctxt = dynamic_cast<const MEPContext*>(&ctxt);
  if ( pctxt ) {
    const StreamDescriptor& dsc = pctxt->descriptor();
    if ( dsc.hasData() )  {
      unsigned long p1 = (unsigned long)dsc.data();
      unsigned long p2 = (unsigned long)&dsc;
      pAddr = new GenericAddress(MDF_StorageType,m_rootCLID,pctxt->specs(),"",p1,p2);
      return StatusCode::SUCCESS;
    }
  }
  pAddr = 0;
  return StatusCode::FAILURE;
}

StatusCode LHCb::MEPSelector::releaseContext(Context*& ctxt) const  {
  MEPContext* pCtxt = dynamic_cast<MEPContext*>(ctxt);
  if ( pCtxt ) {
    pCtxt->close();
    delete pCtxt;
    pCtxt = 0;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode 
LHCb::MEPSelector::resetCriteria(const std::string& criteria,
                                 Context& context)  const
{
  MsgStream log(messageService(), name());
  std::string crit = criteria;
  MEPContext* ctxt = dynamic_cast<MEPContext*>(&context);
  if ( ctxt )  {
    ctxt->close();
    crit.replace(0,5,"DATA='");
    crit += "' LENGTH='" + m_buffLen;
    ctxt->setCriteria(crit);
    StatusCode sc = ctxt->connect();
    if ( !sc.isSuccess() )  {
      log << MSG::ERROR << "Failed to connect to:" << crit << endmsg;
    }
    return sc;
  }
  log << MSG::ERROR << "Invalid iteration context." << endmsg;
  return StatusCode::FAILURE;
}
