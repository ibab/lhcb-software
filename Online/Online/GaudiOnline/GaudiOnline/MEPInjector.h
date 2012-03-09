//  ====================================================================
//  MEPInjector.h
//  --------------------------------------------------------------------
//
//  Authors    : David Svantesson
//              Jean-Christophe Garnier
//				S. Vijay Kartik
//
//  ====================================================================

#ifndef _WIN32

#ifndef MEP_MEPINJECTOR_H
#define MEP_MEPINJECTOR_H

#include "Event/RawEvent.h"
#include "MDF/MDFWriter.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPWriter.h"
#include "MDF/MEPEvent.h"
#include <MDF/MEPFragment.h>
#include "Event/RawEvent.h"
#include "GaudiOnline/MEPHdr.h"

#include "MBM/bmdef.h"
#include "MBM/Consumer.h"

#include "DimTaskFSM.h"

#include <map>
#include <queue>

#include <netinet/ip.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>

#include <semaphore.h>

#include <time.h>



/*
 *    LHCb namespace
 */
namespace LHCb {

#define WHERE "File: " << __FILE__ << "[" << __LINE__ << "] " << __FUNCTION__ << "() :"

#define TWOMB 2097152
#define MEP_REQ_TTL     10
/*
#define LUMI "LUMI"
#define PHYS "PHYS"
#define PERA "PERA"
#define PERB "PERB"
#define AUXI "AUXI"
#define RAND "RAND"
#define TIMI "TIMI"
#define CALX "CALX"
#define UNKN "UNKN"
*/

#define TRG0 "TRG0"
#define TRG1 "TRG1"
#define TRG2 "TRG2"
#define TRG3 "TRG3"
#define TRG4 "TRG4"
#define TRG5 "TRG5"
#define TRG6 "TRG6"
#define TRG7 "TRG7"

 /** @class MEPInjector MEPInjector.cpp
  *  Creates and injects MEPs from MDF files.
  *
  *  @author David Svantesson
  *          Jean-Christophe Garnier
  *  @date   2008-06-26
  */
  class MEPInjector : public Service, virtual public IRunable
                      ,virtual public IIncidentListener
 {
  public:
    typedef std::vector<std::string>        Requirements;

  protected:
    MEPReq m_InjReq;

    bool m_NeedOTConv;

    bool m_LocalTest;   /*To tell to run locally for testing. */
    int m_DestTestPort;
    int m_SrcTestPort;
    int m_TestLimit;

    int m_InitialCredits;

    // Protected Attributes
    Requirements m_Req; /* Vector of strings which tells the options to access to buffer managers,             */
                        /* Most are useless and the vector should be like this :                               */
                        /* "EvType=2;TriggerMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;VetoMask=0,0,0,0; */
                        /* MaskType=ANY;UserType=VIP;Frequency=PERC;Perc=100.0"                                */
                        /* But I wanted it to be parameterisable                                               */

    std::string m_ProcName; /* The RTL process name */

    unsigned int m_TFCMask, m_TapeMask; /* Masks to get Odin information from tape and from online */

    std::vector<std::string> m_BufferNames;    /* Buffer Manager names   */
    std::vector<std::string> m_BufferOptions;  /* Buffer Manager options */
    std::map<std::string, BMID> m_EventBuffers;/* Buffer Manager map     */

    char m_CurEvent[TWOMB]; /* Buffer to store one event */
    int  m_CurEventSize;    /* Size of the event         */

    std::vector<unsigned int> m_TAELens; /* TAE length of each event */

    int m_CreditsFD;       /* DEBUG : File descriptor where to write HLT credits information */

    bool m_gotHLT;          /* Tells that a MEP Request was consumed for injection processing */
    bool m_gotOdin;         /* Tells that an Odin MEP was consumed for injection processing   */

    bool m_ARPMe;           /* Deprecated : The injector will send a few ping answers to the HLTs */
                            /* in order to get their mac address before the main processing starts*/

    bool m_Offline;         /* Offline use of the Odin, i.e. get most of Odin information from tape*/

    bool m_Debug;           /* Debug mode, more verbose and more debugging operations */

    enum InjectorState { NOT_READY, READY, RUNNING, STOPPED }; /* FSM states of the injector */
    InjectorState m_InjState;                                  /* Current state              */

    IIncidentSvc* m_IncidentSvc; /* Service to catch DIM commands */
    IMonitorSvc* m_MonSvc;      /* Service to publish information*/

    char* m_AllNames;        /* Publish the names of all enabled Tell1s */

    sem_t m_OdinCount;       /* Synchronisation to access to Odin MEP data */
                                     /* = counter of MEPs in the queue */
    sem_t m_MEPReqCount;     /* Synchronisation to access to HLT Req data */
                                     /* = counter of requests registered */

    // Data to publish or used to publish monitoring
    struct timeval m_InitTime;    /* Date of initialization of the injector */
    struct timeval m_CurTime;     /* Current date */
    struct timespec m_RTInitTime; /* Date of initialization of the injector XXX*/
    struct timespec m_RTCurTime;  /* Current date XXX*/

    int m_TotBytesTx;          /* Total of bytes transmitted to the farms */
    int m_TotEvtsRead;         /* Total of events read */
    int m_TotEvtsSent;         /* Total of events sent */
    int m_TotMEPsTx;           /* Total of MEPs transmitted */
    int m_RTTotElapsedTime;     /* Elapsed time since initialization, in ms, using RDTSC Time Stamp Couner XXX Not used yet */
    int m_TotElapsedTime;       /* Elapsed time since the initialization, in ms */
    int m_TotCreditConsumed;    /* Total of credits consumed by the injector */
    int m_TotMEPReqTx;          /* Total of MEP request forwarded */
    int m_TotMEPReqRx;          /* Total of MEP request received */
    int m_TotMEPReqPktTx;       /* Total of MEP request packet sent */
    int m_TotMEPReqPktRx;       /* Total of MEP request packet received */
    int m_TotOdinMEPRx;         /* Total of Odin MEP received */
    int m_TotOdinMEPTx;         /* Total of Odin MEP sent */
    int m_CurOdinMEPs;          /* Current number of Odin MEP in reception buffer */
    int m_CurMEPReqCredits;     /* Current number of credits asked by HLTs to be processed */

    pthread_t m_ThreadMEPReqManager; /* pthread descriptor for the MEP request management */
    pthread_t m_ThreadOdinMEPManager; /* pthread descriptor for the Odin MEPs manager */
    pthread_t m_InjectorProcessing; /* The main thread only reads events from a buffer manager, raw use with plain C MBM api */

    int m_MEPProto;     /* IP protocol used for MEPs*/
    int m_MEPReqProto;  /* IP protocol used for MEP Requests*/

    int m_ToHLTSock;    /* Raw socket to send MEPs. */
    int m_MEPBufSize;   /* Size of MEP socket buffer*/

    int m_FromHLTSock;  /* Raw socket to receive MEP Requests */
    int m_MEPReqBufSize;/* Size of MEP Request buffer */

    int m_OdinBufSize;  /* Size of Odin MEP socket buffer */
    int m_FromOdinSock; /* Socket to receive Odin MEP     */
    int m_ToOdinSock;   /* Socket to forward MEP Requests to Odin */

    int m_TimeOut;    /* Time Out for every blocking call */
                                /* Only used for the polling */

    int m_MEPSize;    /* Size of the MEP on which the algorithm is working on */

    bool m_AutoMode;   /* Set by job options, to determine     */
 			        /* how Odin information will be managed */

    std::string m_HLTIfIPAddr;    /* The destination address of HLT MEP Request */
    std::string m_OdinIfIPAddr;   /* The source address to send MEP request to Odin */

    std::vector<std::string> m_HLTNodes;     /* The HLT nodes, to be pinged at initialization */
    std::vector<std::string> m_Tell1Boards;  /* The Tell1 enabled for the injector, format = {"IP", "HOSTNAME", "TYPE" } */

    int m_OdinEthInterface; /* The Ethernet interface for Odin */
    int m_HLTEthInterface;  /* The Ethernet interface for HLT  */

    std::string m_StrOdinIPAddr; /* IP Address of Odin */
    u_int32_t m_BitOdinIPAddr; /* IP Address of Odin */


    std::string m_HLTStrIPAddrTo;/* IP Address to send to, in string */
    u_int32_t m_HLTIPAddrTo;   /* IP Address to send to */

    unsigned short m_EvtID;     /* Id of the event currently processed */

    unsigned short m_DatagramID; /* Datagram Id to manage ourselves this integer, and do not respect the MEP protocol */
                                 /* Because Odin starts to 0 and raw ip does not like it */

    int m_OdinTell1ID;          /* Id of the Odin board */

    std::map<unsigned long, StreamBuffer> m_MapStreamBuffers; /* Streambuffers which manage the memory allocated for the tell1 MEPs */
    std::map<unsigned long, MEPEvent *> m_MapTell1MEPs;       /* The effective Tell1 MEPs, index is Tell1 IP */

    MEPEvent* m_OdinMEP;    /* The Odin MEP currently processed */
    std::queue<char*> m_QueueOdinMEP; /* A queue of Odin MEP to be processed */

    std::map<unsigned int, unsigned int > m_HLTReqs; /* map<HLT IP, counter of request> */
    std::map < unsigned int, unsigned int >::iterator m_HLTReqsIte; /* iterator on the MEP Requests */

    u_int32_t m_PartitionID;   /* Partition ID of the events, read from option file and/or from Odin */
    ///////////////////////////////////////////////////////////////////////
    // These attributes are set by Odin MEPs or by Job options in automode
    unsigned int  m_PackingFactor; /* MEP Event packing factor */

    u_int32_t m_L0ID;          /* Level 0 ID               */
    ///////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////
    // Protected Methods

    // Delete the buffer managers
    StatusCode deleteConsumers();
    // Initialize the use of the buffer managers
    StatusCode initConsumers();
    // Make the buffer managers
    StatusCode initBuffers();
    // Consume one event from the buffer managers, according to the trigger type of the online TFC
    StatusCode getEvent(int nbEv);
    // Log MEP requests received, forwaded and credits consumed
    StatusCode logMEPReqs();

    // Copy the content of the src bank in dest bank. except for run number and GPS time
    void copyOdinBank(OnlineRunInfo **dest, OnlineRunInfo **src);
    // In order to send ARP request at initialization
    StatusCode pingHLTNodes();
    // Get the MTU configured by the system for the opened socket
    int getMTU(int );
    // Cook the MEP which will be sent, setting MEP header and fragment headers
    StatusCode prepareMEP(std::map<unsigned int, RawEvent*> &events, MEPEvent **me);
    // Deal with the IP layer to send the MEP
    StatusCode sendMEP( int tell1IP, MEPEvent *me);
    // Receive MEP(s) from Odin
    StatusCode receiveOdinMEP(char *bufMEP, int *len);
    // Read an event from the MDF transient store and split fragments correctly
    StatusCode processEvent(char *curEvt, unsigned int len);
    // Identify the Tell1 ID from the type and the source of the bank
    in_addr_t getTell1IP(int type, int src);
    // Copy the OdinMEP to the main thread memory space
    StatusCode getOdinInfo();
    // Get the HLT which will receives the MEPs
    StatusCode getHLTInfo();
    // Print the error message and return SatusCode::FAILURE
    StatusCode error(const std::string& msg);
    // Print the MEP on the m_OutStream
    void printMEP(MEPEvent * me, int size);
    // Send a MEP Request to the Readout Supervisor
    StatusCode sendMEPReq(MEPReq *req);

    // Somehow the main processing
    StatusCode readThenSend();
    // Receive a MEP Request from the HLT
    StatusCode receiveMEPReq(char *req);
    // Clear monitoring counters
    void clearCounters();
    // Setup monitoring counters
    int setupCounters();
    // Publish monitoring counters
    void publishCounters(void);

    /// Fix OT non respect of the rules
    StatusCode OTFIX(int id, int nbEv);

   public:
    ///////////////////////////////////////////////////////////////////////
    // Public Methods

    // Standard constructor
    MEPInjector( const std::string& name, ISvcLocator* pSvcLocator );
    // Destructor
    virtual ~MEPInjector()  {}
    // Initialize
    virtual StatusCode initialize();
    // Finalize
    virtual StatusCode finalize();
    // Run
    virtual StatusCode run();
    // Handle
    virtual void handle(const Incident& incident);
    // Main processing of the injector
    StatusCode injectorProcessing();
    // Get information from Odin
    StatusCode manageOdinMEP();
    // Record MEP requests and forward them to TFC
    StatusCode manageMEPRequest();
    // IInterface implementation in order for the service to be used as a Runable by the Application Manager
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppIf);

    ///////////////////////////////////////////////////////////////////////
    // Public Attributes

    bool m_ManagerStop; /* Boolean to tell threads to stop their loops */

    pthread_mutex_t m_SyncMainOdin;/* Mutex used to synchronise the end of the data send and the right of copy of Odin information */
    pthread_mutex_t m_SyncReqOdin; /* Mutex used to synchronise the use of the map of HLT request counters */

    // Streambuffer to hold uncompressed data. For encoding MEPs, contains everything of the MEP (4 bytes of size + MEP buffer)
    StreamBuffer m_OdinData;

  };


} // namespace LHCb

#endif // MEP_MEPINJECTOR_H

#ifndef MEP_THREADINTERFACE
#define MEP_THREADINTERFACE
extern "C" void *MEPReqMgrStartup(void *);
extern "C" void *OdinMEPMgrStartup(void *);
extern "C" void *injectorProcessingStartup(void *);
#endif // MEP_THREADINTERFACE

#ifndef MEP_MAPMGT
#define MEP_MAPMGT
// Deprecated : A comparer used to find the HLT farm which has sent the most of MEP Requests
bool value_comparer(std::map<unsigned int, unsigned int>::value_type &i1, std::map<unsigned int, unsigned int>::value_type &i2)
{
  return i1.second<i2.second;
}
#endif // MEP_MAPMGT


#endif // ifndef _WIN32
