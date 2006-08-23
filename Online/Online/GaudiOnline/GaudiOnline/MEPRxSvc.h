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

#ifdef _WIN32
#ifndef u_int64_t
#define u_int64_t unsigned __int64
#endif
#ifndef u_int32_t
#define u_int32_t unsigned __int32
#endif
#ifndef u_int16_t
#define u_int16_t unsigned __int16
#endif
#ifndef u_int8_t
#define u_int8_t  unsigned  __int8
#endif
#else
#include <sys/types.h>
#endif

#include <map>
#include <vector>
#include <string>

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiOnline/MEPManager.h"
#include "RTL/rtl.h"

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
    *  @version 1.0
    */
    struct MEPRx;
    typedef std::vector<LHCb::MEPRx *>::iterator RXIT;

    class MEPRxSvc : public Service, virtual public IRunable,
    virtual public IIncidentListener { 
      public:
    lib_rtl_thread_t m_handle;
    bool m_receiveEvents, m_forceStop;
    int m_MEPBuffers, m_maxMsForGetSpace, m_pktSamplingCount, 
      m_nSrc, m_sockBuf, m_IPProtoIn, m_refCount, m_MEPBufSize, m_r, m_ethInterface;
    bool m_RTTCCompat;
    bool m_dynamicMEPRequest;
    u_int32_t m_IPOdin;
		std::string m_rxIPAddr;
    std::string m_IPNameOdin, m_bufName, m_mepMgrName;
    std::vector<std::string> m_IPSrc;
    float m_maxBadPktRatio;
    std::map<u_int32_t, int> m_srcAddr;
    std::vector<std::string> m_srcName;
    std::vector<struct MEPRx *> m_freeDsc, m_workDsc, m_usedDsc; 
    lib_rtl_lock_t m_freeDscLock, m_usedDscLock;
    u_int32_t m_partitionID;
    IIncidentSvc* m_incidentSvc; 
    MsgStream *m_log;
    /// Thread execution routine.
    static int exec(void* arg);
    int m_nCnt;
    private:
    IMonitorSvc* m_monSvc;
    u_int32_t m_odinIPAddr;
		u_int8_t *m_trashCan;

  public:
    MEPManager *m_mepMgr;
    int m_sourceID;
    u_int32_t m_ownAddress;
    /* Counters per source */ 
    u_int64_t *m_rxOct, *m_rxPkt;
    /* Global counters */
    int m_totRxPkt, m_totRxOct, m_incEvt;
    /* Error counters */
    u_int32_t *m_badPkt, *m_misPkt, m_swappedMEP;
    u_int64_t m_notReqPkt;
    /// Standard Constructor
    MEPRxSvc(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~MEPRxSvc();
    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// Service overload: initialize()
    virtual StatusCode initialize();
    /// Service overload: finalize()
    virtual StatusCode finalize();
    /// IRunable implementation: Run the object
    virtual StatusCode run();
    enum DAQError { MissingSrc = 0, BadPkt, Multiple, ShortPkt, WrongPartitionID, WrongPackingFactor,
		    DAQ_LAST_ERROR  /* LoopType */
    };
    RXIT ageRx();
    int getMyAddr(u_int32_t &);
    static bool cmpL0ID(MEPRx *, u_int32_t);
    void removePkt(void);
    int setupMEPReq(std::string);
    int sendMEPReq(int);
    void freeRx(void);
    void forceEvent(RXIT &);
    void forceEvents(void);
    int getSrcID(u_int32_t);
    int checkProperties();
    int allocRx();
    int openSock();
    // counter functions
    void clearCounters();
    int setupCounters(int);
    void publishCounters(void);
  public:
    void handle(const Incident& inc);

 
  };
  
  struct DAQErrorBankEntry {    
      /* LHCb Bankheader */
      u_int16_t m_magic;
      u_int16_t m_length;
      u_int8_t  m_type;
      u_int8_t  m_version;
      u_int16_t m_sourceID;
      int m_srcID;
      u_int32_t m_srcIPAddr;
      MEPRxSvc::DAQError m_errorType; 
      void *m_pktData;
  };
}      // End namespace LHCb
#ifdef _WIN32
#undef u_int64_t
#undef u_int32_t
#undef u_int16_t
#undef u_int8_t
#endif
#endif //  GAUDIONLINE_MEPRXSVC_H

