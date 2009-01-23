
//  ====================================================================
//  MEPInjector.h
//  --------------------------------------------------------------------
//
//  Author    : David Svantesson
//              Jean-Christophe Garnier
//
//  ====================================================================

#ifndef _WIN32

#ifndef MEP_MEPINJECTOR_H
#define MEP_MEPINJECTOR_H

#include "Event/RawEvent.h"
#include "MDF/MDFWriter.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"

#include "DimTaskFSM.h"

#include <map>
#include <queue>

#include <netinet/ip.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 
#endif
#include <pthread.h>

#include <semaphore.h>

/// DIM Server header
#include <dis.hxx>


/*
 *    LHCb namespace
 */
namespace LHCb {

#define MEP_REQ_TTL     10


 
 /** @class MEPInjector MEPInjector.cpp
  *  Creates and injects MEPs from MDF files. 
  *
  *  @author David Svantesson
  *          Jean-Christophe Garnier
  *  @date   2008-06-26
  */
  class MEPInjector : public Algorithm, virtual public IRunable 
                      ,virtual public IIncidentListener
 {
  protected:
 
    bool m_gotHLT;

    bool m_gotOdin;  
   
    bool m_ARPMe;
  
    enum InjectorState { NOT_READY, READY, RUNNING, STOPPED };
 
    InjectorState m_InjState;

    /// Protected Attributes
    IIncidentSvc*               m_IncidentSvc; 
    IMonitorSvc*                m_MonSvc;

    char*         m_AllNames;        /* Publish the names of all enabled Tell1s */

    int  m_EvtBuf;
 
    sem_t         m_OdinCount;       /* Synchronisation to access to Odin MEP data */
                                     /* = counter of MEPs in the queue */
 
    sem_t         m_RawEventsCount;
  
    sem_t         m_MEPReqCount;     /* Synchronisation to access to HLT Req data */
                                     /* = counter of requests registered */

    std::queue< /*SmartDataPtr<RawEvent>*/ RawEvent* > m_QueueRawEvents;   /* A queue of raw events to be processed */
 
    /// Data to publish or used to publish monitoring 
    int  m_TotBytesTx;          /* Total of bytes transmitted to the farms */ 
    int  m_TotEvtsRead;         /* Total of events read */
    int  m_TotEvtsSent;         /* Total of events sent */
    int  m_TotMEPsTx;           /* Total of MEPs transmitted */
    struct timeval m_InitTime;          /* Date of initialization of the injector */
    struct timeval m_CurTime;           /* Current date */
    int m_TotElapsedTime;      /* Elapsed time since the initialization, in ms */       
    int m_TotMEPReqTx;          /* Total of MEP request forwarded */
    int m_TotMEPReqRx;          /* Total of MEP request received */
    int m_TotMEPReqPktTx;       /* Total of MEP request packet sent */
    int m_TotMEPReqPktRx;       /* Total of MEP request packet received */
    int m_TotOdinMEPRx;         /* Total of Odin MEP received */
    int m_TotOdinMEPTx;         /* Total of Odin MEP sent */

    pthread_t     m_ThreadMEPReqManager; 
 
    pthread_t     m_ThreadOdinMEPManager;

    pthread_t     m_InjectorProcessing; /// The main thread only reads events from storage

    /// Option to remove DAQStatus (should be true)
    bool          m_RemoveDAQStatus; /* Option to remove DAQStatus */
                                     /* Deprecated ? */
    
    int           m_MEPProto;     /* IP protocol used for MEPs*/
    int           m_MEPReqProto;  /* IP protocol used for MEP Requests*/
    
    int           m_ToHLTSock;    /* Raw socket to send MEPs. */
    int           m_MEPBufSize;   /* Size of MEP socket buffer*/  

    int           m_FromHLTSock;  /* Raw socket to receive MEP Requests */
    int           m_MEPReqBufSize;/* Size of MEP Request buffer */ 

    int 	  m_OdinBufSize;  /* Size of Odin MEP socket buffer */
    int 	  m_FromOdinSock; /* Socket to receive Odin MEP     */
    int 	  m_ToOdinSock;   /* Socket to forward MEP Requests to Odin */

    

    int 	  m_TimeOut;    /* Time Out for every blocking call */
                                /* Only used for the polling */
                                 

    int 	  m_MEPSize;    /* Size of the MEP on which the algorithm is working on */

    bool          m_AutoMode;   /* Set by job options, to determine     */
 			        /* how Odin information will be managed */

    std::string   m_HLTIfIPAddr;    /* The destination address of HLT MEP Request */
    std::string   m_OdinIfIPAddr;   /* The source address to send MEP request to Odin */ 

    std::vector<std::string> m_HLTNodes;     /* The HLT nodes, to be pinged at initialization */ 
    std::vector<std::string> m_Tell1Boards;  /* The Tell1 enabled for the injector, format = {"IP", "HOSTNAME", "TYPE" } */ 

    int 	  m_OdinEthInterface; /* The Ethernet interface for Odin */   
    int           m_HLTEthInterface;  /* The Ethernet interface for HLT  */
 
    std::string   m_StrOdinIPAddr; /* IP Address of Odin */
    u_int32_t     m_BitOdinIPAddr; /* IP Address of Odin */
    

    std::string   m_HLTStrIPAddrTo;/* IP Address to send to, in string */
 
    u_int32_t     m_HLTIPAddrTo;   /* IP Address to send to */
    
    unsigned short m_EvtID;     /* Id of the event currently processed */

    int m_OdinTell1ID;          /* Id of the Odin board */

//    MEPReq m_MEPReq;            /* The MEP request packet */


    std::map<unsigned long, StreamBuffer> m_MapStreamBuffers;
    std::map<unsigned long, MEPEvent *> m_MapTell1MEPs;  

    MEPEvent*        m_OdinMEP;    /* The Odin MEP currently processed */  

    std::queue<char*> m_QueueOdinMEP; /* A queue of Odin MEP to be processed */

    std::map<unsigned int, unsigned int > m_HLTReqs; /* map<HLT IP, counter of request> */
    std::map < unsigned int, unsigned int >::iterator m_HLTReqsIte; /* iterator on the MEP Requests */ 

    ///////////////////////////////////////////////////////////////////////
    /// These attributes are set by Odin MEPs or by Job options in automode
    unsigned int  m_PackingFactor; /* MEP Event packing factor */

    u_int32_t     m_L0ID;          /* Level 0 ID               */
 
    u_int32_t     m_PartitionID;   /* Partition ID of the events*/	
    ///////////////////////////////////////////////////////////////////////

    //int m_probEventID;
    //int m_probPartID;
    //int m_probSize;
    

    ///////////////////////////////////////////////////////////////////////
    /// Private Methods	
 
    /// Copy the content of the src bank in dest bank. except for run number and GPS time
    void copyOdinBank(OnlineRunInfo *dest, OnlineRunInfo *src);

    /// In order to send ARP request at initialization
    StatusCode pingHLTNodes();
   
    /// Get the MTU configured by the system for the opened socket   
    int getMTU(int );

    /// Cook the MEP which will be sent, setting MEP header and fragment headers
    StatusCode prepareMEP(std::map<unsigned int, RawEvent*> &events, MEPEvent **me);

    /// Deal with the IP layer to send the MEP
    StatusCode sendMEP( int tell1IP, MEPEvent *me);

    /// Receive MEP(s) from Odin
    StatusCode receiveOdinMEP(char *bufMEP, int *len);

    /// Read an event from the MDF transient store and split fragments correctly
    StatusCode readEvent();

    /// Identify the Tell1 ID from the type and the source of the bank
    int rawTell1Id(int type, int src);   
    in_addr_t getTell1IP(int type, int src);   

    /// Set Odin MEP banks data
//    void setOdinData(MEPEvent **me);

    /// Get Information from the Odin MEP stored
    StatusCode getInfoFromOdinMEP();

    /// Copy the OdinMEP to the main thread memory space
    StatusCode getOdinInfo(); 
    
    /// Get the HLT which will receives the MEPs
    StatusCode getHLTInfo(); 

    /// Print the error message and return SatusCode::FAILURE
    StatusCode error(const std::string& msg);
   
    /// Print the MEP on the m_OutStream 
    void printMEP(MEPEvent * me, int size);
   
    /// Send a MEP Request to the Readout Supervisor 
    StatusCode sendMEPReq(MEPReq *req);  

    /// Somehow the main processing 
    StatusCode readThenSend();

    /// Receive a MEP Request from the HLT
    StatusCode receiveMEPReq(char *req);

    /// Clear monitoring counters
    void clearCounters();
 
    /// Setup monitoring counters
    int setupCounters();

    /// Publish monitoring counters
    void publishCounters(void);

    /// Fix OT non respect of the rules
    StatusCode OTFIX(int id, int nbEv);  

   public: 
    ///////////////////////////////////////////////////////////////////////
    /// Public Methods

    /// Standard constructor
    MEPInjector( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor
    virtual ~MEPInjector()  {} 

    /// Initialize 
    virtual StatusCode initialize();

    /// Finalize
    virtual StatusCode finalize();

    /// Run
    virtual StatusCode run();
  
    /// Handle 
    virtual void handle(const Incident& incident);
    
    /// Main execution
    virtual StatusCode execute();     
  
    /// Main processing of the injector
    StatusCode injectorProcessing(); 

    /// Get information from Odin
    StatusCode manageOdinMEP();

    StatusCode manageMEPRequest();

    ///////////////////////////////////////////////////////////////////////
    /// Public Attributes
    
    bool	  m_InjectorStop, m_ManagerStop; /* Boolean to tell the thread to stop the loop */

    pthread_mutex_t m_SyncMainOdin;  /* Mutex used to synchronise the end of the data send and the right of copy of Odin information */ 
    
    pthread_mutex_t m_SyncReqOdin;   /* Mutex used to synchronise the use of the map of HLT request counters */

    pthread_mutex_t m_SyncReadProcess;

    /// Streambuffer to hold uncompressed data. For encoding MEPs, contains everything of the MEP (4 bytes of size + MEP buffer)
    StreamBuffer  m_Data;
    StreamBuffer  m_OdinData;
    
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
/// A comparer used to find the HLT farm which has sent the most of MEP Requests
bool value_comparer(std::map<unsigned int, unsigned int>::value_type &i1, std::map<unsigned int, unsigned int>::value_type &i2)
{
  return i1.second<i2.second;
}
#endif // MEP_MAPMGT


#endif // ifndef _WIN32
