//  ====================================================================
//  SLICEReaderSvc.cpp
//  --------------------------------------------------------------------
//
//  Author    : Jean-Christophe Garnier 
//
//  Description : 
//  
//  This file contains the implementation of the Slice reader service. It consists 
//  in a GAUDI service which reads chunks of 2 MBytes, isolate events and push them
//  into a buffer manager.
//  A slice reader can feed a buffer manager at a frequency up to 8kHz. 
//
//  It assumes that the buffer manager were created by the injector, and will be deleted by the injector. 
//
//  ====================================================================
#ifndef _WIN32

#include "GaudiOnline/SLICEReaderSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "MDF/RawEventHelpers.h"
#include "GaudiOnline/MEPRxSys.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "MBM/Requirement.h"
#include "MBM/mepdef.h"

#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#define TWOMB 2097152
#define MDFHDRSZ 48 //12 word of 32 bits, or 4 bytes

#define ERRMSG(a,x) do {  \
  (a) << MSG::ERROR << (x) << " " << " in " << __PRETTY_FUNCTION__<< ":"  << __FILE__<< ":(" << __LINE__ << ")" << endmsg;} while(0);

#define PUBCNT(name, desc) do {m_ ## name = 0; m_MonSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_MonSvc->declareInfo(#name, "I", & m_ ## name [0], m_ ## name.size() * sizeof(int), desc, this);} while(0);

using MBM::Requirement;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, SLICEReaderSvc)
using namespace LHCb;


SLICEReaderSvc::SLICEReaderSvc(const std::string &name, ISvcLocator *pSvcLocator):Service(name, pSvcLocator), m_IncidentSvc(0), m_MonSvc(0)
{   
    m_ProcName = RTL::processName();

    declareProperty("InputFiles", m_InputFiles);
    declareProperty("Loop", m_LoopOverFiles=false);
    declareProperty("BufferName", m_BufferName);
    declareProperty("PartitionID", m_PartitionID=0xffffffff);
    declareProperty("Requirements", m_Req);

    m_BMID = MBM_INV_DESC;
    m_ReaderState = NOT_READY;

} 



// IInterface implementation : queryInterface
StatusCode SLICEReaderSvc::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IRunable::interfaceID().versionMatch(riid) )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppIf);
}

void SLICEReaderSvc::clearCounters()
{
    m_TotEvtsRead = 0;
    m_TotFilesRead = 0;
}

void SLICEReaderSvc::publishCounters()
{
    PUBCNT(TotEvtsRead,     "Total of event reads");
    PUBCNT(TotFilesRead,    "Total of files read");
}

int SLICEReaderSvc::setupCounters() 
{
    MsgStream msgLog(msgSvc(),name());
    clearCounters();
    publishCounters();

    return 0;
}

BMID SLICEReaderSvc::getBuffer()
{
/*
    m_BufferName+="_";
    char tmp[64];
    sprintf(tmp, "%x",m_PartitionID);
    m_BufferName += tmp;
*/ 
   return mbm_include(m_BufferName.c_str(), m_ProcName.c_str(), m_PartitionID);  
}

StatusCode SLICEReaderSvc::error(const std::string & msg)
{     
    static MsgStream msgLog(msgSvc(), name());
    msgLog << MSG::ERROR << msg << endmsg;
    return StatusCode::FAILURE;
}   

StatusCode SLICEReaderSvc::initialize()
{
    StatusCode sc = Service::initialize();
    static MsgStream msgLog(msgSvc(), name());
    if (!sc.isSuccess())
    {
        msgLog << MSG::ERROR << "Failed to initialize service base class." << endmsg;
        return sc;
    }

    if(!service("IncidentSvc", m_IncidentSvc).isSuccess())
    {
        msgLog << MSG::ERROR << "Failed to access incident service." << endmsg;
        return StatusCode::FAILURE;
    }
    m_IncidentSvc->addListener(this, "DAQ_CANCEL");
    m_IncidentSvc->addListener(this, "DAQ_ENABLE"); 

    if(!service("MonitorSvc", m_MonSvc).isSuccess()) {
        ERRMSG(msgLog, "Failed to access to monitor service.");
        return StatusCode::FAILURE;
    }
    else {
      if (setupCounters()) {
        return error("Failed to setup counters");
      }
    }

    m_BMID = getBuffer();

    if(m_BMID == MBM_INV_DESC) 
    {
        msgLog << MSG::ERROR << "Failed to connect to the buffers, make sure the injector opened the one you want to connect to" << endmsg;
        return StatusCode::FAILURE;  
    } 

    m_CurFile =0;
    if(m_InputFiles.size() == 0) 
        msgLog << MSG::WARNING << "No input files -> no injection" << endmsg;

    for(std::vector<std::string>::iterator ite = m_InputFiles.begin(); ite != m_InputFiles.end(); ++ite)
    {
        msgLog << MSG::INFO << *ite << endmsg;        
    }  

    m_CurEvtLen=m_OffsetEvt=m_CurEvtIte=m_CurSliceIte=0;

    m_TotEvtsRead=0;

    msgLog << MSG::INFO << "INITIALISATION DONE" << endmsg;
    m_ReaderState = READY;
    return StatusCode::SUCCESS;
}

void displayBuf(char *buf, int size) 
{
    int *ibuf = (int *) buf;
    for(int i=0; i<size/4; ++i) {
        if(i%4 == 0) printf("\n%5x:", i);
        printf(" %8x ", ibuf[i]);
    }
    printf("\n");
}


StatusCode SLICEReaderSvc::decodeMDF(char *slice, int size) 
{
    m_SliceLen = size;
    m_CurSliceIte =0;
  
    static MsgStream msgLog(msgSvc(), name());
 
    msgLog << MSG::DEBUG << "Size of chunk read : " << size << endmsg;

    if(m_CurEvtIte >0 && m_CurEvtIte < m_CurEvtLen)
    { 
        msgLog << MSG::DEBUG << "(m_CurEvtIte = " << m_CurEvtIte << " < m_CurEvtLen = " << m_CurEvtLen << ")" << endmsg;
        memcpy(m_CurEvent+m_CurEvtIte, slice, m_CurEvtLen-m_CurEvtIte);
        m_CurSliceIte=m_CurEvtLen-m_CurEvtIte;
        m_CurEvtIte=m_CurEvtLen;

        StatusCode sc = pushEvent(m_CurEvent, m_CurEvtLen);
        if(sc.isRecoverable()) 
        {
            msgLog << MSG::INFO << "End of run : exiting decodeMDF" << endmsg;
            return sc;
        } 
        if(sc.isFailure())
        {
            msgLog << MSG::ERROR << "Failed to send event to the injector" << endmsg;
            return sc;
        }

            
        msgLog << MSG::DEBUG << "Cur evt Len = " << m_CurEvtLen << ", cur evt ite = " << m_CurEvtIte << endmsg;
        msgLog << MSG::DEBUG << "Cur slice Len = " << m_SliceLen << ", cur slice ite = " << m_CurSliceIte << endmsg;
    }
    
    //Event completely read 
    ++m_TotEvtsRead;
    
 
    m_OffsetEvt = m_CurSliceIte;
    msgLog << MSG::DEBUG << "cursliceite " << m_CurSliceIte << ", offsetevt " << m_OffsetEvt << endmsg;

    while (m_OffsetEvt < size) {
        int *pword = (int *) (slice+m_OffsetEvt);
        m_CurEvtLen=*pword; //If a MDF Header is on 2 different chunks, this pointer is correct anyway
        if(m_CurEvtLen == 0) {
            msgLog << MSG::WARNING << "Evt size equals 0" << endmsg;
        //    displayBuf(slice+m_OffsetEvt, 56); 
            return StatusCode::FAILURE;
        }
  
        m_CurEvtIte = 0;
        if(m_OffsetEvt + m_CurEvtLen > size)
        {
            /// Event on 2 slices
            memcpy(m_CurEvent, (slice+m_OffsetEvt), size-m_OffsetEvt);         
            m_CurEvtIte=size-m_OffsetEvt;
            m_OffsetEvt += m_CurEvtLen;
                 
        }  
        else
        {
            /// Push Event and go to next
            StatusCode sc = pushEvent(slice+m_OffsetEvt, m_CurEvtLen);
            if(sc.isRecoverable())
            {   
                msgLog << MSG::INFO << "End of run : exiting decodeMDF" << endmsg;
                return sc;
            }
            if(sc.isFailure()) 
            {
                msgLog << MSG::ERROR << "Failed to send event to the injector" << endmsg;
                return sc;
            }  
              
            m_OffsetEvt += m_CurEvtLen;
        }
    }
    return StatusCode::SUCCESS;
}


StatusCode SLICEReaderSvc::readFile() 
{
    static MsgStream msgLog(msgSvc(), name());

    /// XXX do class attribute stats instead
    struct timeval tvbef, tvaf;
    bzero(&tvaf, sizeof(tvaf));
    bzero(&tvbef, sizeof(tvbef));

    gettimeofday(&tvbef, NULL);
    ///XXX

    int fd = 0;
    int nbSlices=1;
    int ret =0;

    fd = open(m_InputFiles[m_CurFile].c_str(), O_RDONLY);
    if(fd <0) {
        msgLog << MSG::ERROR << "Could not open file " << m_InputFiles[m_CurFile] << endmsg;
        return StatusCode::FAILURE;    
    }

    /// First slice, we assume size of event is the first byte
    ret = read(fd, m_Buffer, TWOMB); 
    if(ret <= 0) // Then empty file
    {
        msgLog << MSG::ERROR << "Empty file " << m_InputFiles[m_CurFile] << endmsg;
        close(fd);
        return StatusCode::FAILURE; 
    }

    while (ret!=0 && m_ReaderState == RUNNING) {
        ++nbSlices;
        StatusCode sc=decodeMDF(m_Buffer, ret);
        if(sc.isFailure()) {
            
            if(sc.isRecoverable()) {
                msgLog << MSG::INFO << "End of run : exiting readFile" << endmsg;
            }
            else {
                msgLog << MSG::ERROR << "Problem occured decoding MDF file "<< m_InputFiles[m_CurFile] << endmsg;
                close(fd);
            }
            return sc;
        }

        ret = read(fd, m_Buffer, TWOMB);
    }
 
    close(fd);
    
    /// XXX
    gettimeofday(&tvaf, NULL);
    
    int sec = (tvaf.tv_sec - tvbef.tv_sec);
    int usec = (tvaf.tv_usec - tvbef.tv_usec);
    printf("File %s, containing %d times 2 MBytes, were read from in %d s %d us\n, ", m_InputFiles[m_CurFile].c_str(), nbSlices, sec , usec);
    ///XXX


    msgLog << MSG::INFO << "End of readfile procedure" << endmsg;
    return StatusCode::SUCCESS;
}
    
StatusCode SLICEReaderSvc::run()
{
//    m_ReaderState = RUNNING;
    static MsgStream msgLog(msgSvc(), name());

    while(m_ReaderState != RUNNING){
        switch(m_ReaderState) {
        case STOPPED:
        case NOT_READY:
            msgLog << MSG::DEBUG << "Exiting from reading loop" << endmsg;
            return StatusCode::SUCCESS;
        case READY:
            MEPRxSys::microsleep(100000); // 100 ms
            break;
        default: continue;
        }
    }

    msgLog << MSG::INFO << "Reader running" << endmsg;
 
    StatusCode sc=StatusCode::SUCCESS;
    do
    {
        for(m_CurFile = 0; (unsigned int) m_CurFile < m_InputFiles.size() && m_ReaderState==RUNNING && sc.isSuccess(); ++m_CurFile) {
            sc=readFile();
        }
    }
    while(m_LoopOverFiles && m_ReaderState == RUNNING && sc.isSuccess()); 

   
    if(sc.isFailure()) {
        if(sc.isRecoverable())
        {
            msgLog << MSG::INFO << "End of run : exiting run" << endmsg;
            return StatusCode::SUCCESS;
        }
        else msgLog << MSG::ERROR << "File reading procedure failed" << endmsg;
        return sc;
    }   
 
    msgLog << MSG::INFO << "End of run" << endmsg; 
    return StatusCode::SUCCESS;
}

    
StatusCode SLICEReaderSvc::finalize()
{
    MsgStream msgLog(msgSvc(),name());
    m_ReaderState = NOT_READY;
    m_LoopOverFiles = false;
    msgLog << MSG::INFO << "Finalization" << endmsg;
 
    if(m_IncidentSvc) {
        m_IncidentSvc->removeListener(this);
        m_IncidentSvc->release();
        m_IncidentSvc = 0;
    }

    if (m_MonSvc) {
        m_MonSvc->undeclareAll(this);
        m_MonSvc->release();
        m_MonSvc = 0;
    }

//    mbm_free_space(m_BMID);
    if(m_BMID != MBM_INV_DESC) 
        mbm_exclude(m_BMID);

    return StatusCode::SUCCESS;
}


void SLICEReaderSvc::handle(const Incident& incident){
  MsgStream msgLog(msgSvc(),name());
  msgLog << MSG::DEBUG << "Got incident:" << incident.source() << " of type " << incident.type() << endmsg;
  if (incident.type() == "DAQ_CANCEL")  {  
      if(m_ReaderState != STOPPED) 
      {
          m_ReaderState = STOPPED;
          if(mbm_cancel_request(m_BMID) != MBM_NORMAL) {
              msgLog << MSG::ERROR << "Could not cancel writing request" << endmsg;
          }
          msgLog << MSG::DEBUG << "request cancelled" << endmsg;
      } 
  }
  else if (incident.type() == "DAQ_ENABLE") {
      m_ReaderState = RUNNING;
     
  }
}

StatusCode SLICEReaderSvc::pushEvent(char *event, int size)
{

    static MsgStream msgLog(msgSvc(), name());

    Requirement r;
    if(m_Req.size()>0) {
        r.parse(m_Req[0]);
    }
    else return StatusCode::FAILURE;

    char *buf;
    if(mbm_get_space_a(m_BMID, size, (int **)&buf, NULL, this) != MBM_NORMAL)
    {
        if(m_ReaderState != RUNNING) {
            msgLog << MSG::INFO << "End of run : mgm_get_space cancelled" << endmsg;
            return StatusCode::RECOVERABLE;
        }
        msgLog << MSG::ERROR << "mbm_get_space_a failed" << endmsg;
        return StatusCode::FAILURE;
    }
     
    memcpy(buf, event, size);

    if(mbm_declare_event_and_send(m_BMID, size, r.evtype, r.trmask, 0, (void **) &buf, &size, m_PartitionID) != MBM_NORMAL)
    {
        if(m_ReaderState != RUNNING) {
            msgLog << MSG::INFO << "End of run : mgm_declare_event_and_send cancelled" << endmsg;
            return StatusCode::RECOVERABLE;
        }
        msgLog << MSG::ERROR << "mbm_declare_event_and_send failed" << endmsg;
        return StatusCode::FAILURE;
    } 


    return StatusCode::SUCCESS; 
}

#endif /*_WIN32*/
