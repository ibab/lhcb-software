//	============================================================
//
//	MPRxSvc.h
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
#include "GaudiKernel/IHistogramSvc.h"
#include <GaudiKernel/IMonitorSvc.h>
#include "AIDA/IHistogram1D.h"
// Online stuff
#include "RTL/rtl.h"
#include "RTL/types.h"
#include "NET/IPHeader.h"
#include "GaudiOnline/MEPHdr.h"
#include "GaudiOnline/DAQError.h"
#include "dis.hxx"

// Forward declarations
class MsgStream;
class ISvcLocator;
class IIncidentSvc;

/*
 *  LHCb namespace declaration
 */
namespace LHCb {

/** @class MEPRxSvc MEPRxSvc.h GaudiOnline/MEPRxSvc.h
 *
 *  @author Niko Neufeld (using code by B. Gaidioz and M. Frank)
 *  @version 2.0
 */
struct MEPRx;
class MEPRxSvc;
class MEPRQCommand;
class ClearMonCommand;
class UpMonCommand;
class SetOverflowCmd;
class OverflowStatSvc;
struct DAQErrorEntry {
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
        virtual public IIncidentListener {
public:
    typedef std::vector<LHCb::MEPRx *> Workers;
    typedef Workers::iterator          RXIT;
    friend struct MEPRx;
    enum EvtBuilderState { NOT_READY, READY, RUNNING, STOPPED };
    enum ForceReason { END_OF_RUN, TIME_OUT, NO_BUFFER };
    bool m_LocalTest;   /*To tell to run locally for testing. */
    int m_DestTestPort;     /*udp test ports */
    int m_SrcTestPort;
    char                       *m_allNames;
    int                         m_dataSock; /* Raw socket for receiving data.*/
    int                         m_mepSock;  /* Raw socket to send MEP requests.*/
    EvtBuilderState             m_ebState;
    bool                        m_forceStop;
    bool                        m_dynamicMEPRequest;
    bool                        m_dropIncompleteEvents;
    bool                        m_checkPartitionID;
    bool                        m_expectOdin;
    bool                        m_createDAQErrorMEP;
    bool                        m_createODINMEP; // fake ODIN MEP for timepix testbeam only!
    bool                        m_resetCounterOnRunChange;
    bool                        m_alwaysSendMEPReq; // send a MEP request for every event (complete or not)
    int                        m_overflow;
    bool                        m_overflowActive;
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
    int                         m_MEPsPerMEPReq;      /* Number of MEPs requested per MEPReq.*/
    unsigned int                m_MEPRecvTimeout;     /* Select timeout for waiting for a MEP.*/
    std::string                 m_nameLiveBuf;
    std::string                 m_nameOverflowBuf;
    u_int32_t                   m_partitionID;
    u_int32_t                   m_IPOdin;
    u_int32_t                   m_odinIPAddr;
    unsigned int                m_maxEventAge; // milliseconds
    u_int8_t                   *m_trashCan;
    std::string                 m_rxIPAddr;
    std::string                 m_IPNameOdin;
    std::string                 m_bufName;
    std::string                 m_overflowPath;        // the path to the overflow directory in the local FS
    std::string                 m_badDiskList;         // file containing node names with broken disks (no overflow)
    int                         m_quotaCheckInterval;  // after how many events the quota is re-checked
    int                         m_nAllocMEPBuf;        // how many times m_MEPBufSize is allocated from the BM (default = 1)
    std::vector<std::string>    m_IPSrc;
    float                       m_maxBadPktRatio;
    std::map<u_int32_t,int>     m_srcAddr;
    std::vector<std::string>    m_srcName;
    std::vector<std::string>    m_srcDottedAddr;
    std::vector<int>            m_srcFlags;
    Workers                     m_freeDsc;
    Workers                     m_workDsc;
    Workers                     m_usedDsc;
    LHCb::MEPRx                 *m_ovflBuf;
    lib_rtl_lock_t              m_freeDscLock;
    lib_rtl_lock_t              m_usedDscLock;
    MEPRQCommand                *m_mepRQCommand;
    UpMonCommand		*m_upMonCommand;
    ClearMonCommand		*m_clearMonCommand;
    SetOverflowCmd              *m_setOverflowCmd;
    OverflowStatSvc             *m_overflowStatSvc;
    //SourceStatService           *m_statService;
    IIncidentSvc*               m_incidentSvc;
    IMonitorSvc*                m_monSvc;
    IHistogramSvc*              m_histSvc;
    int                         m_sourceID;
    u_int32_t                   m_ownAddress;
    /* Counters per source */
    std::vector<int64_t>        m_rxOct;
    std::vector<int64_t>        m_rxPkt;
    std::vector<int64_t>        m_rxEvt;
    std::vector<int64_t>        m_rxMEP;
    std::vector<int>	        m_noShow;
    /* Global counters */
    int64_t 			              m_numMEPRecvTimeouts;
    int64_t			                m_numMEPReq;			//This seem to not be used?
    int64_t			                m_totMEPReq, m_totMEPReqPkt;
    int64_t                     m_totRxPkt;
    int64_t                     m_totRxOct;
    int64_t			                m_totRxEvt;
    int64_t			                m_totRxMEP;
    int64_t                     m_notReqPkt;
    int64_t                     m_incEvt;
    int64_t                     m_totBadMEP;
    int                         m_totMEPproduced;
    int                         m_totMEPproducedLive;
    int                         m_totMEPproducedOvfl;
    int                         m_maxErrors;
    std::vector<int64_t>        m_badLenPkt;
    std::vector<int64_t>        m_misPkt;
    std::vector<int64_t>        m_badPckFktPkt;
    std::vector<int64_t>        m_truncPkt;
    std::vector<int64_t>        m_emptyPkt;
    std::vector<int64_t>	      m_multipleEvt;
    IHistogram1D                *m_complTimeTSC; // how long to complete an evt
    IHistogram1D                *m_idleTimeTSC;  // how long between two evts
    IHistogram1D                *m_complTimeSock; // how long to complete an evt
    IHistogram1D                *m_idleTimeSock;  // how long between two evts
    IHistogram1D		            *m_L0IDDiff ; //L0ID gap between two evts
    u_int64_t                   m_runNumber;
    u_int64_t                   m_tLastAdded;   // time of last added fragment in us
    u_int64_t                   m_tLastComp;    // time of last completed event in us
    u_int64_t                   m_tLastRx;      // time of last received frament in us
    u_int64_t                   m_tzero;        // time of first (since process-start) received fragment in us
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

    u_int32_t partitionID() const           {
        return m_partitionID;
    }
    int       numberOfSources() const       {
        return m_nSrc;
    }
    int       mepRefcount()  const          {
        return m_refCount;
    }
    int       spaceSize()  const            {
        return m_MEPBufSize;
    }
    int       sourceID() const              {
        return m_sourceID;
    }
    int       sourceAddr(u_int32_t i)       {
        return m_srcAddr[i];
    }
    int64_t   addIncompleteEvent()          {
        return m_incEvt++;
    }
    RXIT oldestRx();
    static bool cmpL0ID(MEPRx *r, u_int32_t id);
    void removePkt(void);
    StatusCode setupMEPReq(const std::string& odinName);
    StatusCode sendMEPReq(int);
    void freeRx(void);
    void forceEvent(RXIT, enum ForceReason reason);
    int getSrcID(u_int32_t);
    StatusCode checkProperties();
    StatusCode error(const std::string& msg);
    StatusCode info(const std::string &msg);
    StatusCode allocRx();
    StatusCode releaseRx();
    int openSocket(int protocol);
    int setupTimer(void);
    void clearCounters();
    int setupCounters();
    void publishCounters(void);
    void publishHists(void);
    void handle(const Incident&);
    void checkTimeOut(void);
    void truncatedPkt(struct RTL::IPHeader *);
    IMonitorSvc* getMonSvc() {
        return m_monSvc;
    }
    void srcSwap(int, int);
    int srcFindMedianOfMedians(int, int);
    int srcFindMedianIndex(int, int, int);
    int srcPart(int, int);
    void srcSort(int, int);
};

/**
 * A simple DIM command implementation that causes the monitoring counters and histogram to be reset.
 */
class ClearMonCommand : public DimCommand {
    MEPRxSvc *m_mepRxObj;
    IMessageSvc* m_msgSvc;

public:

    // Constructor
    ClearMonCommand(MEPRxSvc *mepRxObj, IMessageSvc *log, const std::string& cmd_name)
        : DimCommand(std::string(cmd_name + "/ClearMonitor").c_str(), (char*)"I"),
          m_mepRxObj(mepRxObj),m_msgSvc(log) {
    }

    ~ClearMonCommand() { }

    virtual void commandHandler(void);
};

/**
 * A simple DIM command implementation that causes the histograms to be updated.
 */
class UpMonCommand : public DimCommand {
    MEPRxSvc *m_mepRxObj;
    IMessageSvc* m_msgSvc;

public:

    // Constructor
    UpMonCommand(MEPRxSvc *mepRxObj, IMessageSvc *log, const std::string& cmd_name)
        : DimCommand(std::string(cmd_name + "/UpMon").c_str(), (char*)"I"),
          m_mepRxObj(mepRxObj),m_msgSvc(log) {
    }

    ~UpMonCommand() { }

    virtual void commandHandler(void);
};

/**
 * A simple DIM command implementation that causes an explicit MEP request to be sent.
 */
class MEPRQCommand : public DimCommand {
    MEPRxSvc *m_mepRxObj;
    IMessageSvc* m_msgSvc;
public:
    MEPRQCommand(MEPRxSvc *mepRxObj, IMessageSvc *log, const std::string& cmd_name)
        : DimCommand(std::string(cmd_name + "/sendMEPRQ").c_str(), (char*)"I"),
          m_mepRxObj(mepRxObj),m_msgSvc(log) {
    }
    virtual ~MEPRQCommand() { }
    virtual void commandHandler(void);
};
/**
 *  A DIM command which sets the m_overflow variable (0 is off) (1 is on)
 */
class SetOverflowCmd: public DimCommand {
    MEPRxSvc *m_mepRxObj;
    IMessageSvc* m_msgSvc;
public:
    bool m_permDisable;
    SetOverflowCmd(MEPRxSvc *mepRxObj, IMessageSvc *log, const std::string& cmd_name)
        : DimCommand(std::string(cmd_name + "/setOverflow").c_str(), (char *)"I"),
          m_mepRxObj(mepRxObj), m_msgSvc(log), m_permDisable(false) {
    }
    virtual ~SetOverflowCmd() {}
    virtual void commandHandler(void);
};

class OverflowStatSvc: public DimService {
    MEPRxSvc *m_mepRxObj;
public:
    OverflowStatSvc(MEPRxSvc *mepRxObj, IMessageSvc* /* log */, const std::string& svc_name)
      : DimService(std::string(svc_name + "/OverflowStatus").c_str(), (char *)"C"),
      m_mepRxObj(mepRxObj) {
    }
    virtual ~OverflowStatSvc() {}
    virtual void serviceHandler();
};
}      // End namespace LHCb

#endif //  GAUDIONLINE_MEPRXSVC_H


