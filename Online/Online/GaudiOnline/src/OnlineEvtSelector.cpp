// $Id: OnlineEvtSelector.cpp,v 1.7 2006-01-11 12:40:05 jost Exp $
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
//	Author     : M.Frank, B.Jost
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
#include "OnlineKernel/OnlineKernel/RTL/bits.h"
#include <list>

static const SvcFactory<GaudiOnline::EvtSelector> s_OnlineEvtSelectorFactory;
const IFactory& OnlineEvtSelectorFactory = s_OnlineEvtSelectorFactory;

namespace GaudiOnline  
{
  class BMEventHeader
  {
    public:
    int m_NumFrag;
    int m_ErrorMask;
    int m_NumMiss;
    int m_MissOffset;
    int offsets[1];
  };
  class BMEventDescriptor
  {
  public:
    BMEventHeader                   *m_EvHeader;
    int	                            m_evType;
    int                             m_PID;
    Bits::BitMask<4>                m_TrMask;
    int                             m_Size;
	void*                           m_MEPBuffPtr;
  };
  
  class OnlineContext : public IEvtSelector::Context 
  {
  public:
	  BMEventDescriptor *m_evdesc;
  private:
    const EvtSelector*              m_pSelector;
  public:
    /// Standard constructor
    OnlineContext(const EvtSelector* pSelector);
    /// Standard destructor 
    virtual ~OnlineContext();
    /// IEvtSelector::Context overload; context identifier
    virtual void* identifier() const {
      return NULL;
    }
    void setCriteria(const std::string& crit);
    /// Receive event and update communication structure
    StatusCode receiveEvent(StreamDescriptor* dsc);
    /// Set connection
    StatusCode connect();
    /// close connection
    void close();
  };
GaudiOnline::OnlineContext::OnlineContext(const EvtSelector* pSelector)
: m_pSelector(pSelector)
{
/*
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
*/
  }

/// Receive event and update communication structure
StatusCode GaudiOnline::OnlineContext::receiveEvent(StreamDescriptor* dsc)  {
  dsc->setLength(0);
/*
if ( m_accessDsc.ioDesc > 0 )  {
    int len = 0;
    dsc->setCurrentAccess(0);
    if ( StreamDescriptor::readLength(m_accessDsc,len) )  {
      if ( len > dsc->max_length() )  {
        dsc->allocate(len+4);
      }
      if ( StreamDescriptor::read(m_accessDsc,dsc->data(),len) )  {
        dsc->setLength(len);
        dsc->setCurrentAccess(&m_accessDsc);
        return StatusCode::SUCCESS;
      }
    }
  }
*/
  return StatusCode::FAILURE;
}

/// Set connection
StatusCode GaudiOnline::OnlineContext::connect()  {
//  switch(m_type)  {
//    case STREAM_FILE:
//    case STREAM_INET:
//    case STREAM_SFC:
//      m_accessDsc = StreamDescriptor::connect(m_conSpec);
//      return m_accessDsc.ioDesc > 0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
//    default:
//     break;
//  }
  return StatusCode::FAILURE;
}

/// close connection
void GaudiOnline::OnlineContext::close()  {
//  StreamDescriptor::close(m_accessDsc);
//  m_type = STREAM_NONE;
}


void GaudiOnline::OnlineContext::setCriteria(const std::string& crit) {
}

GaudiOnline::EvtSelector::EvtSelector(const std::string& nam, ISvcLocator* svcloc)
: Service( nam, svcloc), m_rootCLID(CLID_NULL), m_dataType(StreamDescriptor::NONE)
{
  declareProperty("CnvService",  m_cnvSvcName);
  declareProperty("NumBuffers",  m_numBuffers   = 2);
  declareProperty("LENGTH",      m_buffLen      = "8192");
  declareProperty("BMID",       m_BMIDs = "0");
  declareProperty("REQ1",       m_Rqs[0] = "");
  // Requirement format:
  // "EvType=x;TriggerMask=0xfeedbabe,0xdeadfeed,0xdeadbabe,0xdeadaffe;
  //  VetoMask=0x,0x,0x,0x;MaskType=ANY/ALL;UserType=USER/VIP/ONE;
  //  Frequency=MANY/PERC;Perc=20.5"
  declareProperty("REQ2",       m_Rqs[1] = "");
  declareProperty("REQ3",       m_Rqs[2] = "");
  declareProperty("REQ4",       m_Rqs[3] = "");
  declareProperty("REQ5",       m_Rqs[4] = "");
  declareProperty("REQ6",       m_Rqs[5] = "");
  declareProperty("REQ7",       m_Rqs[6] = "");
  declareProperty("REQ8",       m_Rqs[7] = "");
  declareProperty("PID",        m_PIDs = "1");
  
}
GaudiOnline::EvtSelector::~EvtSelector()
{
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
	    << endmsg;
    return status;
  }
  m_MyUTGID = getenv("UTGID");
  int pid;
  sscanf(m_PIDs.c_str(), "%x", &pid);
  m_MyConsumer = new MBM::Consumer(m_BMIDs, m_MyUTGID, pid);
  for (m_nreqs=0;m_nreqs<8;m_nreqs++)
  {
	if (m_Rqs[m_nreqs] != "")
	{
		m_Reqs[m_nreqs] = new BMRequirement(m_Rqs[m_nreqs]);
		m_Reqs[m_nreqs]->parse();
	}
	else
	{
		break;
	}
  }
  int i;
  for (i=0;i<m_nreqs;i++)
  {
	  BMRequirement *r=m_Reqs[i];
      m_MyConsumer->addRequest(r->evtype, r->trmask,r->vetomask,r->MaskType,
		  r->UserType,r->FreqType,r->freq);
  }

  m_rootCLID = eds->rootCLID();
  eds->release();
  m_dataType = StreamDescriptor::dataType(m_dataTypeName);
  log << MSG::DEBUG << "Selection CLID:" << m_rootCLID << endmsg;
  return status;
}

/// IService implementation: Service finalization
StatusCode GaudiOnline::EvtSelector::finalize()    {
  // Initialize base class
  MsgStream log(messageService(), name());
  int i;
  for (i=0;i<m_nreqs;i++)
  {
	  BMRequirement *r=m_Reqs[i];
      m_MyConsumer->delRequest(r->evtype, r->trmask,r->vetomask,r->MaskType,
		  r->UserType);
  }
  delete m_MyConsumer;
	m_MyConsumer = NULL;

  StatusCode status = Service::finalize();
  if ( !status.isSuccess() )    {
    log << MSG::ERROR << "Error initializing base class Service!" << endmsg;
  }
  return status;
}

StatusCode
GaudiOnline::EvtSelector::createContext(Context*& refpCtxt) const 
{
  OnlineContext* pCtxt = new OnlineContext(this);
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
	int status;
  OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(&ctxt);
  if ( pCtxt != 0 )
  {
	  status = m_MyConsumer->getEvent();
	  if (status == MBM_NORMAL)
	  {
		const MBM::EventDesc &ed = m_MyConsumer->event();
		GaudiOnline::BMEventDescriptor *d = pCtxt->m_evdesc;
		d->m_evType = ed.type;
		void *tm = &d->m_TrMask;
		memcpy(tm,ed.mask,sizeof(d->m_TrMask));
		d->m_Size = ed.len;
		d->m_EvHeader = (BMEventHeader*)ed.data;
		return StatusCode::SUCCESS;
	  }
  }
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::next(Context& ctxt, int jump) const 
{
  if ( jump == 0 ) 
  {
	  StatusCode status = next(ctxt);
      if ( !status.isSuccess() ) 
	  {
        return status;
      }
    return StatusCode::SUCCESS;
  }
  else
  {
	return StatusCode::FAILURE;
  }
}

StatusCode 
GaudiOnline::EvtSelector::previous(Context&  ctxt ) const 
{
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL 
      << " EventSelector Iterator, operator -- not supported " << endmsg;
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::previous(Context& ctxt, int jump) const 
{
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL 
      << " EventSelector Iterator, operator -- not supported " << endmsg;
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::rewind(Context& ctxt) const 
{
  MsgStream log(msgSvc(), name());
  log << MSG::FATAL 
      << " EventSelector Iterator, operation rewind not supported " << endmsg;
  return StatusCode::FAILURE;
}

StatusCode 
GaudiOnline::EvtSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const
{
  const OnlineContext* pctxt = dynamic_cast<const OnlineContext*>(&ctxt);
  if ( pctxt ) 
  {
	Address* pA = new Address(TEST_StorageType, m_rootCLID, pctxt->m_evdesc);
	pAddr = pA;
	return StatusCode::SUCCESS;
  }
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
    crit += "' LENGTH='" + m_buffLen;
    crit += "' TYPE='"   + m_dataTypeName+"'";
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


}