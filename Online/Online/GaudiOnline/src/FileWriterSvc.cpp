//  ============================================================
//
//  FileWriterSvc.cpp
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
#include "GaudiOnline/FileWriterSvc.h"
#include "GaudiOnline/MEPManager.h"
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
#include <boost/filesystem.hpp>

using MBM::Producer;
using MBM::Consumer;
using MBM::EventDesc;
using MBM::Requirement;
using namespace std;
using namespace LHCb;
using namespace FileWriter;

extern "C"
{
  void *ClsFiles(void *a)
  {
    std::list<FileDescr*>::iterator i;
    FileWriterSvc *tis = (FileWriterSvc*)a;
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
        delete (*i);
        tis->m_FileCloseList.erase(i);
        i = tis->m_FileCloseList.begin();
      }
      tis->UnlockList();
      sleep(10);
    }
    return 0;
  }
}

// Standard Constructor
FileWriterSvc::FileWriterSvc(const string& nam, ISvcLocator* svc) :
  OnlineService(nam, svc), /*m_mepMgr(0), *//*m_consumer(0),*/
      m_receiveEvts(false), m_RunNumber(0),m_FileDesc(0),m_SteeringSvc(0)
{
  m_mepIn =  m_mepOut = m_minAlloc = m_EvIn = m_EvOut= 0;
//  declareProperty("Input", m_input="None");
  declareProperty("PrintFreq", m_freq = 0.);
  declareProperty("Requirements", m_req);
//  declareProperty("MEPManager", m_mepMgrName = "LHCb::MEPManager/MEPManager");
//  declareProperty("RoutingBits", m_routingBits = 0x400);
  declareProperty("MEPFilePrefix", m_FilePrefixMEP="/localdisk/overflow/Run_");
  declareProperty("EvtFilePrefix", m_FilePrefixEvt="/localdisk/HLT1/Run_");
  declareProperty("SizeLimit", m_SizeLimit=250);
  declareProperty("FileCloseDelay",m_FileCloseDelay=10);
  declareProperty("MaxEvents",m_maxevts=-1);
  declareProperty("EventFraction",m_evfrac=1.0);
  declareProperty("DIMSteering",m_DIMSteering = 0);
  declareProperty("PartitionName",m_PartitionName="LHCb");
  declareProperty("NodePattern",m_NodePattern="hlt[a-f][0-1][0-9](.*)");
  m_RunList.clear();
  m_texit = false;
  m_numev = 0;
  m_writeEnabled=false;
  pthread_mutex_init(&m_listlock,0);
  pthread_create(&m_tid,NULL,&ClsFiles,this);
}

// Standard Destructor
FileWriterSvc::~FileWriterSvc()
{
  pthread_cancel(m_tid);
  pthread_mutex_destroy(&m_listlock);
}

// IInterface implementation: Query interface
StatusCode FileWriterSvc::queryInterface(const InterfaceID& riid,
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
void FileWriterSvc::handle(const Incident& )
{
//  MsgStream log(msgSvc(), name());
//  log << MSG::INFO << "Got incident:" << inc.source() << " of type "
//      << inc.type() << endmsg;
//  if (inc.type() == "DAQ_CANCEL")
//  {
//    m_receiveEvts = false;
//    m_consumer->cancel();
//  }
//  else if (inc.type() == "DAQ_ENABLE")
//  {
//    m_receiveEvts = true;
//  }
}

StatusCode FileWriterSvc::initialize()
{
  StatusCode sc = OnlineService::initialize();
  MsgStream log(msgSvc(), name());
  bool matchstat;
  m_BytesOut = 0;
  m_NumFiles = 0;
  m_node = RTL::nodeNameShort();
  m_writeEnabled = true;
  reg_NodePattern = boost::regex(m_NodePattern.c_str(),boost::regex_constants::icase);
  matchstat = boost::regex_search(m_node, reg_NodePattern);
//  if (matchstat)
//  {
//    printf("[WARNING] Node %s matched pattern %s\n",m_node.c_str(), m_NodePattern.c_str());
//  }
//  else
//  {
//    printf("[WARNING] Node %s does not match pattern %s\n",m_node.c_str(), m_NodePattern.c_str());
//  }
  m_writeEnabled = matchstat;
  if (sc.isSuccess())
  {
//        incidentSvc()->addListener(this, "DAQ_CANCEL");
//        incidentSvc()->addListener(this, "DAQ_ENABLE");
        declareInfo("MEPsIn", m_mepIn = 0, "Number of MEPs received.");
        declareInfo("MEPsOut", m_mepOut = 0, "Number of MEPs written.");
        declareInfo("EvtsIn", m_EvIn = 0, "Number of Events received.");
        declareInfo("EvtsOut", m_EvOut = 0, "Number of Events written.");
        declareInfo("BytesOut",m_BytesOut=0,"Number of Bytes Written to File");
        declareInfo("NumberofFiles",m_NumFiles=0,"Total Number of Files");
        this->m_SizeLimit *= 1024*1024;
        size_t lslash = m_FilePrefixMEP.find_last_of("/");
        m_DirectoryMEP = m_FilePrefixMEP.substr(0,lslash);
        {
          boost::filesystem::path dir(m_DirectoryMEP);
          if (!boost::filesystem::exists(dir))
          {
            if (!boost::filesystem::create_directories(dir))
            {
              log<<MSG::ERROR<<"Cannot Create Directory "<<dir<<endmsg;
              return StatusCode::FAILURE;
            }
          }
        }
        lslash = m_FilePrefixEvt.find_last_of("/");
        m_DirectoryEvt = m_FilePrefixEvt.substr(0,lslash);
        {
          boost::filesystem::path dir(m_DirectoryEvt);
          if (!boost::filesystem::exists(dir))
          {
            if (!boost::filesystem::create_directories(dir))
            {
              log<<MSG::ERROR<<"Cannot Create Directory "<<dir<<endmsg;
              return StatusCode::FAILURE;
            }
          }
        }
        return StatusCode::SUCCESS;
//      }
//      catch (exception& e)
//      {
//        return error(string("Failed setup MEP buffers:") + e.what());
//      }
//    }
//    return error("Failed to access MEP manager service.");
  }
  return error("Failed to initialize service base class.");
}
StatusCode FileWriterSvc::start()
{
  OnlineService::start();
  if (m_DIMSteering != 0)
  {
    if (m_SteeringSvc == 0)
    {
      std::string svcnam;
      svcnam = m_PartitionName+"/"+name()+"/Control";
      m_SteeringSvc = new SteeringInfo(svcnam,m_Steeringdata,this);
    }
    m_maxevts = -1;
    m_EvOut = 0;
  }
  return StatusCode::SUCCESS;
}
void FileWriterSvc::clearCounters()
{
  m_EvOut = 0;
  m_EvIn = 0;
  m_mepIn = 0;
  m_mepOut = 0;
  return;
}
StatusCode FileWriterSvc::stop()
{
  if (m_SteeringSvc != 0)
  {
    delete m_SteeringSvc;
  }
  OnlineService::stop();
  return StatusCode::SUCCESS;
}
StatusCode FileWriterSvc::finalize()
{
//  if (m_consumer)
//  {
//    m_consumer = 0;
//  }
  undeclareAll();
  m_minAlloc = 0;
  m_mepIn = 0;
  m_mepOut = 0;
  return OnlineService::finalize();
}

/// Process single event
StatusCode FileWriterSvc::run()
{
  return StatusCode::SUCCESS;
}
bool FileWriterSvc::matchRequirements(const EventDesc &e)
{
  BinRequirements::iterator it;
  bool tmatch, vmatch;
  tmatch = false;
  vmatch = false;
  for (it=m_breq.begin();it!=m_breq.end();it++)
  {
    for (int i=0;i<4;i++)
    {
      tmatch = tmatch | ((it->trmask[i] & e.mask[i])!= 0);
      vmatch = vmatch | ((it->vetomask[i] & e.mask[i]) !=0);
    }
  }
  if (!tmatch || vmatch) return false;
  return true;
}
StatusCode FileWriterSvc::run(const EventDesc& e, unsigned int runnr)
{
  if (this->m_DIMSteering !=0)
  {
    if (this->m_Steeringdata == 0)
    {
      return StatusCode::SUCCESS;
    }
  }
  if (!m_writeEnabled)
  {
    return StatusCode::SUCCESS;
  }
//  ulonglong prtCount = fabs(m_freq) > 1. / ULONGLONG_MAX ? ulonglong(1.0/ m_freq) : ULONGLONG_MAX;
  m_receiveEvts = true;
  {
    switch (e.type)
    {
      case EVENT_TYPE_MEP:
      {
        m_mepIn++;
        break;
      }
      case EVENT_TYPE_EVENT:
      {
        m_EvIn++;
        break;
      }
      default:
      {
        break;
      }
    }
    if (!matchRequirements(e)) return StatusCode::SUCCESS;
    if (m_numev >= m_maxevts && m_maxevts>0 ) return StatusCode::SUCCESS;
    m_numev++;
    double f=float(rand());
    f = f/RAND_MAX;
    if (f>m_evfrac) return StatusCode::SUCCESS;
    switch (e.type)
    {
      case EVENT_TYPE_MEP:
      {
        MEPEVENT* ev = (MEPEVENT*) e.data;
        MEPEvent* me = (MEPEvent*) ev->data;
        m_mepIn++;
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
          if (m_RunNumber != runnr)
          {
            Markclose(m_FileDesc);
            m_FileDesc->state = C_CLOSED;
            m_RunNumber = runnr;
            RunDesc *r = new RunDesc;
            r->m_RunNumber = m_RunNumber;
            r->m_Files = 0;
            m_RunList[m_RunNumber] = r;
            m_FileDesc = openFile(m_RunNumber,FILETYPE_MEP);
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
          break;
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
          break;
        }
        break;
      }
      case EVENT_TYPE_EVENT:
      {
        RawBank *ev = (RawBank*)e.data;
        MDFHeader *mdf = (MDFHeader*)ev->data();
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
          if (m_RunNumber != runnr)
          {
//            close(m_FileDesc->m_Handle);
//            m_FileDesc->state = C_CLOSED;
            Markclose(m_FileDesc);
            m_RunNumber = runnr;
            RunDesc *r = new RunDesc;
            r->m_RunNumber = m_RunNumber;
            r->m_Files = 0;
            m_RunList[m_RunNumber] = r;
            m_FileDesc = openFile(m_RunNumber,FILETYPE_EVENT);
            CreateMonitoringInfo(m_RunNumber);
          }
        }
        RunDesc *r = m_RunList[m_RunNumber];
        m_FileDesc = r->m_CurrentFileDescr;
        ssize_t status;
        size_t towrite = e.len - ev->hdrSize();
        status = Write(m_FileDesc->m_Handle, mdf, towrite);
        if (status == -1)
        {
          break;
        }
        else
        {
          m_BytesOut += towrite;
          r->m_MEPs++;
          m_EvOut++;
          m_FileDesc->m_BytesWritten += towrite;
          r->m_BytesWritten += towrite;
          if (m_FileDesc->m_BytesWritten >this->m_SizeLimit)
          {
            Markclose(m_FileDesc);
            m_FileDesc->state = C_CLOSED;
            m_FileDesc = openFile(m_RunNumber,FILETYPE_EVENT);
          }
          break;
        }
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}
void FileWriterSvc::Markclose(FileWriter::FileDescr* d)
{
  d->m_CloseAt = time(0)+m_FileCloseDelay;
  LockList();
  this->m_FileCloseList.push_back(d);
  UnlockList();
}
unsigned int FileWriterSvc::getRunNumber(MEPEvent *me)
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

FileDescr *FileWriterSvc::openFile(unsigned int runn, FTYPE t)
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
std::vector<std::string> &FileWriterSvc::getRequirements()
{
  for (Requirements::iterator i = m_req.begin(); i != m_req.end(); ++i)  {
    Requirement r;
    r.parse(*i);
    m_breq.insert(m_breq.end(),r);
  }
  return this->m_req;
}
//void FileWriterSvc::AddRequirements(MBM::Consumer* consumer)
//{
//  m_consumer = consumer;
//  try
//  {
//    for (Requirements::iterator i = m_req.begin(); i != m_req.end(); ++i)  {
//      Requirement r;
//      r.parse(*i);
//      consumer->addRequest(r);
//      m_breq.insert(m_breq.end(),r);
//    }
//    return;
//  }
//  catch (exception& e)
//  {
//    error(string("Failed setup MEP buffers:") + e.what());
//    return;
//  }
//}

void FileWriterSvc::CreateMonitoringInfo(unsigned int runn)
{
  RunDesc *r=m_RunList[runn];
  if(true) return;
  std::string namePref;
  char cRunNo[255];
  sprintf(cRunNo,"%d/",runn);
//  namePref = m_input+"/";
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
std::string FileWriterSvc::FileTime()
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
void FileWriterSvc::handleFileWriteError()
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

ssize_t FileWriterSvc::Write(int fd, const void *buf, size_t n)
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
