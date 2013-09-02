//  ============================================================
//
//  MEPOverflowWriterSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Markus Frank
//
//  ===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/HLTMonWriterSvc.h"
#include "GaudiOnline/MEPManager.h"
#include "GaudiOnline/DAQError.h"
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
using namespace MonWriter;
DECLARE_NAMESPACE_SERVICE_FACTORY(MonWriter,HLTMonWriterSvc);

extern "C"
{
  void *myCloseFiles(void *a)
  {
    std::list<FileDescr*>::iterator i;
    HLTMonWriterSvc *tis = (HLTMonWriterSvc*)a;
    while (!tis->m_texit)
    {
      tis->LockList();
      i = tis->m_FileCloseList.begin();
      time_t now = time(0);
      while (i != tis->m_FileCloseList.end())
      {
        if (now > (*i)->m_CloseAt)
        {
          close ((*i)->m_Handle);
        }
        tis->m_FileCloseList.erase(i);
        delete (*i);
        i = tis->m_FileCloseList.begin();
      }
      tis->UnlockList();
      sleep(10);
    }
    return 0;
  }
}

// Standard Constructor
HLTMonWriterSvc::HLTMonWriterSvc(const string& nam, ISvcLocator* svc) :
  OnlineService(nam, svc), m_mepMgr(0), m_consumer(0),
      m_receiveEvts(false), m_RunNumber(0),m_FileDesc(0)
{
  m_mepIn =  m_mepOut = m_minAlloc = 0;
  declareProperty("Input", m_input="None");
  declareProperty("PrintFreq", m_freq = 0.);
  declareProperty("Requirements", m_req);
  declareProperty("MEPManager", m_mepMgrName = "LHCb::MEPManager/MEPManager");
  declareProperty("RoutingBits", m_routingBits = 0x400);
  declareProperty("MEPFilePrefix", m_FilePrefixMEP="/scratch/LHCb/HLT1/MEP_Run_");
  declareProperty("EvtFilePrefix", m_FilePrefixEvt="/scratch/LHCb/HLT1/Evt_Run_");
  declareProperty("SizeLimit", m_SizeLimit=2000);
  declareProperty("FileCloseDelay",m_FileCloseDelay=10);
  m_RunList.clear();
  m_texit = false;
  pthread_mutex_init(&m_listlock,0);
  pthread_create(&m_tid,NULL,&myCloseFiles,this);
}

// Standard Destructor
HLTMonWriterSvc::~HLTMonWriterSvc()
{
  pthread_cancel(m_tid);
  pthread_mutex_destroy(&m_listlock);
}

// IInterface implementation: Query interface
StatusCode HLTMonWriterSvc::queryInterface(const InterfaceID& riid,
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
void HLTMonWriterSvc::handle(const Incident& inc)
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
}

StatusCode HLTMonWriterSvc::initialize()
{
  StatusCode sc = OnlineService::initialize();
  MsgStream log(msgSvc(), name());
  m_BytesOut = 0;
  m_NumFiles = 0;
  m_node = RTL::nodeNameShort();
  if (sc.isSuccess())
  {
    if (service(m_mepMgrName, m_mepMgr).isSuccess())
    {
      try
      {
        m_consumer = m_mepMgr->createConsumer(m_input,m_mepMgr->processName());
        for (Requirements::iterator i = m_req.begin(); i != m_req.end(); ++i)  {
          Requirement r;
          r.parse(*i);
          m_consumer->addRequest(r);
        }
        incidentSvc()->addListener(this, "DAQ_CANCEL");
        incidentSvc()->addListener(this, "DAQ_ENABLE");
        declareInfo("MEPsIn", m_mepIn = 0, "Number of MEPs received.");
        declareInfo("MEPsOut", m_mepOut = 0, "Number of MEPs written.");
        declareInfo("BytesOut",m_BytesOut=0,"Number of Bytes Writte to File");
        declareInfo("NumberofFiles",m_NumFiles=0,"Total Number of Files");
        this->m_SizeLimit *= 1024*1024;
        return StatusCode::SUCCESS;
      }
      catch (exception& e)
      {
        return error(string("Failed setup MEP buffers:") + e.what());
      }
    }
    return error("Failed to access MEP manager service.");
  }
  return error("Failed to initialize service base class.");
}

StatusCode HLTMonWriterSvc::finalize()
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
StatusCode HLTMonWriterSvc::run()
{
//  ulonglong prtCount = fabs(m_freq) > 1. / ULONGLONG_MAX ? ulonglong(1.0/ m_freq) : ULONGLONG_MAX;
  m_receiveEvts = true;
  for (int sc = m_consumer->getEvent(); sc == MBM_NORMAL && m_receiveEvts; sc = m_consumer->getEvent())
  {
    const EventDesc& e = m_consumer->event();
    switch (e.type)
    {
      case EVENT_TYPE_MEP:
      {
        MEPEVENT* ev = (MEPEVENT*) e.data;
        MEPEvent* me = (MEPEvent*) ev->data;
        m_mepIn++;
        unsigned int runnr = getRunNumber(me);
        if (m_RunNumber == 0)
        {
          m_RunNumber = runnr;
          RunDesc *r = new RunDesc;
          r->m_RunNumber = m_RunNumber;
          r->m_Files = 0;
          m_RunList[m_RunNumber] = r;
          m_FileDesc = openFile(m_RunNumber,FILETYPE_MEP);
          CreateMonitoringInfo(m_RunNumber);
        }
        else
        {
//          if (m_RunNumber != runnr)
//          {
//            Markclose(m_FileDesc);
//            m_FileDesc->state = C_CLOSED;
//            m_RunNumber = runnr;
//            RunDesc *r = new RunDesc;
//            r->m_RunNumber = m_RunNumber;
//            r->m_Files = 0;
//            m_RunList[m_RunNumber] = r;
//            m_FileDesc = openFile(m_RunNumber,FILETYPE_MEP);
//            CreateMonitoringInfo(m_RunNumber);
//          }
        }
        RunDesc *r = m_RunList[m_RunNumber];
        m_FileDesc = r->m_CurrentFileDescr;
        ssize_t status;
        size_t towrite = me->size()+me->sizeOf();
        status = Write(m_FileDesc->m_Handle, me, towrite);
        if (status == -1)
        {
          continue;
        }
        else
        {
          m_BytesOut += me->m_size;
          r->m_MEPs++;
          m_mepOut++;
          m_FileDesc->m_BytesWritten += me->size();
          m_FileDesc->m_BytesWritten += me->sizeOf();
          r->m_BytesWritten += me->size();
          r->m_BytesWritten += me->sizeOf();
          if (m_FileDesc->m_BytesWritten >this->m_SizeLimit)
          {
            Markclose(m_FileDesc);
            m_FileDesc->state = C_CLOSED;
            m_FileDesc = openFile(m_RunNumber,FILETYPE_MEP);
          }
          continue;
        }
        break;
      }
      case EVENT_TYPE_EVENT:
      {
        RawBank *ev = (RawBank*)e.data;
        MDFHeader *mdf = (MDFHeader*)ev->data();
        m_EvIn++;
        unsigned int runnr = mdf->subHeader().H1->runNumber();
        if (m_RunNumber == 0)
        {
          m_RunNumber = runnr;
          RunDesc *r = new RunDesc;
          r->m_RunNumber = m_RunNumber;
          r->m_Files = 0;
          m_RunList[m_RunNumber] = r;
          m_FileDesc = openFile(m_RunNumber,FILETYPE_EVENT);
          CreateMonitoringInfo(m_RunNumber);
        }
        else
        {
//          if (m_RunNumber != runnr)
//          {
////            close(m_FileDesc->m_Handle);
////            m_FileDesc->state = C_CLOSED;
//            Markclose(m_FileDesc);
//            m_RunNumber = runnr;
//            RunDesc *r = new RunDesc;
//            r->m_RunNumber = m_RunNumber;
//            r->m_Files = 0;
//            m_RunList[m_RunNumber] = r;
//            m_FileDesc = openFile(m_RunNumber,FILETYPE_EVENT);
//            CreateMonitoringInfo(m_RunNumber);
//          }
        }
        RunDesc *r = m_RunList[m_RunNumber];
        m_FileDesc = r->m_CurrentFileDescr;
        ssize_t status;
        size_t towrite = e.len - ev->hdrSize();
        status = Write(m_FileDesc->m_Handle, mdf, towrite);
        if (status == -1)
        {
          continue;
        }
        else
        {
          m_BytesOut += towrite;
          r->m_MEPs++;
          m_mepOut++;
          m_FileDesc->m_BytesWritten += towrite;
          r->m_BytesWritten += towrite;
          if (m_FileDesc->m_BytesWritten >this->m_SizeLimit)
          {
            Markclose(m_FileDesc);
            m_FileDesc->state = C_CLOSED;
            m_FileDesc = openFile(m_RunNumber,FILETYPE_EVENT);
          }
          continue;
        }
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}
void HLTMonWriterSvc::Markclose(FileDescr* d)
{
  d->m_CloseAt = time(0)+m_FileCloseDelay;
  LockList();
  this->m_FileCloseList.push_back(d);
  UnlockList();
}
unsigned int HLTMonWriterSvc::getRunNumber(MEPEvent *me)
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
FileDescr *HLTMonWriterSvc::openFile(unsigned int runn, FTYPE t)
{
  RunDesc *r = m_RunList[runn];
  int seq = r->m_Files;
  seq++;
  r->m_Files++;
  FileDescr *f = new FileDescr(t);
  r->m_CurrentFileDescr = f;
  f->m_Sequence = seq;
  char fname[255];
  std::string format;
  if (t == FILETYPE_MEP)
  {
    format = m_FilePrefixMEP+"%07d_%s.%s.MEP";
  }
  else
  {
    format = m_FilePrefixEvt+"%07d_%s.%s.mdf";
  }
  std::string ftim = FileTime();
  sprintf(fname,format.c_str(),runn,ftim.c_str(),m_node.c_str());
  f->m_BytesWritten = 0;
  f->m_Handle = open(fname,O_RDWR+O_CREAT+O_APPEND+O_LARGEFILE+O_NOATIME,
      S_IRWXU|S_IRWXG|S_IRWXO);
  f->m_FileName = fname;
  f->state = C_OPEN;
  f->m_BytesWritten = 0;
  m_NumFiles++;
  return f;
}

void HLTMonWriterSvc::CreateMonitoringInfo(unsigned int runn)
{
  RunDesc *r=m_RunList[runn];
  std::string namePref;
  char cRunNo[255];
  sprintf(cRunNo,"%d/",runn);
  namePref = m_input+"/";
  namePref +=cRunNo;
  std::string comm;
  comm ="Number of Files";
  std::string mnam = namePref+"NoFiles";
  declareInfo(namePref+"NoFiles",r->m_Files,comm);
  comm="Number of MEPs";
  mnam = namePref+"MEPs";
  declareInfo(mnam,r->m_MEPs,comm);
  comm="Number of Bytes";
  mnam = namePref+"Bytes";
  declareInfo(mnam,r->m_BytesWritten,comm);
}
std::string HLTMonWriterSvc::FileTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, 80, "%Y%m%d-%H%M%S", timeinfo);
  std::string ret = std::string(buffer);
  return ret;
}
void HLTMonWriterSvc::handleFileWriteError()
{
  std::string node;
  node = m_node;
  for (unsigned int i=0;i<node.size();i++)
  {
    node[i] = toupper(node[i]);
  }
  std::string cmdname=node+"_MEPRx_01/setOverflow";
  DimClient::sendCommand(cmdname.c_str(),2);
}

ssize_t HLTMonWriterSvc::Write(int fd, const void *buf, size_t n)
{
  char *cbuf = (char*)buf;
  size_t towrite = n;
  int status;
  int nintr_max = 2;
  while (towrite > 0)
  {
    status = write(fd, cbuf, towrite);
    if (status >0)
    {
      towrite = towrite - status;
      cbuf += status;
      continue;
    }
    else if (status == 0)
    {
      printf("%s: 0 Bytes written! Ignoring MEP\n", RTL::processName().c_str());
      status = -1;
      return status;
    }
    else if (status == -1)
    {
      if (errno == EIO || errno == ENOSPC)
      {
        printf("%s: File Write Error (IO or NoSpace): Errno = %d\n", RTL::processName().c_str(),errno);
        handleFileWriteError();
        status = -1;
        return status;
      }
      else if (errno == EINTR)
      {
        nintr_max--;
        if (nintr_max >0)
        {
          continue;
        }
        else
        {
          status = -1;
          return status;
        }
      }
      else
      {
        printf("%s: File Write Error: Errno = %d\n", RTL::processName().c_str(),errno);
        handleFileWriteError();
        status = -1;
        return status;
      }
    }
  }
  status = 0;
  return status;
}
