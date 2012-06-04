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
#include "GaudiOnline/MEPOverflowWriterSvc.h"
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
#include <time.h>
#include <errno.h>
#include "dic.hxx"


using MBM::Producer;
using MBM::Consumer;
using MBM::EventDesc;
using MBM::Requirement;
using namespace std;
using namespace LHCb;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPOverflowWriterSvc)

// Standard Constructor
MEPOverflowWriterSvc::MEPOverflowWriterSvc(const string& nam, ISvcLocator* svc) :
  OnlineService(nam, svc), m_mepMgr(0), m_consumer(0),
      m_receiveEvts(false), m_RunNumber(0),m_FileDesc(0)
{
  m_mepIn =  m_mepOut = m_minAlloc = 0;
  declareProperty("PrintFreq", m_freq = 0.);
  declareProperty("Requirements", m_req);
  declareProperty("MEPManager", m_mepMgrName = "LHCb::MEPManager/MEPManager");
  declareProperty("RoutingBits", m_routingBits = 0x400);
  declareProperty("FilePrefix", m_FilePrefix="/localdisk/overflow/Run_");
  declareProperty("SizeLimit", m_SizeLimit=250);
  m_RunList.clear();
}

// Standard Destructor
MEPOverflowWriterSvc::~MEPOverflowWriterSvc()
{
}

// IInterface implementation: Query interface
StatusCode MEPOverflowWriterSvc::queryInterface(const InterfaceID& riid,
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
void MEPOverflowWriterSvc::handle(const Incident& inc)
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

StatusCode MEPOverflowWriterSvc::initialize()
{
  StatusCode sc = OnlineService::initialize();
  m_BytesOut = 0;
  m_NumFiles = 0;
  MsgStream log(msgSvc(), name());
  if (sc.isSuccess())
  {
    if (service(m_mepMgrName, m_mepMgr).isSuccess())
    {
      try
      {
        m_consumer = m_mepMgr->createConsumer("Overflow",m_mepMgr->processName());
        for (Requirements::iterator i = m_req.begin(); i != m_req.end(); ++i)
        {
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

StatusCode MEPOverflowWriterSvc::finalize()
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
StatusCode MEPOverflowWriterSvc::run()
{
//  ulonglong prtCount = fabs(m_freq) > 1. / ULONGLONG_MAX ? ulonglong(1.0/ m_freq) : ULONGLONG_MAX;
  m_receiveEvts = true;
  for (int sc = m_consumer->getEvent(); sc == MBM_NORMAL && m_receiveEvts; sc = m_consumer->getEvent())
  {
//    unsigned int pid = id->partitionID;
    const EventDesc& e = m_consumer->event();
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
      m_FileDesc = openFile(m_RunNumber);
      CreateMonitoringInfo(m_RunNumber);
    }
    else
    {
      if (m_RunNumber != runnr)
      {
        close(m_FileDesc->m_Handle);
        m_FileDesc->state = C_CLOSED;
        m_RunNumber = runnr;
        RunDesc *r = new RunDesc;
        r->m_RunNumber = m_RunNumber;
        r->m_Files = 0;
        m_RunList[m_RunNumber] = r;
        m_FileDesc = openFile(m_RunNumber);
        CreateMonitoringInfo(m_RunNumber);
      }
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
        close(m_FileDesc->m_Handle);
        m_FileDesc->state = C_CLOSED;
        m_FileDesc = openFile(m_RunNumber);
      }
      continue;
    }
  }
  return StatusCode::SUCCESS;
}
unsigned int MEPOverflowWriterSvc::getRunNumber(MEPEvent *me)
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
FileDescr *MEPOverflowWriterSvc::openFile(unsigned int runn)
{
  RunDesc *r = m_RunList[runn];
  int seq = r->m_Files;
  seq++;
  r->m_Files++;
  FileDescr *f = new FileDescr;
  r->m_CurrentFileDescr = f;
  f->m_Sequence = seq;
  char fname[255];
  std::string format = m_FilePrefix+"%07d_%s.MEP";
  std::string ftim = FileTime();
  sprintf(fname,format.c_str(),runn,ftim.c_str());
  f->m_BytesWritten = 0;
  f->m_Handle = open(fname,O_RDWR|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
  f->m_FileName = fname;
  f->state = C_OPEN;
  f->m_BytesWritten = 0;
  m_NumFiles++;
  return f;
}

void MEPOverflowWriterSvc::CreateMonitoringInfo(unsigned int runn)
{
  RunDesc *r=m_RunList[runn];
  std::string namePref;
  char cRunNo[255];
  sprintf(cRunNo,"%d/",runn);
  namePref = "Overflow/";
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
std::string MEPOverflowWriterSvc::FileTime()
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
void MEPOverflowWriterSvc::handleFileWriteError()
{
  std::string node;
  node = RTL::nodeNameShort();
  for (unsigned int i=0;i<node.size();i++)
  {
    node[i] = toupper(node[i]);
  }
  std::string cmdname=node+"_MEPRx_01/setOverflow";
  DimClient::sendCommand(cmdname.c_str(),0);
}

ssize_t MEPOverflowWriterSvc::Write(int fd, const void *buf, size_t n)
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
