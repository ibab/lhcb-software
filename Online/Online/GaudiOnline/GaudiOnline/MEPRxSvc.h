#if 0
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

#include <sys/types.h>
#include <vector>
#include <string>

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "RTL/rtl.h"

// Forward declarations
class ISvcLocator;
class IIncidentSvc;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class MEPRxSvc MEPRxSvc.h GaudiOnline/MEPRxSvc.h
    *
    *  @author Niko Neufeld (using code by B. Gaidioz and M. Frank)
    *  @version 1.0
    */
  class MEPRxSvc : public Service, virtual public IRunable {
    lib_rtl_thread_t m_handle;
    int m_MEPBuffers, m_maxMsForGetSpace, m_pktSamplingCount, m_ethDevIn, 
	m_nSrc, m_sockBuf, m_IPProtoIn, m_refCount, m_MEPBufSize;
    std::string m_IPOdin, m_bufName;
    std::vector<std::string> m_IPSrc;
    float m_maxBadPktRatio;
    std::map<u_int32_t, int> m_srcAddr;
    std::vector<std::string> m_srcName;
    u_int32_t m_partitionID;
    IIncidentSvc* m_incidentSvc;
    struct iovec m_trashVec[1]; 
    /// Thread execution routine.
    static int exec(void* arg);
    /// Worker routine
    int execute();
  public:
    /* Counters */
    u_int64_t *m_rxOct, *m_rxPkt;
    /* Error-counters */
    u_int32_t *m_badPkt, *m_misPkt;
    u_int64_t notReqPkt;
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
  };
}      // End namespace LHCb
#endif //  GAUDIONLINE_MEPRXSVC_H
#endif /* if 0 ////////
