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
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiOnline/MEPManager.h"
#include "RTL/rtl.h"
#include "RTL/types.h"

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

    class MEPRxSvc : public Service, 
                     virtual public IRunable,
                     virtual public IIncidentListener 
    {
    public:
      typedef std::vector<LHCb::MEPRx *> Workers;
      typedef Workers::iterator RXIT;
      bool         m_receiveEvents;
      bool         m_forceStop;
      bool         m_RTTCCompat;
      bool         m_dynamicMEPRequest;

      int          m_MEPBuffers; 
      int          m_maxMsForGetSpace;
      int          m_pktSamplingCount; 
      int          m_nSrc;
      int          m_sockBuf;
      int          m_IPProtoIn;
      int          m_refCount;
      int          m_MEPBufSize;
      int          m_r;
      int          m_ethInterface;
      u_int32_t    m_partitionID;
      u_int32_t    m_IPOdin;
      u_int32_t    m_odinIPAddr;
      u_int8_t    *m_trashCan;
      std::string  m_rxIPAddr;
      std::string  m_IPNameOdin;
      std::string  m_bufName;
      std::string  m_mepMgrName;
      std::vector<std::string>    m_IPSrc;
      float                       m_maxBadPktRatio;
      std::map<u_int32_t, int>    m_srcAddr;
      std::vector<std::string>    m_srcName;
      Workers                     m_freeDsc;
      Workers                     m_workDsc;
      Workers                     m_usedDsc; 
      IIncidentSvc*               m_incidentSvc; 
      int                         m_nCnt;
      lib_rtl_lock_t m_freeDscLock;
      lib_rtl_lock_t m_usedDscLock;
      IMonitorSvc* m_monSvc;

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
      enum DAQError { 
        MissingSrc = 0, 
        BadPkt, 
        Multiple,
        ShortPkt,
        WrongPartitionID,
        WrongPackingFactor,
	      DAQ_LAST_ERROR  /* LoopType */
      };
      RXIT ageRx();
      static bool cmpL0ID(MEPRx *, u_int32_t);
      void removePkt(void);
      int setupMEPReq(std::string);
      int sendMEPReq(int);
      void freeRx(void);
      void forceEvent(RXIT &);
      void forceEvents(void);
      int getSrcID(u_int32_t);
      StatusCode checkProperties();
      StatusCode error(const std::string& msg);
      StatusCode allocRx();
      StatusCode releaseRx();
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

#endif //  GAUDIONLINE_MEPRXSVC_H

