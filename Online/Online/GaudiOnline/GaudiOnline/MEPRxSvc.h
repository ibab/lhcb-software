//	============================================================
//
//	MEPRxSvc.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Niko Neufeld 
//                  using code by B. Gaidioz and M. Frank
//
//	===========================================================
#ifndef GAUDIONLINE_MEPRXSVC_H
#define GAUDIONLINE_MEPRXSVC_H 1

#include <map>
#include <vector>
#include <string>

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMonitorSvc.h"

// Online stuff
#include "RTL/rtl.h"
#include "RTL/types.h"
#include "NET/IPHeader.h"
#include "GaudiOnline/MEPHdr.h"
#include "dis.hxx"


// Forward declarations
class MsgStream;
class ISvcLocator;
class IIncidentSvc;

// defines which should eventually go to some bank-type class 
#ifndef DAQ_ERR_BANK_VERSION
#define DAQ_ERR_BANK_VERSION 0
#endif

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MEPRxSvc MEPRxSvc.h GaudiOnline/MEPRxSvc.h
    *
    *  @author Niko Neufeld (using code by B. Gaidioz and M. Frank)
    *  @version 1.1
    */
  struct MEPRx;
  class MEPRxSvc;
  class MEPRQCommand;

  struct DAQErrorEntry {    
    enum DAQErrorType { 
      MissingSrc = 0, 
      BadPkt, 
      Multiple,
      ShortPkt,
      WrongPartitionID,
      WrongPackingFactor,
      EmptyMEP,
      DAQ_LAST_ERROR  /* LoopType */
    };
      
    /* LHCb Bankheader */
    u_int16_t m_magic;
    u_int16_t m_length;
    u_int8_t  m_type;
    u_int8_t  m_version;
    u_int16_t m_sourceID;
    int m_srcID;
    u_int32_t m_srcIPAddr;
    DAQErrorType m_errorType; 
    void *m_pktData;
  };

  class MEPRxSvc : public Service, 
                    virtual public IRunable,
                    virtual public IIncidentListener 
  {
  public:
    typedef std::vector<LHCb::MEPRx *> Workers;
    typedef Workers::iterator          RXIT;
    friend struct MEPRx;

  protected:
    enum EvtBuilderState { NOT_READY, READY, RUNNING, STOPPED };

    char                       *m_allNames;
    int                         m_dataSock; /* Raw socket for receiving data.*/
    int                         m_mepSock;  /* Raw socket to send MEP requests.*/
    EvtBuilderState             m_ebState;
    bool                        m_forceStop;
    bool                        m_RTTCCompat;
    bool                        m_dynamicMEPRequest;
    bool                        m_dropIncompleteEvents;	
    bool                        m_checkPartitionID;
    bool                        m_expectOdin;
    int                         m_MEPBuffers; 
    int                         m_maxMsForGetSpace;
    int                         m_pktSamplingCount; 
    int                         m_nSrc;
    int                         m_sockBuf;
    int                         m_IPProtoIn;
    int                         m_refCount;
    int                         m_nCrh;
    int                         m_MEPBufSize;
    int                         m_ethInterface;
    int                         m_initialMEPReq;	/* Number of initial MEPReqs to send.*/
    int                         m_MEPsPerMEPReq; /* Number of MEPs requested per MEPReq.*/
    int                         m_MEPRecvTimeout; /* Select timeout for waiting for a MEP.*/
    u_int32_t                   m_partitionID;
    u_int32_t                   m_IPOdin;
    u_int32_t                   m_odinIPAddr;
    unsigned int                m_maxEventAge; // (in millis)
    u_int8_t                   *m_trashCan;
    std::string                 m_rxIPAddr;
    std::string                 m_IPNameOdin;
    std::string                 m_bufName;
    std::vector<std::string>    m_IPSrc;
    float                       m_maxBadPktRatio;
    std::map<u_int32_t,int>     m_srcAddr;
    std::vector<std::string>    m_srcName;
    std::vector<std::string>    m_srcDottedAddr;
    std::vector<int>            m_srcFlags;
    Workers                     m_freeDsc;
    Workers                     m_workDsc;
    Workers                     m_usedDsc; 
    lib_rtl_lock_t              m_freeDscLock;
    lib_rtl_lock_t              m_usedDscLock;
    MEPRQCommand                *m_mepRQCommand;
    //SourceStatService           *m_statService;
    
    IIncidentSvc*               m_incidentSvc; 
    IMonitorSvc*                m_monSvc;

    int                         m_sourceID;

    u_int32_t                   m_ownAddress;

    /* Counters per source */ 
    std::vector<int>            m_rxOct;
    std::vector<int>            m_rxPkt;
    std::vector<int>            m_rxEvt;

    /* Global counters */
    int m_numMEPRecvTimeouts, m_numMEPReq, m_totMEPReq, m_totMEPReqPkt;

    // Error counters
    // u_int64_t m_notReqPkt, m_incEvt; // 64-bit not yet supported in IMonitorSvc

    DimService * m_test;
    int * m_testint;

  public:
    int                         m_totRxPkt;
    int                         m_totRxOct;
    int                         m_notReqPkt;
    int                         m_incEvt;
    int                         m_totBadMEP;
    int                         m_totWrongPartID;
    unsigned long               m_timer;
    int                         m_maxErrors;
    int                         m_nErrorSamples;
    int                         m_errorCheckInterval; // (in millis)
    bool                        m_cryError;
    std::vector<int>            m_badLenPkt;
    std::vector<int>            m_misPkt;
    std::vector<int>            m_badPckFktPkt;
    std::vector<int>            m_truncPkt;
    std::vector<int>            m_emptyPkt;
    std::vector<int>		m_multipleEvt;

    /// Standard Constructor
    MEPRxSvc(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~MEPRxSvc();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, 
				      void** ppvInterface);
    /// Service overload: initialize()
    virtual StatusCode initialize();
    /// Service overload: finalize()
    virtual StatusCode finalize();
    /// IRunable implementation: Run the object
    virtual StatusCode run();

    u_int32_t partitionID() const           {  return m_partitionID;  }
    int       numberOfSources() const       {  return m_nSrc;         }
    int       mepRefcount()  const          {  return m_refCount;     }
    int       spaceSize()  const            {  return m_MEPBufSize;   }
    int       sourceID() const              {  return m_sourceID;     }
    int       sourceAddr(u_int32_t i)       {  return m_srcAddr[i];   }
    int       addIncompleteEvent()          {  return m_incEvt++;     }
    int       totWrongPartID()              {  return m_totWrongPartID;  }   
    RXIT ageRx();
    static bool cmpL0ID(MEPRx *r, u_int32_t id);
    void removePkt(void);
    StatusCode setupMEPReq(const std::string& odinName);
    StatusCode sendMEPReq(int);
    void freeRx(void);
    void forceEvent(RXIT &);
    int getSrcID(u_int32_t);
    StatusCode checkProperties();
    StatusCode error(const std::string& msg);
    StatusCode allocRx();
    StatusCode releaseRx();
    int openSocket(int protocol);
    int setupTimer(void);
    void clearCounters();
    int setupCounters();
    void publishCounters(void);
    void handle(const Incident&); 
    void ageEvents(void);
    int checkPartitionID(u_int32_t addr, struct MEPHdr *);
    void cryError(void);
    void truncatedPkt(struct RTL::IPHeader *);
  };

  /**
   * A simple DIM command implementation that causes an explicit MEP request to be sent.
   */
  class MEPRQCommand : public DimCommand {

    MEPRxSvc *m_mepRxObj;
    IMessageSvc* m_msgSvc;

    public:

    // Constructor
    MEPRQCommand(MEPRxSvc *mepRxObj, IMessageSvc *log, const std::string& cmd_name) 
   : DimCommand(std::string(cmd_name + "/sendMEPRQ").c_str(), "I"),
      m_mepRxObj(mepRxObj),m_msgSvc(log) {
    }

    ~MEPRQCommand() { }

    virtual void commandHandler(void);
  };


}      // End namespace LHCb

#endif //  GAUDIONLINE_MEPRXSVC_H

