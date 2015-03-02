//  ============================================================
//
//  FileWriteMgr.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Markus Frank
//
//  ===========================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/FileWriteMgr.h"
#include "GaudiOnline/MEPManager.h"
#include "GaudiOnline/FileWriterSvc.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"
#include "MBM/Consumer.h"
#include "MBM/Requirement.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MEPMultiFragment.h"
#include "MDF/MEPFragment.h"
#include <climits>
#include <cmath>
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "time.h"
#include <errno.h>
#include "dic.hxx"

using MBM::Producer;
using MBM::Consumer;
using MBM::EventDesc;
using MBM::Requirement;
using namespace std;
using namespace LHCb;
void toLowerCase(std::string &s)
{
  for (unsigned int i=0;i<s.size();i++)
  {
    s[i] = tolower(s[i]);
  }
}

//extern "C"
//{
//  void *CloseFiles(void *a)
//  {
//    std::list<FileDescr*>::iterator i;
//    FileWriteMgr *tis = (FileWriteMgr*)a;
//    while (!tis->m_texit)
//    {
//      tis->LockList();
//      i = tis->m_FileCloseList.begin();
//      time_t now = time(0);
//      while (i != tis->m_FileCloseList.end())
//      {
//        if (now > (*i)->m_CloseAt)
//        {
//          close ((*i)->m_Handle);
//        }
//        delete (*i);
//        tis->m_FileCloseList.erase(i);
//        i = tis->m_FileCloseList.begin();
//      }
//      tis->UnlockList();
//      sleep(10);
//    }
//    return 0;
//  }
//}

// Standard Constructor
FileWriteMgr::FileWriteMgr(const string& nam, ISvcLocator* svc) :
  OnlineService(nam, svc), m_mepMgr(0), m_consumer(0),
      m_receiveEvts(false), m_RunNumber(0)
{
  m_SvcLoc = svc;
  m_mepIn =  m_mepOut = m_minAlloc = 0;
  declareProperty("Input", m_input="None");
  declareProperty("MEPManager", m_mepMgrName = "LHCb::MEPManager/MEPManager");
  declareProperty("RoutingBits", m_routingBits = 0x400);
//  declareProperty("WriterServicePatt",m_ServPatt="AlignDataWriter_");
  declareProperty("WriterList", m_FileWriters);
//  m_RunList.clear();
  m_texit = false;
//  pthread_mutex_init(&m_listlock,0);
//  pthread_create(&m_tid,NULL,&CloseFiles,this);
}

// Standard Destructor
FileWriteMgr::~FileWriteMgr()
{
//  pthread_cancel(m_tid);
  pthread_mutex_destroy(&m_listlock);
}

// IInterface implementation: Query interface
StatusCode FileWriteMgr::queryInterface(const InterfaceID& riid,
    void** ppIf)
{
  if (IRunable::interfaceID().versionMatch(riid))
  {
    *ppIf = (IRunable*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// Incident handler implemenentation: Inform that a new incident has occured
void FileWriteMgr::handle(const Incident& inc)
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Got incident:" << inc.source() << " of type "
      << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")
  {
    m_receiveEvts = false;
    m_consumer->cancel();
  }
  else if (inc.type() == "DAQ_ENABLE")
  {
    m_receiveEvts = true;
  }
  else if (inc.type() == "APP_INITIALIZED")
  {
    if (service(m_mepMgrName, m_mepMgr).isSuccess())
    {
      try
      {
        m_consumer = m_mepMgr->createConsumer(m_input,m_mepMgr->processName());
      }
      catch (exception& e)
      {
      }
    }
    SvcList::iterator it;
    for (it=m_ServiceList.begin();it!=m_ServiceList.end();it++)
    {
      FileWriterSvc *fw=(*it);
      std::vector<std::string> reqlist = fw->getRequirements();
      for (size_t i=0;i<reqlist.size();i++)
      {
        Requirement r;
        r.parse(reqlist[i]);
        m_consumer->addRequest(r);
      }
    }
  }
}

StatusCode FileWriteMgr::initialize()
{
  StatusCode sc = OnlineService::initialize();
  MsgStream log(msgSvc(), name());
  m_BytesOut = 0;
  m_NumFiles = 0;
//  toLowerCase(m_ServPatt);
  m_node = RTL::nodeNameShort();
  ISvcList svclist;
//  svclist = m_SvcLoc->getServices();
//  ISvcList::iterator it;
  for (size_t i=0;i<m_FileWriters.size();i++)
//  for (it=svclist.begin();it!=svclist.end();it++)
  {
    string nam;
    IService *is;
//    is = *it;
//    nam = is->name();
//    toLowerCase(nam);
    sc = service(m_FileWriters[i],is,true);
    if (sc.isSuccess())
    {
      FileWriterSvc* fs = dynamic_cast<FileWriterSvc*>(is);
      m_ServiceList.insert(m_ServiceList.end(),fs);
    }
  }
  if (sc.isSuccess())
  {
    incidentSvc()->addListener(this, "DAQ_CANCEL");
    incidentSvc()->addListener(this, "DAQ_ENABLE");
    incidentSvc()->addListener(this, "APP_INITIALIZED");
    declareInfo("MEPsIn", m_mepIn = 0, "Number of MEPs received.");
    declareInfo("MEPsOut", m_mepOut = 0, "Number of MEPs written.");
    declareInfo("BytesOut",m_BytesOut=0,"Number of Bytes Writte to File");
    declareInfo("NumberofFiles",m_NumFiles=0,"Total Number of Files");
    this->m_SizeLimit *= 1024*1024;
    return sc;
  }
  return error("Failed to initialize service base class.");
}

StatusCode FileWriteMgr::finalize()
{
  if (m_consumer)
  {
    delete m_consumer;
    m_consumer = 0;
  }
  if (m_mepMgr)
  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  undeclareAll();
  m_minAlloc = 0;
  m_mepIn = 0;
  m_mepOut = 0;
  return OnlineService::finalize();
}

/// Process single event
StatusCode FileWriteMgr::run()
{
//  ulonglong prtCount = fabs(m_freq) > 1. / ULONGLONG_MAX ? ulonglong(1.0/ m_freq) : ULONGLONG_MAX;
  m_receiveEvts = true;
  unsigned int runnr;
  for (int sc = m_consumer->getEvent(); sc == MBM_NORMAL && m_receiveEvts; sc = m_consumer->getEvent())
  {
    const EventDesc& e = m_consumer->event();
    switch (e.type)
    {
      case EVENT_TYPE_MEP:
      {
        MEPEVENT* ev = (MEPEVENT*) e.data;
        MEPEvent* me = (MEPEvent*) ev->data;
        runnr =  getRunNumber(me);
        break;
      }
      case EVENT_TYPE_EVENT:
      {
        RawBank *ev = (RawBank*)e.data;
        MDFHeader *mdf = (MDFHeader*)ev->data();
        m_EvIn++;
        runnr = mdf->subHeader().H1->runNumber();
        break;
      }
      default:
      {
        printf("Unknown Event Data Type. %d",e.type);
        continue;
      }
    }
    SvcList::iterator it;
    for (it=m_ServiceList.begin();it!=m_ServiceList.end();it++)
    {
      FileWriterSvc *fw=(*it);
      fw->run(e,runnr);
    }
  }
  return StatusCode::SUCCESS;
}
unsigned int FileWriteMgr::getRunNumber(MEPEvent *me)
{

  for (MEPMultiFragment* mf = me->first(); mf < me->last(); mf = me->next(mf))
  {
    for (MEPFragment* f = mf->first(); f < mf->last(); f = mf->next(f))
    {
      RawBank* b = f->first();
      if (b->type() == RawBank::ODIN)
      {
        return *(b->data());
      }
    }
  }
  return 0;
}
