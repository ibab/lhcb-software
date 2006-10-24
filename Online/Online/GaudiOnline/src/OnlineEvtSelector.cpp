// $Id: OnlineEvtSelector.cpp,v 1.18 2006-10-24 11:25:11 frankb Exp $
//====================================================================
//	OnlineEvtSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : GaudiOnline
//
//  Description: The GaudiOnline::OnlineEvtSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//	Author     : M.Frank, B.Jost
//====================================================================

// Include files
#include "GaudiOnline/OnlineEvtSelector.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "MBM/mepdef.h"
#include "MBM/Consumer.h"
#include "MDF/StorageTypes.h"
#include "MDF/RawDataAddress.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawEventDescriptor.h"

#include "GaudiKernel/SvcFactory.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineEvtSelector)

using LHCb::RawBank;
using LHCb::MEPFragment;
using LHCb::RawEventDescriptor;

namespace LHCb  {
  class OnlineContext : public IEvtSelector::Context   {
  public:
    std::vector<RawBank*>    m_banks;
    RawEventDescriptor       m_evdesc;
  private:
    const OnlineEvtSelector* m_sel;
    MBM::Consumer*           m_consumer;
    void*                    m_mepStart;
    bool                     m_needFree;
  public:
    /// Standard constructor
    OnlineContext(const OnlineEvtSelector* pSelector)
    : m_sel(pSelector), m_consumer(0), m_mepStart(0), m_needFree(false) {}
    /// Standard destructor 
    virtual ~OnlineContext() {}
    /// IEvtSelector::Context overload; context identifier
    virtual void* identifier() const                     { return (void*)m_sel; }
    virtual const std::vector<RawBank*>& banks() const   { return m_banks;      }
    const RawEventDescriptor& descriptor() const         { return m_evdesc;     }
    StatusCode receiveEvent()  {
      m_banks.clear();
      if ( m_consumer )  {
        if ( m_needFree )   {
          try  {
            m_consumer->freeEvent();
          } catch(...) {
          }
        }
        try  {
          if ( m_consumer->getEvent() == MBM_NORMAL )  {
            const MBM::EventDesc& e = m_consumer->event();
            m_evdesc.setPartitionID(m_consumer->partitionID());
            m_evdesc.setTriggerMask(e.mask);
            m_evdesc.setEventType(e.type);
            m_evdesc.setHeader(e.data);
            m_evdesc.setSize(e.len);
            m_needFree = true;
            if ( m_sel->m_decode && e.type == EVENT_TYPE_EVENT )  {
              m_evdesc.setMepBuffer(m_mepStart);
              for(int i=0, n=m_evdesc.numberOfFragments(); i<n; ++i)  {
                // LHCb::MEPFragment* f = m_evdesc.fragment(i);
                // int off = int(int(f)-int(m_mepStart));
                // printf("mep:%p fragment:%p %d offset:%d nbanks:%d\n",
                //         m_mepStart,f,i,off,m_banks.size());
                LHCb::decodeFragment(m_evdesc.fragment(i), m_banks);
              }
            }
            return StatusCode::SUCCESS;
          }
        }
        catch(const std::exception& e)  {
          m_sel->error(std::string("Failed to read next event:")+e.what());
        }
        catch(...)  {
          m_sel->error("Failed to read next event - Unknown exception.");
        }
      }
      return StatusCode::FAILURE;
    }
    StatusCode connect(const std::string& input)  {
      StatusCode sc = (m_sel->m_mepMgr) ? connectMEP(input) : connectMBM(input);
      if ( sc.isSuccess() )  {
        for (int i=0, n=m_sel->m_nreqs; i<n; ++i )  {
          m_consumer->addRequest(m_sel->m_Reqs[i]);
        }
      }
      return sc;
    }
    StatusCode connectMEP(const std::string& input)  {
      MEPID mepID = m_sel->m_mepMgr->mepID();
      if ( mepID != MEP_INV_DESC )  {
        m_mepStart = (void*)mepID->mepStart;
        if ( input == "EVENT" )  {
          m_consumer = new MBM::Consumer(mepID->evtBuffer,mepID->processName,mepID->partitionID);
        }
        else if ( input == "MEP" )  {
          m_consumer = new MBM::Consumer(mepID->mepBuffer,mepID->processName,mepID->partitionID);
        }
        else if ( input == "RESULT" )  {
          m_consumer = new MBM::Consumer(mepID->resBuffer,mepID->processName,mepID->partitionID);
        }
        if ( m_consumer )  {
          if ( m_consumer->id() != MBM_INV_DESC )  {
            return StatusCode::SUCCESS;
          }
        }
      }
      return StatusCode::FAILURE;
    }
    StatusCode connectMBM(const std::string& /* input */ )  {
      if ( m_sel )  {
        std::string bm_name = m_sel->m_input;
        unsigned int pid = m_sel->m_partID;
        if ( m_sel->m_partitionBuffer )  {
          char txt[32];
          bm_name += "_";
          bm_name += _itoa(pid,txt,16);
        }
        m_consumer = new MBM::Consumer(bm_name,RTL::processName(),pid);
        return m_consumer->id() == MBM_INV_DESC ? StatusCode::FAILURE : StatusCode::SUCCESS;
      }
      return StatusCode::FAILURE;
    }
    void close()  {
      if ( m_consumer )  {
        for (int i=0, n=m_sel->m_nreqs; i<n; ++i )  {
          m_consumer->delRequest(m_sel->m_Reqs[i]);
        }
        delete m_consumer;
        m_consumer = 0;
      }
      m_mepStart = 0;
    }
  };
}

LHCb::OnlineEvtSelector::OnlineEvtSelector(const std::string& nam, ISvcLocator* svc)
: Service(nam,svc), m_mepMgr(0)
{
  // Requirement format:
  // "EvType=x;TriggerMask=0xfeedbabe,0xdeadfeed,0xdeadbabe,0xdeadaffe;
  //  VetoMask=0x,0x,0x,0x;MaskType=ANY/ALL;UserType=USER/VIP/ONE;
  //  Frequency=MANY/PERC;Perc=20.5"
  declareProperty("Input",m_input  = "EVENT");
  declareProperty("Decode",m_decode = true);
  declareProperty("PartitionID",m_partID = 0x103);
  declareProperty("PartitionBuffer",m_partitionBuffer=false);
  declareProperty("REQ1", m_Rqs[0] = "");
  declareProperty("REQ2", m_Rqs[1] = "");
  declareProperty("REQ3", m_Rqs[2] = "");
  declareProperty("REQ4", m_Rqs[3] = "");
  declareProperty("REQ5", m_Rqs[4] = "");
  declareProperty("REQ6", m_Rqs[5] = "");
  declareProperty("REQ7", m_Rqs[6] = "");
  declareProperty("REQ8", m_Rqs[7] = "");
}

// IInterface::queryInterface
StatusCode LHCb::OnlineEvtSelector::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == IID_IEvtSelector )  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface( riid, ppvIf );
}

/// IService implementation: Db event selector override
StatusCode LHCb::OnlineEvtSelector::initialize()    {
  // Initialize base class
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )    {
    return error("Error initializing base class Service!");
  }
  if ( m_input == "EVENT" || m_input == "RESULT" || m_input == "MEP" )  {
    status = service("MEPManager",m_mepMgr);
    if ( !status.isSuccess() )   {
      return error("Failed to access service MEPManager.");
    }
  }
  for ( m_nreqs=0; m_nreqs<8; ++m_nreqs )  {
    if ( !m_Rqs[m_nreqs].empty() )   {
      m_Reqs[m_nreqs].parse(m_Rqs[m_nreqs]);
      continue;
    }
    break;
  }
  return status;
}

/// IService implementation: Service finalization
StatusCode LHCb::OnlineEvtSelector::finalize()    {
  // Initialize base class
  m_nreqs = 0;
  if ( m_mepMgr )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return Service::finalize();
}

StatusCode LHCb::OnlineEvtSelector::error(const std::string& msg)  const {
  MsgStream log(messageService(), name());
  log << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::OnlineEvtSelector::createContext(Context*& refpCtxt) const  {
  OnlineContext* ctxt = new OnlineContext(this);
  refpCtxt = ctxt;
  return ctxt->connect(m_input);
}

StatusCode LHCb::OnlineEvtSelector::next(Context& ctxt) const {
  try  {
    OnlineContext* pCtxt = dynamic_cast<OnlineContext*>(&ctxt);
    if ( pCtxt != 0 )   {
      return pCtxt->receiveEvent();
    }
  }
  catch(const std::exception& e)  {
    error("Exception: "+std::string(e.what())+" Failed to receive the next event.");
  }
  catch(...)  {
    error("Unknown exception: Failed to receive the next event.");
  }
  return StatusCode::FAILURE;
}

/// Access last item in the iteration
StatusCode LHCb::OnlineEvtSelector::last(Context& /*refContext*/) const  {
  return error(" EventSelector::last(ctxt) not supported ");
}

StatusCode LHCb::OnlineEvtSelector::next(Context& /* ctxt */, int /* jump */) const {
  return error(" EventSelector::next(ctxt, jump) not supported ");
}

StatusCode LHCb::OnlineEvtSelector::previous(Context& /* ctxt */ ) const  {
  return error(" EventSelector::previous(ctxt) not supported ");
}

StatusCode LHCb::OnlineEvtSelector::previous(Context& /* ctxt */, int /* jump */) const {
  return error(" EventSelector::previous(ctxt,jump) -- not supported ");
}

StatusCode LHCb::OnlineEvtSelector::rewind(Context& /* ctxt */) const {
  return error(" EventSelector Iterator, operation rewind not supported ");
}

StatusCode 
LHCb::OnlineEvtSelector::createAddress(const Context& ctxt, IOpaqueAddress*& pAddr) const
{
  const OnlineContext* pctxt = dynamic_cast<const OnlineContext*>(&ctxt);
  if ( pctxt )   {
    const RawEventDescriptor& d = pctxt->descriptor();
    unsigned long   p0 = (unsigned long)&d;
    RawDataAddress* pA = new RawDataAddress(RAWDATA_StorageType,CLID_DataObject,"","0",p0,0);
    pA->setData(std::pair<char*,int>(0,0));
    if ( m_decode && d.eventType() == EVENT_TYPE_EVENT )  {
      pA->setType(RawDataAddress::BANK_TYPE);
      pA->setBanks(&pctxt->banks());
    }
    else if ( d.eventType() == EVENT_TYPE_EVENT ) {
      pA->setType(RawDataAddress::DATA_TYPE);
      pA->setData(std::make_pair((char*)d.header(),d.size()));
    }
    else if ( d.eventType() == EVENT_TYPE_MEP ) {
      pA->setType(RawDataAddress::MEP_TYPE);
      MEPEVENT* ev = (MEPEVENT*)d.header();
      pA->setData(std::make_pair(ev->data,d.size()));
    }
    else  {
      pA->setData(std::make_pair((char*)d.header(),d.size()));
    }
    pA->setFileOffset(0);
    pAddr = pA;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::OnlineEvtSelector::releaseContext(Context*& ctxt) const  {
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
LHCb::OnlineEvtSelector::resetCriteria(const std::string& crit,Context& ct) const {
  OnlineContext* ctxt = dynamic_cast<OnlineContext*>(&ct);
  if ( ctxt )  {
    ctxt->close();
    if ( ctxt->connect(m_input).isSuccess() )  {
      return StatusCode::SUCCESS;
    }
    return error("Failed to connect to:"+crit);
  }
  return error("Invalid iteration context. Cannot connect to:"+crit);
}
