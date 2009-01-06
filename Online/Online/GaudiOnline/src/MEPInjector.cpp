//  ====================================================================
//  MEPInjector.cpp
//  --------------------------------------------------------------------
//
//  Author    : David Svantesson
//              Jean-Christophe Garnier 
//
//  Description : 
//  
//  This file contains the implementation of the MEP injector. It consists 
//  in a GAUDI algorithm which reads raw events from MDF files, produces
//  Tell1's MEPs, and then it sends them to the HLT.
//
//  It implements also a MEP request manager and an Odin MEP manager in order
//  to be integrated in the DAQ network.
//
//  Each task is made by a dedicated thread. 
//
//  ====================================================================

#ifndef _WIN32

// Include files from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/MEPRxSys.h"
#include "GaudiOnline/MEPHdr.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Chrono.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPWriter.h"
#include "MDF/MEPEvent.h"
#include <MDF/MEPFragment.h>
#include "Event/RawEvent.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiOnline/MEPRxSvc.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Stat.h"

#include "GaudiOnline/MEPInjector.h"


#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/poll.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>


#include <math.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#ifdef _WIN32
#define ERRMSG(a,x) do {  \
  a << MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __FUNCDNAME__ << ":" << __LINE__ << endmsg;} while(0);
#else
#define ERRMSG(a,x) do {  \
  (a) << MSG::ERROR << (x) << " " << " in " << __PRETTY_FUNCTION__<< ":"  << __FILE__<< ":(" << __LINE__ << ")" << endmsg;} while(0);
#endif


#define PUBCNT(name, desc) do {m_ ## name = 0; m_MonSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_MonSvc->declareInfo(#name, "I", & m_ ## name [0], m_ ## name.size() * sizeof(int), desc, this);} while(0);


//XXX
static std::map<int, std::map<int, int> > BKSTATMAP;  
static int BKSTATI=0;
void saveHisto();

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb, MEPInjector)
#define MEPEVENTOFFSET		 4
#define IPHDRSZ			20
#define MEPHDRSZ	        12
#define ODFRAGSZ		52 /// Odin Fragment size, with Frag Header and only 1 bank
#define BKHDRSZ			 8
#define FRAGHDRSZ		 4
using namespace LHCb;

static bool finalSent=true;;

typedef union {
    u_int32_t word;
    struct {
	u_int16_t evtid;
	u_int16_t len;
    } part;
} EventHdr_t;

/// Streambuffer manages correctly the memory, reallocating if need more than it currently has, and freeing once the streambuffer is destroyed, i.e. in the end of the program because the streambuffer is an attribute of the MEPInjector
/// This function also sets the real size of the MEP in the first 4 bytes of the buffer, but the MEPEvent constructor of encodeMEP function sets it to 0 ... before to set it back to the same value
static void *extendBuffer(void *p, size_t len)
{
    StreamBuffer *s = (StreamBuffer *) p;
    s->reserve(len);
    unsigned int *pui = (unsigned int *) s->data();
    *pui = len;
    return s->data();
}





MEPInjector::MEPInjector(const std::string & name, ISvcLocator * pSvcLocator):Algorithm(name,
	  pSvcLocator), m_IncidentSvc(0), m_MonSvc(0) 
{
    declareProperty("RemoveDAQStatus", m_RemoveDAQStatus = true);	// To remove bank header (we want this)
    declareProperty("PackingFactor", m_PackingFactor = 10);	 


    declareProperty("MEPProto", m_MEPProto = 0xF2);
    declareProperty("MEPReqProto", m_MEPReqProto = 0xFD);


    declareProperty("HLTIPAddrTo", m_HLTStrIPAddrTo = "0.0.0.0");

    declareProperty("MEPSockBuf", m_MEPBufSize = 0x100000);
    declareProperty("MEPReqSockBuf", m_MEPReqBufSize = 0x10000);

    declareProperty("L0ID", m_L0ID = 0x00000000);
    declareProperty("AutoMode", m_AutoMode = true);

    declareProperty("OdinIf", m_OdinIfIPAddr = "0.0.0.0"); 
    declareProperty("OdinIPAddr", m_StrOdinIPAddr = "0.0.0.0");
    declareProperty("HLTIf", m_HLTIfIPAddr = "0.0.0.0");
    
    declareProperty("OdinEthInterface", m_OdinEthInterface=m_OdinIfIPAddr[m_OdinIfIPAddr.length()]);
    declareProperty("HLTEthInterface", m_HLTEthInterface=m_HLTIfIPAddr[m_HLTIfIPAddr.length()]);
    

    declareProperty("TimeOut", m_TimeOut = -1);

    /// Partition ID set to all detectors by default, but copied from Odin in TFC mode
    declareProperty("PartitionID", m_PartitionID = 0x7fff);

    declareProperty("EventBuffer", m_EvtBuf = 300000);

    declareProperty("Tell1Boards", m_Tell1Boards);

    m_InjState = NOT_READY;
}



/// Initialize
StatusCode MEPInjector::initialize()
{
    static MsgStream log(msgSvc(), name());	//Message stream for output


    for(std::vector<std::string>::iterator ite=m_Tell1Boards.begin(); ite != m_Tell1Boards.end(); ite+=3) 
    {
        u_int32_t tell1id = MEPRxSys::IPStringToBits(*ite);
        /// Seems to be managed correctly alone if it needs to be increased        
        /// Cause a tell1 can send different bank types (XXX check with niko), impossible to compute the length of the MEP from the length of the first bank found, 5000 seems enough to contain the biggest banks and some others  
        void *memory = extendBuffer(&(m_MapStreamBuffers[tell1id]), m_PackingFactor * 5000 );
        bzero(memory, m_PackingFactor * 5000);
        m_MapTell1MEPs[tell1id] = new(memory) MEPEvent(0);
        m_MapTell1MEPs[tell1id]->setSize(0);   

    }

    if(!service("IncidentSvc", m_IncidentSvc).isSuccess()) {
        ERRMSG(log, "Failed to access to incident service.");
        return StatusCode::FAILURE;
    }
    m_IncidentSvc->addListener(this, "DAQ_CANCEL");
    m_IncidentSvc->addListener(this, "DAQ_ENABLE");
   
    if(!service("MonitorSvc", m_MonSvc).isSuccess()) {
        ERRMSG(log, "Failed to access to monitor service.");
        return StatusCode::FAILURE;
    }
    else {
      if (setupCounters()) {
        return error("Failed to setup counters");
      }
    }

    m_OdinBufSize = m_MEPBufSize;
 

    m_OdinEthInterface = m_OdinIfIPAddr[m_OdinIfIPAddr.length() -1] -48;
    m_HLTEthInterface = m_HLTIfIPAddr[m_HLTIfIPAddr.length() -1] -48;
    

    /// Try to trap all signals, parent process should if child process wants
/*
    sigset_t injSet;
    sigfillset(&injSet); 
    sigdelset(&injSet, SIGILL);
    sigdelset(&injSet, SIGFPE);
    sigdelset(&injSet, SIGSEGV);
    sigdelset(&injSet, SIGBUS);
    sigdelset(&injSet, SIGKILL);
    sigdelset(&injSet, SIGSTOP);

    if(pthread_sigmask(SIG_BLOCK, &injSet, NULL))
    {   
        ERRMSG(log, "sig mask error");
        return StatusCode::FAILURE;
    }
*/

    if (m_MEPProto < 0 || m_MEPProto > 255) {
	ERRMSG(log, "IPProtoIn is an unsigned 8 bit quantity");
        return StatusCode::FAILURE;
    }
    std::string errmsg;

    m_InjectorStop= false;
    m_ManagerStop = false;
/*
    if (MEPRxSys::parse_addr(m_StrOdinIPAddr, m_BitOdinIPAddr) != 0) {
        std::string msg;
        if(MEPRxSys::addr_from_name(m_StrOdinIPAddr, m_BitOdinIPAddr, msg) != 0) {
            ERRMSG(log, " Odin address conversion : " + msg);
            return StatusCode::FAILURE;
        }
    }
*/
    m_BitOdinIPAddr = MEPRxSys::IPStringToBits(m_StrOdinIPAddr);

    /// This socket allows us to edit IP header
    if ((m_ToHLTSock =  MEPRxSys::open_sock_arb_source(m_MEPProto, m_MEPBufSize, errmsg)) < 0)    {
        ERRMSG(log, "Failed to open socket:" + errmsg);
	return StatusCode::FAILURE;
    }


    m_HLTIPAddrTo=0;

    m_OdinTell1ID = -1;

    

    if(sem_init(&m_RawEventsCount, 0, 0) == -1) {
            ERRMSG(log, "Failed to initialize semaphore");
            perror("sem_init");
            exit(errno);
        }

    if (pthread_mutex_init(&m_SyncReadProcess, NULL)) {
        ERRMSG(log, "Failed to initialize mutex");
        return StatusCode::FAILURE;
    }


    if (!m_AutoMode) {

        void *memory = extendBuffer(&m_OdinData, m_OdinBufSize);
        m_OdinMEP = new(memory) MEPEvent(0);

        if (m_OdinMEP == NULL) {
            ERRMSG(log, "Odin MEP Buffer Allocation failed");
            return StatusCode::FAILURE;
        }

        log<<MSG::DEBUG << "IP to send to odin : "<<m_OdinIfIPAddr << " interface : "<<m_OdinEthInterface << endmsg; 

	if ((m_FromOdinSock = MEPRxSys::open_sock(m_MEPProto, m_OdinBufSize, m_OdinEthInterface, m_OdinIfIPAddr, false, errmsg)) < 0) {
            ERRMSG(log, "Failed to open socket:" + errmsg);
            return StatusCode::FAILURE;
	}

        if ((m_FromHLTSock = MEPRxSys::open_sock(m_MEPReqProto, 
                                              m_MEPReqBufSize, 
					      m_HLTEthInterface, m_HLTIfIPAddr, 
                                              true, errmsg)) < 0) {
            ERRMSG(log, "Failed to open socket:" + errmsg);
            return StatusCode::FAILURE;
        }
 
        if ((m_ToOdinSock = MEPRxSys::open_sock(m_MEPReqProto, 
                                              m_MEPReqBufSize, 
                                              m_OdinEthInterface, m_OdinIfIPAddr, 
                                              true, errmsg)) < 0) { 
            ERRMSG(log, "Failed to open socket:" + errmsg);
            return StatusCode::FAILURE;
        }


        if(sem_init(&m_OdinCount, 0, 0) == -1) {
            ERRMSG(log, "Failed to initialize semaphore");
            perror("sem_init");
            exit(errno);
        }
  
        if(sem_init(&m_MEPReqCount, 0, 0) == -1) {
            ERRMSG(log, "Failed to initialize semaphore");
            perror("sem_init");
            exit(errno);
        }  


	if (pthread_mutex_init(&m_SyncMainOdin, NULL)) {
            ERRMSG(log, "Failed to initialize mutex");
            return StatusCode::FAILURE;
	}
        if (pthread_mutex_init(&m_SyncReqOdin, NULL)) {
            ERRMSG(log, "Failed to initialize mutex");
            return StatusCode::FAILURE;
        }


	if(pthread_create(&m_ThreadOdinMEPManager, NULL, OdinMEPMgrStartup, this)) {
	    ERRMSG(log, "Failed to start MEP Odin Manager Thread ");
            return StatusCode::FAILURE;
	}
	if(pthread_create(&m_ThreadMEPReqManager, NULL, MEPReqMgrStartup, this)) {
	    ERRMSG(log, "Failed to start MEP Request Manager Thread ");
            return StatusCode::FAILURE;
	}
/*
        StatusCode sc;
        sc = getHLTInfo();
        if(sc.isFailure()) {
            ERRMSG(log, " Selecting a HLT");
            return StatusCode::FAILURE;
        }

        sc= getOdinInfo();
        if(sc.isFailure()) 
        {
            ERRMSG(log, " Copying data from Odin MEP");
            return StatusCode::FAILURE;
        }
*/
    }
    else
        m_EvtID = m_L0ID & 0x0000ffff;


    if(pthread_create(&m_InjectorProcessing, NULL, injectorProcessingStartup, this)) {
        ERRMSG(log, "Failed to start Injector Processing Thread ");
        return StatusCode::FAILURE;
    }

    log << MSG::DEBUG << "Injector initialized" << endmsg; 
//    log << MSG::WARNING<<"CKTAG" <<"L0ID;type;src;IP"<<endmsg; 
    m_InjState = READY;

    return StatusCode::SUCCESS;
}


/// Read an event from a MDF file, and store each banks in the future MEP
StatusCode MEPInjector::readEvent()
{
    static int icalled = 0;
    icalled++;

    static MsgStream log(msgSvc(), name());	//Message stream for output

    static unsigned int nbEv =0;
    int tell1id = 0;
    int len = 0;
    int retVal=0;
   
    /// Wait for an event to read from the queue
    /// The loop is compulsory because POSIX semaphore are awaken by any signal  
//    while((retVal=sem_wait(&m_RawEventsCount))==-1 && errno == EINTR) continue;

    retVal=sem_wait(&m_RawEventsCount);
    if(retVal==-1)
    {   
        ERRMSG(log, "Waiting on the semaphore");
        perror("sem_wait");
        exit(errno);
    }

    /// The queue is shared between this reader process and a writer process, so it is a critical resource 
    /// protected by a mutex

    if(pthread_mutex_lock(&m_SyncReadProcess)) {
        ERRMSG(log, " Locking mutex");
        return StatusCode::FAILURE;
    }

    RawEvent *raw = NULL;

    if(m_QueueRawEvents.size()) {
        raw = m_QueueRawEvents.front();
        m_QueueRawEvents.pop();
    }
    else return StatusCode::RECOVERABLE; /// This happens only in the end of the process, when all files were read and all event were sent, this is how the injector knows when to close everything 


    if(pthread_mutex_unlock(&m_SyncReadProcess)) {
        ERRMSG(log, " Unlocking mutex");
        return StatusCode::FAILURE;
    }

    if(! raw) {
        log << MSG::ERROR << "Invalid raw event" << endmsg;
    }
    else
    {

        if (!m_AutoMode) {
	    std::vector < RawBank * >bnks = raw->banks(RawBank::ODIN);
	    for (std::vector < RawBank * >::iterator ib = bnks.begin();
		 ib != bnks.end(); ++ib) {
		raw->removeBank(*ib);
	    }
	}



        std::vector < RawBank * >bnks = raw->banks(RawBank::DAQ);
        for (std::vector < RawBank * >::iterator ib = bnks.begin();
            ib != bnks.end(); ++ib) {
            raw->removeBank(*ib);
        }

	for (int bkType = RawBank::L0Calo; bkType < RawBank::LastType;
	     ++bkType) {
	    const std::vector < RawBank * >&bks =
		raw->banks(RawBank::BankType(bkType));

	    for (unsigned int i = 0; i < bks.size(); ++i) {
		RawBank *hdr = bks[i];

                std::map<int, int>::iterator tite = BKSTATMAP[bkType].find(hdr->sourceID());
                if(tite == BKSTATMAP[bkType].end())
                    BKSTATMAP[bkType][hdr->sourceID()]=1;
                else BKSTATMAP[bkType][hdr->sourceID()]+=1;
                 
  
                if (hdr->totalSize() == 0 || hdr->magic() != 0xCBCB ) {
		    ERRMSG(log, "Severe error in fragment");
		    return StatusCode::FAILURE;  
		}
 
                MEPEvent *curmep = NULL;
                char *ccur = NULL;
                MEPHdr *mh = NULL;
                MEPFrgHdr *mf = NULL; 

                
		tell1id = getTell1IP(hdr->type(), hdr->sourceID());
		if (tell1id == 0) { 
//                    log << MSG::DEBUG<<"Unknown source "<<hdr->sourceID()<<" or type "<<RawBank::typeName(hdr->type()) << " no tell1 found, execution continue "<<endmsg; 
		    continue;
		}

//XXX
/*
                if(hdr->type() != RawBank::OT)
                    log << MSG::WARNING<<"CKTAG" <<(m_L0ID + nbEv) <<  ";" << RawBank::typeName(hdr->type()) << ";" << hdr->sourceID() << ";"<<tell1id << endmsg;
*/

		len = hdr->totalSize();	/// I need len = hdr + body + padding, because the bank array contains the padding    
 
                /////////////////////////////////////////
                // Encoding while reading
                ////////////////////////////////////////
                std::map<unsigned long, MEPEvent *>::iterator ite = m_MapTell1MEPs.find(tell1id);
                if(ite == m_MapTell1MEPs.end()) 
                {
                    continue; // it means that we have to ignore this tell1, as the map is initialized in the initialize procedure
                   
                }

                curmep = m_MapTell1MEPs[tell1id]; 
                ccur = (((char *) curmep) + MEPEVENTOFFSET);
  	        mh = (MEPHdr *) (ccur + IPHDRSZ);
                   
                /// Use the mepevent 4 first byte as "pointer" to the beginning of the current fragment, to set up the header
                /// Use the MEp tot len as the current tot len 
                /// Only when reading the last event of a MEP(or the first but if so has to be updated each time ... ), same with Fragment Header and banks
                /// Set the MEP Header
                if(nbEv == 0) 
                {
                    /// XXX OT FIX
                    if(bkType == RawBank::OT 
                       || bkType == RawBank::OTError 
                       || bkType == RawBank::OTRaw ) 
                    { 
                         bzero(curmep, MEPHDRSZ + IPHDRSZ + MEPEVENTOFFSET + m_PackingFactor*(BKHDRSZ + FRAGHDRSZ) + m_PackingFactor*2000 );
                    }
                    else
                        /// Only sets the first headers to 0, and then everything should be fine 
                        bzero(curmep, MEPHDRSZ + IPHDRSZ + MEPEVENTOFFSET + BKHDRSZ + FRAGHDRSZ );

                    curmep->setSize(IPHDRSZ + MEPHDRSZ ); 
                    mh->m_totLen  = MEPHDRSZ;
                    mh->m_partitionID = m_PartitionID;
                    
                    mh->m_l0ID = m_L0ID;
                }
                mh->m_nEvt = nbEv+1;
                
                /// Sets the Fragment header, care, multi banks per fragments ... 
                ccur = (((char *) curmep) + curmep->size() + MEPEVENTOFFSET);    
                mf = ( MEPFrgHdr *) ccur;
                /// The first bank of a fragment is encountered if we still point to the old fragment with mf and the evtId has been incremented
                /// Or if the evtId = 0 and the size of the fragment has never been written 
                if(mf->m_l0IDlow != m_EvtID || (m_EvtID ==0 && mf->m_len == 0 )  ) /// If a bank has not been already registred for this event, i.e. first bank for cur event
                {
                    /// First update the pointer to beginning of cur frag = end of previous frag
                    curmep->setSize(mh->m_totLen + IPHDRSZ);
                    ccur = (((char *) curmep) + curmep->size() + MEPEVENTOFFSET);
                    mf = ( MEPFrgHdr *) ccur; 
                    mf->m_l0IDlow = m_EvtID;
                    mf->m_len = len ;
                    mh->m_totLen += FRAGHDRSZ;
                }
                else
                {
                    mf->m_len +=len;
                }
                ccur = ((char *)curmep) + MEPEVENTOFFSET + IPHDRSZ + mh->m_totLen; 
                mh->m_totLen +=len;
 
                /// Copy the bank opaque body + hdr + padding
                memcpy(ccur, hdr, len); 
	    }			//end for banks
	}			//end for banksType
        if(++nbEv == m_PackingFactor)
        {


            nbEv=0;
        }

        raw->release();
    } //end if(raw) 
    return StatusCode::SUCCESS;
}

/// To be called with 12 and 16, for OT A and OT C, when pf reached in readThenSend 

void MEPInjector::OTFIX(int side) {

     u_int32_t otipstart = (192&0x000000ff) | ((169<<8)&0x0000ff00) | ((side<<16)&0x00ff0000) | ((1<<24)&0xff000000);
     int nbOtIP=24;
     u_int32_t otinc = ((1<<24)&0xff000000);   
     int inc=0; 
                
     MEPBkHdr emptyBk;
     emptyBk.m_type=RawBank::OT;
     emptyBk.m_version=0;
     emptyBk.m_sourceID = inc+1;
     emptyBk.m_len=8;
     emptyBk.m_magic = 0xCBCB;      

     MEPFrgHdr emptyFrg;
     emptyFrg.m_len=8;
    

     for(int otipcur=otipstart; inc < nbOtIP; ++inc, otipcur+=otinc) 
     {  
            MEPEvent *curmep = m_MapTell1MEPs[otipcur];
           
            char *ccur = (((char *) curmep) + MEPEVENTOFFSET);
            MEPHdr *mh = (MEPHdr *) (ccur + IPHDRSZ);
         
            /// XXX OT FIX
		/// First, check event count in MEPHdr, 
                /// Then count effective number of frags
                /// If offset, decrement evt id of the frag to get a sequence with no hop
                /// Fill end of MEP with empty banks
 
                mh->m_nEvt = m_PackingFactor;
                mh->m_l0ID = m_L0ID;
                mh->m_totLen = MEPHDRSZ; 
                u_int32_t otl0id = m_L0ID;
                ccur = (((char *) curmep) + MEPEVENTOFFSET + MEPHDRSZ + IPHDRSZ);
                MEPFrgHdr *mf = (MEPFrgHdr *) ccur; 
                unsigned int otnbev = 0;  
                
                while(mf->m_len != 0){ //tells that every bank present were put as a sequence of fragment, to remove hops
                    mf->m_l0IDlow = otl0id + otnbev;
                    mh->m_totLen+=mf->m_len + FRAGHDRSZ; 
                        
                    ++otnbev;
                    ccur = ccur + FRAGHDRSZ + mf->m_len;    
                    mf= (MEPFrgHdr*) ccur; 
                } 

                while(otnbev < m_PackingFactor) //append empty banks
                {
                    emptyFrg.m_l0IDlow=otl0id + otnbev;
                    memcpy(ccur, &emptyFrg, sizeof(MEPFrgHdr));
                    ccur += FRAGHDRSZ;
                    memcpy(ccur, &emptyBk, emptyBk.m_len);
                    ccur += emptyBk.m_len; 
                    mh->m_totLen= mh->m_totLen + sizeof(MEPFrgHdr) + emptyBk.m_len;
                    ++otnbev;
                }
     } 
}

/** This function prints out the contents of a MEP.
 *
 *  ( The output is printed in hexadecimal numbers with 16 bytes on each row )
 */
void MEPInjector::printMEP(MEPEvent *me, int size) {
  //int size = me->size();
  const char* data = me->start();
  
  for (int i=0; i< (size+20); i++) {
    if ((i % 16)==0) {
      std::cout << std::endl;
      std::printf("%08X :",i);
      
    }
    std::printf(" %02X",(unsigned char) data[i]);
  }
  std::cout << std::endl;
}



/// Main execution
/// It reads data from a MDF transient store
/// Once it has reached the packing factor, it produces the MEP and then 
/// send it
StatusCode MEPInjector::execute()
{

    Chrono chrono(chronoSvc(), "execute");
    static MsgStream log(msgSvc(), name());     //Message stream for output
  
    SmartDataPtr < RawEvent > raw(eventSvc(), RawEventLocation::Default);


    // InjectorStop should be set by the handler of the incident DAQ_CANCEL, which is the cancel command forwwarded by the GaudiTask specialisation which encapsulate our algorithm, sehr elegant
    while(m_QueueRawEvents.size() > 20000 && m_InjState == RUNNING)
    {
        MEPRxSys::microsleep(1000);
    }    

   /// To avoid synchronisation problems ...


    if (!raw) {
        log << MSG::ERROR << "Invalid raw event " << endmsg;
    }
    else {
                                                   /// The use of the semaphore is too slow here

        if(pthread_mutex_lock(&m_SyncReadProcess)) {
            ERRMSG(log, " Locking mutex");
            return StatusCode::FAILURE;
        }
       
        RawEvent * nraw = new RawEvent();
        nraw->addRef();
        for (int bkType = RawBank::L0Calo; bkType < RawBank::LastType;
             ++bkType) {
            if(bkType == RawBank::DAQ) continue;
            const std::vector < RawBank * >&bks =
                raw->banks(RawBank::BankType(bkType));

            for (unsigned int i = 0; i < bks.size(); ++i) {
                RawBank *hdr = bks[i];
                nraw->addBank(hdr); 
            }
        }

        m_QueueRawEvents.push( nraw );
      
        m_TotEvtsRead++;
 
        if(sem_post(&m_RawEventsCount)==-1)
        {   
            ERRMSG(log, "Posting on the semaphore");
            perror("sem_post");
            exit(errno);
        }
        
        if(pthread_mutex_unlock(&m_SyncReadProcess)) {
            ERRMSG(log, " Unocking mutex");
            return StatusCode::FAILURE;
        }
/*
        int sval=0;
        do
        {
            if(sem_getvalue(&m_RawEventsCount, &sval)) 
            {
                ERRMSG(log, "Getting value of the semaphore");
                perror("sem_getvalue");
                exit(errno);
            }
            MEPRxSys::microsleep(1); /// 1 us
        }  
        while(sval>m_EvtBuf);
*/
    }

    return StatusCode::SUCCESS;
}


StatusCode MEPInjector::injectorProcessing()
{
    static MsgStream log(msgSvc(), name());

    /// Try to trap all signals
/*
    sigset_t injSet;
    sigfillset(&injSet);
   
    if(pthread_sigmask(SIG_BLOCK, &injSet, NULL)) 
    {
        ERRMSG(log, "sig mask error");
        return StatusCode::FAILURE;
    }
*/
    StatusCode sc = StatusCode::SUCCESS;
    
    if(!m_AutoMode) {
        sc = getHLTInfo();
        if(sc.isRecoverable()) {
           log << MSG::DEBUG << "getHLTINFO Exited on Recover" << endmsg; 
           return StatusCode::SUCCESS;
        }
        else if(sc.isFailure()) {
            ERRMSG(log, " Selecting a HLT");
            return StatusCode::FAILURE;
        }

        sc= getOdinInfo();
        if(sc.isRecoverable()) {
           log << MSG::DEBUG << "getOdinINFO Exited on Recover" << endmsg; 
           return StatusCode::SUCCESS;
        } 
        else if(sc.isFailure()) 
        {
            ERRMSG(log, " Copying data from Odin MEP");
            return StatusCode::FAILURE;
        }
    }

    while (sc.isSuccess() && !m_InjectorStop) {
        if(m_InjState != RUNNING) {
            MEPRxSys::microsleep(1000); 
            continue;   //If injector is not running, idle      
        }
 
        sc = readThenSend();
        if(sc.isRecoverable()) 
        {
            return StatusCode::SUCCESS;
        }
        else if (sc.isFailure())
        {
            ERRMSG(log, "Main processing failed, aborting !");
            return sc;
        }
    }
 

    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::readThenSend()
{
    static MsgStream log(msgSvc(), name());	//Message stream for output
    StatusCode sc;
    static unsigned int nbEv = 1;	// Used in Odin Mode to read the L0Id of the good bank                            // Used to count nb event read
    MEPEvent *me = NULL;

    sc = readEvent();
    if (sc.isRecoverable() )  /// End of the job
    {
        log<<MSG::INFO<< "End of injection"<<endmsg; 
        return StatusCode::RECOVERABLE;
    }
    if (sc.isFailure()) {
	ERRMSG(log, " Reading an event from the shared queue");
        return StatusCode::FAILURE;
    }
  
    finalSent = true;
 
    //If we have reached packing factor
    if (nbEv == m_PackingFactor ) {
        OTFIX(12);
        OTFIX(16);

        //XXX 
        //sleep(10); //MEPRXTEST so i get the time to kill

      
        finalSent = false;
	if (!m_AutoMode) {

            u_int32_t addrFrom = m_BitOdinIPAddr + ((32<<24)&0xff000000); 
            log<<MSG::DEBUG<<__FUNCTION__<<"MEP SIZE : " << m_OdinMEP->size() << endmsg;

	    sc = sendMEP(addrFrom, m_OdinMEP);
            if(sc.isFailure()) {
                ERRMSG(log, "Could not send Odin MEP");
                return StatusCode::FAILURE;
            }
	}

        for(std::map<unsigned long, MEPEvent *>::iterator iteTell1ID = m_MapTell1MEPs.begin(); iteTell1ID != m_MapTell1MEPs.end(); ++iteTell1ID)
        { 
            me = iteTell1ID->second;
	    if(me != NULL)
            {
                sc = sendMEP(iteTell1ID->first, me);
   	        if (sc.isFailure()) {
	  	    ERRMSG(log, " sendMEP error for Tell1 IP " + iteTell1ID->first);
                    return StatusCode::FAILURE;
                }
	    }
	} /// end for

	if (!m_AutoMode) {
            /// Wait for new information from Odin and HLT
            /// Actually I hope these is no wait 
            StatusCode sc; 
            sc = getHLTInfo();
            if(sc.isRecoverable()) return sc;
            if(sc.isFailure()) {
                ERRMSG(log, " Selecting a HLT");
                return StatusCode::FAILURE;
            }

            sc= getOdinInfo();
            if(sc.isRecoverable()) return sc;
            if(sc.isFailure()) {
                ERRMSG(log, " Copying data from Odin MEP");
                return StatusCode::FAILURE;
            } 

	} else {
	    /// The IP datagram ID is set to the least significant 15 bits of the first 
	    /// event ID of the MEP, which are the least significant 15 bits of the 
	    /// MEP event id   
	    /// If we are in ODIN mode, this Id should be given by ODIN, 
	    /// else we increment  it each time, starting from a job option valu
	    m_L0ID += m_PackingFactor;
	    m_EvtID = 0x0000ffff & m_L0ID;
	} /// end if(!m_AutoMode)  
	nbEv = 1;

        m_TotEvtsSent += m_PackingFactor;
        gettimeofday(&m_CurTime, NULL); 
        m_TotElapsedTime = (m_CurTime.tv_sec - m_InitTime.tv_sec)*1000 + (m_CurTime.tv_usec - m_InitTime.tv_usec)/1000 ;
 
        ///XXX For networking stat purposes
        if((m_TotEvtsSent)%2000 == 0) {
            //log << MSG::DEBUG 
            log << MSG::INFO <<m_TotEvtsSent<<"events sent in "<< m_TotElapsedTime << "ms" << endmsg;
        }

    } else {  ///Packing factor not reached
	if (m_AutoMode)
	    ++m_EvtID;
	else {
            MEPFragment *mepfrag = (MEPFragment *) ( ((char *) m_OdinMEP) + MEPEVENTOFFSET +
                                   IPHDRSZ + MEPHDRSZ + nbEv * ODFRAGSZ);
            m_EvtID = mepfrag->eventID();
            log << MSG::DEBUG << "EvtID read from Frag Header (and used) : " << m_EvtID << endmsg;
        }
	++nbEv;
    }///end if(packing_factor)

/*
    ///XXX To do histograms
    if(++BKSTATI > 30000) {
        saveHisto();
        exit(1);
    } 
*/
    return StatusCode::SUCCESS;
}


/// Get the MTU set in the system configuration, 
int MEPInjector::getMTU(int netdev)
{
    struct ifreq eth1req;
    char netdev_name[10];
    sprintf(netdev_name, netdev < 0 ? "lo" : "eth%d", netdev);
    strcpy(eth1req.ifr_name, netdev_name);
    ioctl(m_ToHLTSock, SIOCGIFMTU, &eth1req);

    return eth1req.ifr_mtu;
}


/// Get Tell1 IP, return 0 if no tell1 were found  
in_addr_t MEPInjector::getTell1IP(int type, int src)
{
    static MsgStream log(msgSvc(), name());	//Message stream for output
    
//    static stdmap<int, int> MuonMap
    in_addr_t ipNet= ( (192 + (169<<8))&0x0000ffff);
    switch (type) {
    case RawBank::L0CaloFull:	// 37
    case RawBank::L0CaloError:
    case RawBank::L0Calo:
	return (/*inet_addr("192.169.2.0")*/ ipNet|(6<<16)  | (src + 1 << 24));	//L0Calo = L0Suppression result for Hcal and Ecal
    case RawBank::L0DUError:
    case RawBank::L0DU:
	return (/*inet_addr("192.169.7.2")*/ ipNet|(7<<16) | (2 << 24));
    case RawBank::Velo:
        if(src >= 128) {      //L0PU
            ipNet |= (15<<16);
            switch(src) {
            case 128 : return (ipNet | (2<<24));  
            case 130 : return (ipNet | (3<<24));  
            case 129 : return (ipNet | (4<<24));  
            case 131 : return (ipNet | (5<<24)); 
            default : 
                //log << MSG::WARNING << "Unknown source "<< src << " for Velo bank type"<<endmsg;   
                return 0;
            }
        }
        else 
        {

        ipNet |= (11<<16);
        switch(src) {  /// Don't worry I did not write it by myself, script helps ;-), XXX replace by maps for TT and Velo cause the switch is to big so performances are crap for these bank types
        case 65: return (ipNet | (80<<24));
        case 1: return (ipNet | (61<<24));
        case 3: return (ipNet | (79<<24));
        case 67: return (ipNet | (62<<24));
        case 69: return (ipNet | (78<<24));
        case 5: return (ipNet | (60<<24));
        case 7: return (ipNet | (76<<24));
        case 71: return (ipNet | (59<<24));
        case 73: return (ipNet | (77<<24));
        case 9: return (ipNet | (63<<24));
        case 11: return (ipNet | (64<<24));
        case 75: return (ipNet | (82<<24));
        case 77: return (ipNet | (69<<24));
        case 13: return (ipNet | (81<<24));
        case 15: return (ipNet | (68<<24));
        case 79: return (ipNet | (83<<24));
        case 81: return (ipNet | (74<<24));
        case 17: return (ipNet | (87<<24));
        case 19: return (ipNet | (70<<24));
        case 83: return (ipNet | (88<<24));
        case 85: return (ipNet | (71<<24));
        case 21: return (ipNet | (89<<24));
        case 23: return (ipNet | (72<<24));
        case 87: return (ipNet | (84<<24));
        case 89: return (ipNet | (73<<24));
        case 25: return (ipNet | (85<<24));
        case 27: return (ipNet | (65<<24));
        case 91: return (ipNet | (86<<24));
        case 93: return (ipNet | (67<<24));
        case 29: return (ipNet | (92<<24));
        case 31: return (ipNet | (75<<24));
        case 95: return (ipNet | (90<<24));
        case 97: return (ipNet | (66<<24));
        case 33: return (ipNet | (91<<24));
        case 35: return (ipNet | (57<<24));
        case 99: return (ipNet | (54<<24));
        case 101: return (ipNet | (56<<24));
        case 37: return (ipNet | (52<<24));
        case 39: return (ipNet | (55<<24));
        case 103: return (ipNet | (51<<24));
        case 105: return (ipNet | (58<<24));
        case 41: return (ipNet | (53<<24));
        case 0: return (ipNet | (24<<24));
        case 64: return (ipNet | (35<<24));
        case 66: return (ipNet | (26<<24));
        case 2: return (ipNet | (32<<24));
        case 4: return (ipNet | (22<<24));
        case 68: return (ipNet | (33<<24));
        case 70: return (ipNet | (17<<24));
        case 6: return (ipNet | (31<<24));
        case 8: return (ipNet | (21<<24));
        case 72: return (ipNet | (34<<24));
        case 74: return (ipNet | (23<<24));
        case 10: return (ipNet | (39<<24));
        case 12: return (ipNet | (25<<24));
        case 76: return (ipNet | (36<<24));
        case 78: return (ipNet | (42<<24));
        case 14: return (ipNet | (37<<24));
        case 16: return (ipNet | (40<<24));
        case 80: return (ipNet | (30<<24));
        case 82: return (ipNet | (19<<24));
        case 18: return (ipNet | (38<<24));
        case 20: return (ipNet | (41<<24));
        case 84: return (ipNet | (29<<24));
        case 86: return (ipNet | (20<<24));
        case 22: return (ipNet | (27<<24));
        case 24: return (ipNet | (18<<24));
        case 88: return (ipNet | (28<<24));
        case 90: return (ipNet | (8<<24));
        case 26: return (ipNet | (12<<24));
        case 28: return (ipNet | (7<<24));
        case 92: return (ipNet | (15<<24));
        case 94: return (ipNet | (1<<24));
        case 30: return (ipNet | (9<<24));
        case 32: return (ipNet | (3<<24));
        case 96: return (ipNet | (11<<24));
        case 98: return (ipNet | (4<<24));
        case 34: return (ipNet | (10<<24));
        case 36: return (ipNet | (6<<24));
        case 100: return (ipNet | (14<<24));
        case 102: return (ipNet | (5<<24));
        case 38: return (ipNet | (13<<24));
        case 40: return (ipNet | (2<<24));
        case 104: return (ipNet | (16<<24));
        default : 
            //log << MSG::WARNING << "Unknown source "<< src << " for Velo bank type"<<endmsg;   
            return 0;
        }
        }
        break;
    case RawBank::Rich: // Lets assume that the relation is sourceID +1 for last digit, as they seems to do not care
        

 	return (/*inet_addr("192.169.14.0")*/ ipNet+(14<<16) | ((src)<< 24));
    case RawBank::TTFull:
    case RawBank::TTError:
    case RawBank::TTPedestal:
    case RawBank::TTProcFull:
    case RawBank::TT:
        ipNet |= (10<<16);
        switch(src) {
        case 0: return (ipNet | (17<<24));
        case 1: return (ipNet | (18<<24));
        case 2: return (ipNet | (19<<24));
        case 3: return (ipNet | (20<<24));
        case 4: return (ipNet | (1<<24));
        case 5: return (ipNet | (2<<24));
        case 6: return (ipNet | (3<<24));
        case 7: return (ipNet | (33<<24));
        case 8: return (ipNet | (34<<24));
        case 9: return (ipNet | (35<<24));
        case 10: return (ipNet | (36<<24));
        case 32: return (ipNet | (21<<24));
        case 33: return (ipNet | (22<<24));
        case 34: return (ipNet | (23<<24));
        case 35: return (ipNet | (24<<24));
        case 36: return (ipNet | (4<<24));
        case 37: return (ipNet | (5<<24));
        case 38: return (ipNet | (6<<24));
        case 39: return (ipNet | (37<<24));
        case 40: return (ipNet | (38<<24));
        case 41: return (ipNet | (39<<24));
        case 42: return (ipNet | (40<<24));
        case 64: return (ipNet | (25<<24));
        case 65: return (ipNet | (26<<24));
        case 66: return (ipNet | (27<<24));
        case 67: return (ipNet | (28<<24));
        case 68: return (ipNet | (7<<24));
        case 69: return (ipNet | (8<<24));
        case 70: return (ipNet | (9<<24));
        case 71: return (ipNet | (10<<24));
        case 72: return (ipNet | (11<<24));
        case 73: return (ipNet | (41<<24));
        case 74: return (ipNet | (42<<24));
        case 75: return (ipNet | (43<<24));
        case 76: return (ipNet | (44<<24));
        case 96: return (ipNet | (29<<24));
        case 97: return (ipNet | (30<<24));
        case 98: return (ipNet | (31<<24));
        case 99: return (ipNet | (32<<24));
        case 100: return (ipNet | (12<<24));
        case 101: return (ipNet | (13<<24));
        case 102: return (ipNet | (14<<24));
        case 103: return (ipNet | (15<<24));
        case 104: return (ipNet | (16<<24));
        case 105: return (ipNet | (45<<24));
        case 106: return (ipNet | (46<<24));
        case 107: return (ipNet | (47<<24));
        case 108: return (ipNet | (48<<24));
        default : 
            //log << MSG::WARNING << "Unknown source "<< src << " for TT bank type"<<endmsg;   
            return 0; 
        } 
	break;
    case RawBank::ITFull:
    case RawBank::ITError:
    case RawBank::ITPedestal:
    case RawBank::ITProcFull:
    case RawBank::IT:
        ipNet |= (13<<16);
	if (src < 16)
	    return (/*inet_addr("192.169.13.0")*/ ipNet | ((src + 1) << 24));
	// if (src < 32) return 1 + 1 + 88 + 18 + 48 + 14 + (src - 11);
	if (src < 64)
	    return (/*inet_addr("192.169.13.0")*/ ipNet | ((src - 17) << 24));
	if (src < 90)
	    return (/*inet_addr("192.169.13.0")*/ ipNet | ((src - 35) << 24));
	break;
    case RawBank::OTRaw:	// 32
    case RawBank::OTError:
    case RawBank::OT: 
/*
	char T = (src >> 8) & 0x01;   
	char L = (src >> 4) & 0x0F;
	char Q = (src >> 0) & 0x0F;
*/
        if(src > 24)
//	if ((Q & 0x1) == 1)	// A side
	{
          ipNet+=(12<<16); 
	} 
        else			// C side 
	{
          ipNet+=(16<<16); 
	}
//        return (ipNet+ ( ( (((Q >> 1)& 0x1)*12) + ((T-1)*4) + L + 1 )<< 24) ); 
        return (ipNet+ ((((src) %24)+1) << 24));

    case RawBank::MuonFull:
    case RawBank::MuonError:
    case RawBank::Muon:
	switch (src) {
	case 0:
	    return (/*inet_addr("192.169.18.6")*/ ipNet | (18<<16)|(6<<24));
	case 1:
	    return (/*inet_addr("192.169.18.7")*/ ipNet | (18<<16)|(7<<24));
	case 2:
	    return (/*inet_addr("192.169.9.7")*/ ipNet | (9<<16)|(7<<24));
	case 3:
	    return (/*inet_addr("192.169.9.6")*/ ipNet | (9<<16)|(6<<24));
	case 4:
	    return (/*inet_addr("192.169.18.4")*/ ipNet | (18<<16)|(4<<24));
	case 5:
	    return (/*inet_addr("192.169.18.5")*/ ipNet | (18<<16)|(5<<24));
	case 6:
	    return (/*inet_addr("192.169.9.5")*/ ipNet | (9<<16)|(5<<24));
	case 7:
	    return (/*inet_addr("192.169.9.4")*/ ipNet | (9<<16)|(4<<24));
	case 8:
	    return (/*inet_addr("192.169.18.3")*/ ipNet | (18<<16)|(3<<24));
	case 9:
	    return (/*inet_addr("192.169.9.3")*/ ipNet | (9<<16) |(3<<24));
	case 10:
	    return (/*inet_addr("192.169.18.2")*/ ipNet | (18<<16) |(2<<24));
	case 11:
	    return (/*inet_addr("192.169.9.2")*/ ipNet | (9<<16) |(2<<24));
	case 12:
	    return (/*inet_addr("192.169.18.1")*/ ipNet | (18<<16) |(1<<24));
	case 13:
	    return (/*inet_addr("192.169.9.1")*/ ipNet | (9<<16) |(1<<24));
	default:
            //log << MSG::WARNING << "Unknown source "<< src << " for Muon bank type"<<endmsg;   
            return 0;

	}
        break;
    case RawBank::L0PU:
        ipNet |= (15<<16);
        switch(src) {
        case 0 : return (ipNet | (1<<24));   /// Not sure about only this one, if an unknown src appear, it should be it ;) /* Dear Martin mentionned 2 but it seems it is 0*/
        default : 
            //log << MSG::WARNING << "Unknown source "<< src << " for L0PU bank type"<<endmsg;   
            return 0;
        }  
	return (/*inet_addr("192.169.15.0")*/ ipNet |(15<<16) + ((src + 1) << 24));	//TPU
    case RawBank::DAQ:
	return ipNet; //XXX
    case RawBank::ODIN:  ///XXX Not used in TFC mode, maybe wrong when used in AutoMode
//	return (/*inet_addr("192.169.5.0")*/ ipNet |(5<<16) + ((src+1) << 24));
        return m_BitOdinIPAddr; 
    case RawBank::EcalE:
    case RawBank::EcalTrig: 
    case RawBank::EcalPackedError: 
    case RawBank::EcalPacked:
	return (/*inet_addr("192.169.3.0")*/ ipNet |(3<<16) | ((src + 1) << 24));
    case RawBank::HcalE:
    case RawBank::HcalTrig:
    case RawBank::HcalPackedError:
    case RawBank::HcalPacked:
	return (/*inet_addr("192.169.2.0")*/ ipNet |(2<<16) | ((src + 1) << 24));
    case RawBank::PrsE:
    case RawBank::PrsTrig:
    case RawBank::PrsPackedError:
    case RawBank::PrsPacked:
	return (/*inet_addr("192.169.8.0")*/ ipNet |(8<<16) | ((src + 1) << 24));
    case RawBank::L0MuonCtrlAll:	// 39
    case RawBank::L0MuonProcCand:	// 40
    case RawBank::L0MuonProcData:	// 41
    case RawBank::L0MuonRaw:	// 42
    case RawBank::L0MuonError:	// 43
    case RawBank::L0Muon: // TMU
        ipNet+= (4<<16);
        switch(src) {
        case 3 :  return (ipNet | (4<<24));
        case 4 :  return (ipNet | (5<<24));
        case 5 :  return (ipNet | (6<<24));
        case 2 :  return (ipNet | (1<<24));
        case 1 :  return (ipNet | (2<<24));
        case 0 :  return (ipNet | (3<<24));
        default : 
            //log << MSG::WARNING << "Unknown source "<< src << " for L0Muon bank type"<<endmsg;   
            return 0;
        } 
             
	// case RawBank:: HLT:        return 1 + 1 + 88 + 18 + 42 + 48 + 24 + 36 + 2 + 1 + src;
    default:
        break;
    }
    return 0;
}
/*
/// Set data in the Odin MEP
/// For Automode only? Should set the EventType in normal mode too I think, yes but in the received Odin MEP 
void MEPInjector::setOdinData(MEPEvent ** me)
{
    int l0ID = m_L0ID;

    MEPMultiFragment *multFrag = (*me)->first();
    for (MEPFragment * iterFrag = multFrag->first();
	 iterFrag < multFrag->last(); iterFrag = multFrag->next(iterFrag))
    {
	for (RawBank * iterBank = iterFrag->first();
	     iterBank < iterFrag->last();
	     iterBank = iterFrag->next(iterBank)) {
	    for (OnlineRunInfo * iterOdinData = iterBank->begin < OnlineRunInfo > (); iterOdinData < iterBank->end < OnlineRunInfo > (); ++iterOdinData) {
		iterOdinData->L0ID = l0ID++;	///XXX
//        iterOdinData->EventType = smthing; 
	    }
	}
    }
}
*/

StatusCode MEPInjector::sendMEP(int tell1IP, MEPEvent * me)
{
    static MsgStream log(msgSvc(), name());

    if(me->size() == 0) return StatusCode::SUCCESS; //no data to send
    me->setSize(0);
    //Apply an offset of 4 to the MEPEvent structure to be sent, because the first 4 bytes are the size of the whole structure
    char *datagram = (char *) me + 4;

    struct MEPHdr *mh = NULL;
    mh = (struct MEPHdr *) &(((u_int8_t *) me->start())[20]);        // The MEP-header starts after the 20 byte IP-header
    int MEPSize = mh->m_totLen;


    u_int16_t datagramID =
	((u_int16_t) (0x000000FF & m_L0ID) << 8) |
	((u_int16_t) (0x0000FF00 & m_L0ID) >> 8);

//    log<<MSG::DEBUG<<"datagram id : "<< datagramID<<endmsg;

    int iDatagramSize = MEPSize +20 ;

    /// The size which interests us is the size of the IP datagram
    /// So lets remove the ethernet header from the max size 
    int iIPMTU = getMTU(m_HLTEthInterface) - 14;

    u_int32_t addrTo;
    if(m_AutoMode)
    {/*
        if (MEPRxSys::parse_addr(m_HLTStrIPAddrTo, addrTo) != 0) {
            std::string msg;
            if(MEPRxSys::addr_from_name(m_HLTStrIPAddrTo, addrTo, msg) != 0) {
   	        ERRMSG(log, " Automode HLT Address conversion ");
                return StatusCode::FAILURE;
            }
        }
      */
      addrTo = MEPRxSys::IPStringToBits(m_HLTStrIPAddrTo);
    }
    else
        addrTo = m_HLTIPAddrTo;
    /// If the frame size, i.e. the size of the datagram + the ethernet header, is less than the MTU, send it in one block, else fragment it  
    if (iDatagramSize < iIPMTU) {
	/// Do not fragment

	int n =
	    MEPRxSys::send_msg_arb_source(m_ToHLTSock, m_MEPProto, tell1IP,
					  addrTo, datagram, iDatagramSize,
					  datagramID);

	if (n == (int) iDatagramSize) {
	    return StatusCode::SUCCESS;
	}
	if (n == -1) {
	    ERRMSG(log, " MEP sending ");
	    return StatusCode::FAILURE;
	}
	ERRMSG(log, " MEP corrupted on send! Sent length:" + n);
        return StatusCode::FAILURE;
    }


    /// Fragmentation needed
    /// Fragments are send in the same order as the data 

    /// Pointer to the MEP data, skipping the ip header
    char *cFullMEP = (char *) me + 4 + 20;

    /// Buffer which will store the fragments to send
    char *cBuf = (char *) malloc(iIPMTU);
    if (cBuf == NULL) {
	ERRMSG(log, " No memory for buffer ");
	return StatusCode::FAILURE;
    }
    struct iphdr *hdr = (struct iphdr *) cBuf;
    hdr->frag_off = 0x0000;


    /// We can put MTU - IP header len - ETH header len data in each frame, and it has to be aligned on 8 bytes, because the 3 lower bytes from the fragment offset field are always 0
    int iAlign = (iIPMTU - 20) % 8;
    unsigned int uiMaxData = (iIPMTU - 20) - iAlign;

    int iNbIter = MEPSize / uiMaxData;

    /// Last datagram size without headers, it contains the bytes which could not get aligned in the full datagrams 
    int iLastDGramSize = MEPSize - uiMaxData * iNbIter;

    log << MSG::DEBUG << " Number of fragments : " << iNbIter << " of size "<< iIPMTU << endmsg;
    log << MSG::
	DEBUG << " Size of last fragment : " << iLastDGramSize <<
	endmsg;
    int iBytesSent = 0;

    for (int iCur = 1; iCur <= iNbIter; ++iCur)	/// 
    {
	/// Copy the data from the original datagram into the new one
	memcpy(cBuf + 20, cFullMEP, uiMaxData);

	/// Set the flag "More fragment" 
	hdr->frag_off |= 0x0020;

	/// Send the datagram (format the header) 
	int n =
	    MEPRxSys::send_msg_arb_source(m_ToHLTSock, m_MEPProto, tell1IP,
					  addrTo, cBuf, uiMaxData + 20,
					  datagramID);
	if (n == -1) {
	    ERRMSG(log, " MEP sending ");
	    return StatusCode::FAILURE;
	}

	iBytesSent += n;

	/// Prepare next datagram header
	/// Increment IP fragment offset 
	unsigned short tmpFragOff = (uiMaxData * iCur) >> 3;
	unsigned short cMask = 0x0000;
	cMask = tmpFragOff >> 8;
	hdr->frag_off = tmpFragOff << 8;
	hdr->frag_off |= cMask;

	hdr->frag_off &= 0xff1f;

	cFullMEP += uiMaxData;
    }

    /// Send the last datagram

    memcpy(cBuf + 20, cFullMEP, iLastDGramSize);

    /// Unset the flag "More fragment"
    hdr->frag_off &= 0xffDf;

    int n =
	MEPRxSys::send_msg_arb_source(m_ToHLTSock, m_MEPProto, tell1IP,
				      addrTo,
				      cBuf, iLastDGramSize + 20,
				      datagramID);

    iBytesSent += n;

    log << MSG::
	DEBUG << "Number of bytes sent : " << iBytesSent << "out of " <<
	MEPSize << endmsg;

    free(cBuf);

    if (n == -1) {
	ERRMSG(log, " MEP sending ");
	return StatusCode::FAILURE;
    }
    if (iBytesSent - ((iNbIter + 1) * 20) == MEPSize ) {
	log << MSG::
	    DEBUG << "MEP successfully fragmented and sent!" << endmsg;

        m_TotMEPsTx++; 
	return StatusCode::SUCCESS;
    }
    ERRMSG(log, " MEP corrupted on send! Sent length:" + n);
    return StatusCode::FAILURE;
}


StatusCode MEPInjector::sendMEPReq(MEPReq * req)
{
    static MsgStream log(msgSvc(), name()); 

    int n =
	MEPRxSys::send_msg(m_ToOdinSock, m_BitOdinIPAddr, MEP_REQ_TOS, req, MEP_REQ_LEN,
			   0);
    if (n == MEP_REQ_LEN) {
        log<<MSG::DEBUG<<"MEP Req successfully sent"<<endmsg;
	m_TotMEPReqTx += req->nmep;
	m_TotMEPReqPktTx++;
	return StatusCode::SUCCESS;
    }
    if (n == -1) {
	ERRMSG(log, " Send MEP request");
        return StatusCode::FAILURE;
    }
    ERRMSG(log, " MEPRequest corrupted on send!");
    return StatusCode::FAILURE;
}

StatusCode MEPInjector::receiveOdinMEP(char *bufMEP, int *retLen)
{
    static MsgStream log(msgSvc(), name());

    
    int n = MEPRxSys::rx_poll(m_FromOdinSock, m_TimeOut);	 
  
 
    if (n != 1) {
        /// Recoverable will ask for one more iteration of the manager
        /// which checks boolean which asks for end of injection
        return StatusCode::RECOVERABLE;
    }

    /// Reception with IP header, some cooking needed ?
    int len =
	MEPRxSys::recv_msg(m_FromOdinSock, bufMEP,
			   m_OdinBufSize,
			   0);
    if (len < 0) {
	if (!MEPRxSys::rx_would_block()) {
	    ERRMSG(log, " recvmsg");
            return StatusCode::FAILURE;
        }

	///XXX MANAGE THIS BETTER ! 
    }
    *retLen = len;
 
    m_TotOdinMEPRx++;

    RTL::IPHeader * iphdr = (RTL::IPHeader *) (bufMEP);

    ///Source address = 192.169.5.X + 32 (where OdinIPAdress for MEP Requests is 192.169.5.X)
    u_int32_t odinIPAddr = m_BitOdinIPAddr + ((32<<24)&0xff000000);  
 
    
    /// Check that the Odin MEP sender IP is the one we want 
    log << MSG::DEBUG <<"IP match : "<< iphdr->saddr << " =?= " << odinIPAddr << endmsg;
    if (iphdr->saddr != odinIPAddr) {
	ERRMSG(log, " Unexepected Odin source for received data");
        return StatusCode::RECOVERABLE;   ///XXX RECOVER THIS ERROR
    }
    log<<MSG::DEBUG<<"ODIN MEP RECEIVED"<<endmsg;
 
    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::receiveMEPReq(char *req)
{
    static MsgStream log(msgSvc(), name());
 
    if (req == NULL)
	return StatusCode::FAILURE;

    int n = MEPRxSys::rx_poll(m_FromHLTSock, m_TimeOut);	
    if (n != 1) {
        /// Recoverable will ask for one more iteration of the manager
        /// which checks boolean which asks for end of injection
        return StatusCode::RECOVERABLE;
    }
  
    int len =
	MEPRxSys::recv_msg(m_FromHLTSock, req, MEP_REQ_LEN + IPHDRSZ, 0);
    
    if (len < 0) {
	if (!MEPRxSys::rx_would_block()) {
	    ERRMSG(log, "recvmsg");
            return StatusCode::FAILURE;
        }
    }
    if (len != MEP_REQ_LEN + IPHDRSZ) {
	ERRMSG(log, " Packet received is not a MEP Request ");
        return StatusCode::FAILURE;
    }
    log<<MSG::DEBUG<<"MEP REQUEST RECEIVED"<<endmsg;
    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::manageMEPRequest()
{
    static MsgStream log(msgSvc(), name());
    StatusCode sc;
    char req[MEP_REQ_LEN + IPHDRSZ];
    memset(req, 0, sizeof(req));
 
    while (!m_ManagerStop) {
        if(m_InjState != RUNNING) { 
            MEPRxSys::microsleep(1000);
            continue;   //If injector is not running, idle  
        }
 
	sc = receiveMEPReq(req);
        if(sc.isRecoverable()) /// Should be the end of injection but continue to check 
        { 
            continue;
        } 
	if (sc.isFailure()) {
	    ERRMSG(log, " MEP Request Receive failed");
	    return sc;
	}

        log<<MSG::DEBUG<< "Got a MEP Request, forward it"<<endmsg;

        if (pthread_mutex_lock(&m_SyncReqOdin)) {
            ERRMSG(log, " Locking mutex");
            return StatusCode::FAILURE;
        }

        /// Increment the number of MEP request from the HLT in the map
	struct iphdr *hdr = (struct iphdr *) req;
	MEPReq *mreq = (MEPReq *) (req + 20);
	if (m_HLTReqs.find(hdr->saddr) == m_HLTReqs.end()) {
	    m_HLTReqs[hdr->saddr] = (unsigned int) (mreq->nmep &0x000000ff);
	} else {
	    m_HLTReqs[hdr->saddr] += (unsigned int) (mreq->nmep &0x000000ff);
	}

	m_TotMEPReqRx += mreq->nmep;
	m_TotMEPReqPktRx++;

        log<<MSG::DEBUG<< "FARM IP ADDRESS : "<< hdr->saddr <<" ; Credit asked " << mreq->nmep << " or " << m_HLTReqs[hdr->saddr] <<endmsg;

        if (pthread_mutex_unlock(&m_SyncReqOdin)) {
            ERRMSG(log, " Unlocking mutex");
            return StatusCode::FAILURE;
        }
        
        for(unsigned int i=0; i< (unsigned int) (mreq->nmep &0x000000ff); ++i) {
            
            int testsem = 0;
            sem_getvalue(&m_MEPReqCount, &testsem);
            log << MSG::DEBUG << "MEPReq SEM before post = "<<testsem << endmsg;
	    if(sem_post(&m_MEPReqCount)==-1)
            {
                ERRMSG(log, "Posting on the semaphore");
                perror("sem_post");
                exit(errno);  
            } 
        }

	/// Forward the MEP Request received, skip headers, default will be better for us 
        sc = sendMEPReq((MEPReq *) (req + 20));	
	if (sc.isFailure()) {
	    ERRMSG(log, " MEP Request Send");
	    return sc;
	}
    }
    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::manageOdinMEP()
{
    static MsgStream log(msgSvc(), name());
    StatusCode sc;

    char *bufMEP = (char *) malloc(m_OdinBufSize); /// I don't like calloc
    if(bufMEP == NULL) {
        ERRMSG(log, " Memory problem allocating Odin MEP buffer");
        return StatusCode::FAILURE;
    }
    memset(bufMEP, 0, m_OdinBufSize);
  
    int len;

    while (!m_ManagerStop) {
        if(m_InjState != RUNNING) {
            MEPRxSys::microsleep(1000);
            continue;   //If injector is not running, idle  
        }

	sc = receiveOdinMEP(bufMEP, &len);
        if(sc.isRecoverable())  /// Should be the end of injection, continue is to check it 
        {
            continue;
        } 
	if (sc.isFailure()) {
	    ERRMSG(log, "Failed to receive Odin Information : MEP reception");
            return StatusCode::FAILURE;
	}

	if (pthread_mutex_lock(&m_SyncMainOdin)) {
	    ERRMSG(log, "Failed locking mutex");
	    return StatusCode::FAILURE;
	}

        /// Need something (sem) to tell that the copy has been done? or a good algo
         
        /// Add the Odin MEP to the queue
        char* tmp = (char *)malloc(len+4);
        memcpy(tmp+4, bufMEP, len);
        int *itmp = (int *) tmp;
        itmp[0] = len;
        m_QueueOdinMEP.push(tmp); 
 
        log << MSG::DEBUG << "SIZE OF ODIN MEP BUFFER : " << m_QueueOdinMEP.size() << endmsg; 

	if (pthread_mutex_unlock(&m_SyncMainOdin)) {
	    ERRMSG(log, "Failed unlocking mutex");
	    return StatusCode::FAILURE;
	}

        int testsem = 0;
        sem_getvalue(&m_OdinCount, &testsem);
        log << MSG::DEBUG << "Odin SEM before post = "<<testsem << endmsg;

        if(sem_post(&m_OdinCount)==-1)
        {
            ERRMSG(log, "Posting on the semaphore");
            perror("sem_post"); 
            exit(errno);
        }
    }
    free(bufMEP);
    return StatusCode::SUCCESS;
}


StatusCode MEPInjector::getInfoFromOdinMEP() 
{

    /// XXX get all data from all banks, according to the pf, it means copy the complete mep (memcpy)   
    /// Set packing factor, L0ID, and all "global" variable according to the Odin MEP received
    struct MEPHdr *mh =
        (struct MEPHdr *) &(((u_int8_t *) (m_OdinMEP)->start())[20]);

    m_L0ID = mh->m_l0ID;
    m_PackingFactor = mh->m_nEvt;
    m_PartitionID = mh->m_partitionID;


    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::getHLTInfo() 
{
    static int cpt = 0;

    static MsgStream log(msgSvc(), name());    //Message stream for output
    StatusCode ret=StatusCode::RECOVERABLE;
    int retVal=0;
   
    log << MSG::DEBUG << "GET HLT INFO BEFORE SEM" <<endmsg;

    int testsem = 0;
    sem_getvalue(&m_MEPReqCount, &testsem);
    log << MSG::DEBUG << "HLT SEM = "<<testsem << endmsg;
 
//    while((retVal=sem_wait(&m_MEPReqCount))==-1 && errno == EINTR) continue;
    retVal=sem_wait(&m_MEPReqCount);
    if(retVal == -1) {
        perror("sem_wait");
        exit(errno);
    }
    log << MSG::DEBUG << "GET HLT INFO AFTER SEM" <<endmsg;
 
    if (pthread_mutex_lock(&m_SyncReqOdin)) {
        ERRMSG(log, "Failed locking mutex");
        return StatusCode::FAILURE;
    }

    /// Select an HLT farm where to send the MEPs, max should be fine
    /// XXX Needs short synchro with the MEPReq manager
    if (m_HLTReqs.size() > 0) {
	std::map < unsigned int, unsigned int >::iterator ite =
	std::max_element(m_HLTReqs.begin(), m_HLTReqs.end(),
			 value_comparer);
        if(ite == m_HLTReqs.end()) return StatusCode::FAILURE; 
        m_HLTIPAddrTo = ite->first;

        log << MSG::DEBUG << "FARM SELECTED TO RECEIVE DATA : " << m_HLTIPAddrTo << endmsg;  
        log << MSG::DEBUG << "NUMBER OF HLT MANAGED = " << ++cpt << endmsg;

        --(ite->second);
        ret = StatusCode::SUCCESS;
    } 

    if (pthread_mutex_unlock(&m_SyncReqOdin)) {
        ERRMSG(log, "Failed unlocking mutex");
        return StatusCode::FAILURE;
    }

    return ret;
}

StatusCode MEPInjector::getOdinInfo()
{
    static int cpt = 0;

    StatusCode ret=StatusCode::RECOVERABLE;
    static MsgStream log(msgSvc(), name());    //Message stream for output
    int retVal=0;

    log << MSG::DEBUG << "GET ODIN INFO BEFORE SEM" << endmsg;

    int testsem = 0;
    sem_getvalue(&m_OdinCount, &testsem);
    log << MSG::DEBUG << "Odin SEM = "<<testsem << endmsg;

//    while ( (retVal=sem_wait(&m_OdinCount)==-1) && errno == EINTR) continue;
    retVal=sem_wait(&m_OdinCount);
    if(retVal ==-1) {
        perror("sem_wait");
        exit(errno);
    }

    log << MSG::DEBUG << "GET ODIN INFO AFTER SEM" << endmsg;

    if (pthread_mutex_lock(&m_SyncMainOdin)) {
        ERRMSG(log, "Failed locking mutex");
        return StatusCode::FAILURE;
    }
   
    
 
    /// Get the first Odin MEP from the queue
    if(m_QueueOdinMEP.empty()==false)
    {
        char *tmp = m_QueueOdinMEP.front(); 
        int *itmp = (int *) tmp; 
        memcpy(m_OdinData.data(), tmp, itmp[0] ); 
        m_OdinMEP->setSize(itmp[0]);

        m_QueueOdinMEP.pop();
        free(tmp);

        MEPHdr *mephdr = (MEPHdr *) ( ((char *) m_OdinMEP) +  MEPEVENTOFFSET + IPHDRSZ);
        m_EvtID = mephdr->m_l0ID;
        m_L0ID= mephdr->m_l0ID;
        m_PartitionID = mephdr->m_partitionID;
        m_PackingFactor = mephdr->m_nEvt;

        log << MSG::DEBUG << "Odin information : " <<endmsg;
        log << MSG::DEBUG << "EvtID = "<< m_EvtID << endmsg;
        log << MSG::DEBUG << "PartitionID = "<< m_PartitionID << endmsg;
        log << MSG::DEBUG << "PackingFactor = "<< m_PackingFactor << endmsg;
        log << MSG::DEBUG << "L0ID = " << m_L0ID << endmsg;

        log << MSG::DEBUG << "NUMBER OF ODIN MEP MANAGED = " << ++cpt << endmsg;

        ++m_TotOdinMEPTx;  

        ret = StatusCode::SUCCESS; 
    }

    if (pthread_mutex_unlock(&m_SyncMainOdin)) {
        ERRMSG(log, "Failed unlocking mutex");
        return StatusCode::FAILURE;
    }

    return ret;
}


StatusCode MEPInjector::error(const std::string & msg)
{
    static MsgStream log(msgSvc(), name());
    log << MSG::ERROR << msg << endmsg;
    return StatusCode::FAILURE;
}

void saveHisto()
{
    for (int bkType = RawBank::L0Calo; bkType < RawBank::LastType;
             ++bkType)
    {
        std::string str = "HISTOS/"+RawBank::typeName((RawBank::BankType)bkType);
        FILE *output = fopen(str.c_str(), "w");  
        
        fprintf(output, "NB EVENT = %d\n", BKSTATI);
        fprintf(output, "%s;SourceID;Count\n", RawBank::typeName((RawBank::BankType)bkType).c_str());   
        for(std::map<int, int>::iterator ite = BKSTATMAP[bkType].begin(); ite != BKSTATMAP[bkType].end(); ite ++) {
            fprintf(output, "%sSTAT;%d;%d\n", RawBank::typeName((RawBank::BankType)bkType).c_str(),ite->first,ite->second);   

        }
        fclose(output); 
    }
}


StatusCode MEPInjector::finalize()
{
    static MsgStream log(msgSvc(), name());
    StatusCode sc;
    MEPEvent *me = NULL;

    m_InjState = NOT_READY;

    std::cout << "ici aussi" << std::endl;

    /// XXX Set this only on dim stop command ???
    m_InjectorStop = true;
    m_ManagerStop = true;


    /// No more need of synchronisation to access to the queues/maps, min value = 1
    /// and they should exit  
    if(sem_post(&m_RawEventsCount)==-1)
    {   
        ERRMSG(log, "Posting on the semaphore");
        perror("sem_post");
        exit(errno);
    }
    if(sem_post(&m_MEPReqCount)==-1)
    {
        ERRMSG(log, "Posting on the semaphore");
        perror("sem_post");
        exit(errno);
    } 
    if(sem_post(&m_OdinCount)==-1) 
    {
        ERRMSG(log, "Posting on the semaphore");
        perror("sem_post");
        exit(errno);
    }
    
/*
    if(pthread_kill(m_InjectorProcessing, SIGTERM))
    {
        ERRMSG(log, "Kill SIGTERM InjectorProcessing");
        return StatusCode::FAILURE;
    }
*/
    if(pthread_join(m_InjectorProcessing, NULL))
    {
       ERRMSG(log, "injectorProcessing thread join");
       return StatusCode::FAILURE;
    }

    /// Do not do if here cause of dim stop cmd, only in automode
    /// XXX AND HOW CAN I CHECK IF IM HERE IN TFC MODE WITHOUT STOP CMD BUT  IN END ??
    /// This is done to ensure that nothing remains in the queue, the reader thread takes the job of the injectorprocessing thread ... but it's easier
    if(m_AutoMode) {
        while(m_QueueRawEvents.size()>0) {
            readThenSend(); ///XXX NOT REALLY OPTIMIZED BUT THIS IS A POC YET
        }
    }


    /// Once the file is completely read, it is possible that we have to send a MEP which has not reached the Packing Factor
    /// Just do as usual, the encodeMEP method deal with that 
    /// and will set the correct number of event 
    /// Done only if the file was completely read, if finalize cause of dim command, 
    /// do nothing, just close (if i find how to use this dim stuff)
    if(finalSent) 
    {      
        /// Use the first stored MEP to find the last packing factor  
        std::map<unsigned long, MEPEvent *>::iterator iteTell1ID = m_MapTell1MEPs.begin(); ///Hope that the first ip is not an OT ... 
        me = (iteTell1ID->second);
        if(me != NULL)
        {      
            MEPHdr *mephdr = (MEPHdr *) ( ((char *) me) +  MEPEVENTOFFSET + IPHDRSZ);
            m_PackingFactor = mephdr->m_nEvt;
            OTFIX(12);
            OTFIX(16);
        
            sc = sendMEP(iteTell1ID->first, me);
            if (sc.isFailure())
                return sc;
        } ///end if
        for(++iteTell1ID; iteTell1ID != m_MapTell1MEPs.end(); ++iteTell1ID)
        {
            me = (iteTell1ID->second);
            if(me != NULL)
            {
   	        sc = sendMEP(iteTell1ID->first, me);
	        if (sc.isFailure())
	            return sc;
            } ///end if
        } ///end for
        if (!m_AutoMode) {
            ///XXX HEre, modify the packing factor of the Odin MEP in order for the HLT to accept the incomplete MEPS
            /// else you get that ERROR Bad packet from ottella15-d1 [mismatch in packing factor]
            /// First send normal MEPS, get their pf and send the odin mep with the pf

            u_int32_t addrFrom = m_BitOdinIPAddr +  ((32<<24)&0xff000000);
            log<<MSG::DEBUG<<__FUNCTION__<<"MEP SIZE : " << m_OdinMEP->size() << endmsg;

            MEPHdr *mephdr = (MEPHdr *) ( ((char *) m_OdinMEP) +  MEPEVENTOFFSET + IPHDRSZ);
            
            m_OdinMEP->setSize(m_OdinMEP->size() - ODFRAGSZ * (mephdr->m_nEvt - m_PackingFactor)); 
            mephdr->m_totLen-=(ODFRAGSZ * (mephdr->m_nEvt - m_PackingFactor)) ;
            mephdr->m_nEvt = m_PackingFactor;

            sc = sendMEP(addrFrom, m_OdinMEP);
            if(sc.isFailure()) {
                ERRMSG(log, "Could not send Odin MEP");
                return StatusCode::FAILURE;
            }

            log<< MSG::DEBUG << "Odin MEP sent";
        }
 
        m_TotEvtsSent += m_PackingFactor; 
    } /// end if finalSent
    if(! m_AutoMode) {
/*
        if(pthread_kill(m_ThreadOdinMEPManager, SIGTERM))
        {
            ERRMSG(log, "Kill SIGTERM OdinMEPManager");
            return StatusCode::FAILURE;
        } 
        if(pthread_kill(m_ThreadMEPReqManager, SIGTERM))
        {
            ERRMSG(log, "Kill SIGTERM MEPReqManager");
            return StatusCode::FAILURE;
        }
*/
        if(pthread_join(m_ThreadOdinMEPManager, NULL))
        {
            ERRMSG(log, "OdinMEPManager thread join");
            return StatusCode::FAILURE;
        }
        if(pthread_join(m_ThreadMEPReqManager, NULL)) 
        {
            ERRMSG(log, "MEPReqManager thread join");
            return StatusCode::FAILURE;
        }


        ///XXX Should be tested
        close(m_FromOdinSock);
        close(m_ToOdinSock); 
        close(m_FromHLTSock);

        sem_destroy(&m_OdinCount);
        sem_destroy(&m_MEPReqCount);

    }

    sem_destroy(&m_RawEventsCount);
    close(m_ToHLTSock);

    log << MSG::DEBUG <<"Injector finalized"<<endmsg;
 
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

    return StatusCode::SUCCESS;
}

StatusCode MEPInjector::run() {

    return StatusCode::SUCCESS;
}


template <typename T> static void resetCounter(T& cnt) { cnt = 0; }
template <typename T> static void resetCounters(T& cnt,size_t len) {
  cnt.resize(len,0);
  std::for_each(cnt.begin(),cnt.end(),resetCounter<typename T::value_type>);
} 


void MEPInjector::publishCounters()
{
//  PUBCNT(totRxOct,           "Total received bytes");
//  PUBARRAYCNT(badLenPkt,     "MEPs with mismatched length");
}

void MEPInjector::clearCounters() {
//  resetCounters(m_rxOct, m_nSrc);
//  m_totMEPReq          = 0;

    m_TotMEPReqRx = 0;
    m_TotMEPReqPktRx = 0;
    m_TotOdinMEPRx = 0;
    m_TotMEPReqTx = 0;
    m_TotMEPReqPktTx = 0;
    m_TotOdinMEPTx = 0;
    m_TotEvtsRead = 0;
    m_TotEvtsSent = 0;
    m_TotElapsedTime = 0;
    m_TotMEPsTx = 0;
    gettimeofday(&m_InitTime, NULL);
}

int MEPInjector::setupCounters() {
  MsgStream log(msgSvc(),"MEPRx");
  clearCounters();
  publishCounters();
  
  // create cstring for source names
  std::string all_names = "";
  for (unsigned i = 0; i < m_Tell1Boards.size(); ++i)  all_names = all_names + '\0' + m_Tell1Boards[i];
  if (!(m_AllNames = new char[all_names.size()+1]))
    return 1;
   ::memcpy(m_AllNames, (const char *) all_names.data(), all_names.size() +1);
  
  m_MonSvc->declareInfo("srcName", "C:", m_AllNames, sizeof(m_Tell1Boards), "Source IP names", this);
  log << MSG::INFO << all_names << ", " << m_Tell1Boards.size() << endmsg;
  return 0;
}



void MEPInjector::handle(const Incident& inc) {
  MsgStream log(msgSvc(),name());
  std::cout << "handle called" << std::endl;
  log << MSG::INFO << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")  { //but if I remember well the PVSS interface was leading the injector in not ready no ? XXX check this once Alba is back 
      m_InjState = STOPPED;
  }
  else if (inc.type() == "DAQ_ENABLE") {
      m_InjState = RUNNING;
  }
}


// These startup functions are helper functions. They have normal C linkage.
void *MEPReqMgrStartup(void *object)
{
    MEPInjector *injector = (MEPInjector *) object;

    printf("THREAD : Running thread object in a new thread : MEP Request Manager\n");

    StatusCode sc = injector->manageMEPRequest();
    //printf("Deleting object\n");
    //delete tgtObject;
    if (sc.isFailure())
	return (void *) -1;
    else
	return (void *) 0;
}

void *OdinMEPMgrStartup(void *object)
{
    MEPInjector *injector = (MEPInjector *) object;

    printf("THREAD : Running thread object in a new thread : Odin MEP Manager\n");

    StatusCode sc = injector->manageOdinMEP();
    //printf("Deleting object\n");
    //delete tgtObject;
    if (sc.isFailure())
	return (void *) -1;
    else
	return (void *) 0;
}

void *injectorProcessingStartup(void *object)
{
    MEPInjector *injector = (MEPInjector *) object;

    printf("THREAD : Running thread object in a new thread : Injector Process\n");

    StatusCode sc = injector->injectorProcessing();
    //printf("Deleting object\n");
    //delete tgtObject;
    if (sc.isFailure())
        return (void *) -1;
    else
        return (void *) 0;

}


#endif //ifndef _WIN32
