//  ============================================================
//
//  MEPErrorAdder.h
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : David Svantesson 
//
//  This service adds up the counters of received and errornous MEPs per TELL1-source. One service runs for each subfarm and one for each partition.
//  
//
//  ===========================================================
#ifndef GAUDIONLINE_ERRORADDINGSVC_H
#define GAUDIONLINE_ERRORADDINGSVC_H 1

#include <map>
#include <vector>
#include <string>

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMonitorSvc.h"

#include "dis.hxx"
#include "dic.hxx"

// Forward declarations
class MsgStream;
class ISvcLocator;


/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  class MEPErrorAdder : public Service,
			  virtual public IRunable,
			  virtual public IIncidentListener,
                          public DimClient
  {
  public:
    MEPErrorAdder(const std::string& name, ISvcLocator* svc);
    ~MEPErrorAdder();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode run();
    virtual StatusCode queryInterface(const InterfaceID& riid,
				      void** ppvInterface);	
    void handle(const Incident&);
  private: 
    enum ServiceState { NOT_READY, READY, RUNNING, STOPPED };
    ServiceState		m_svcState;

    std::string                 m_listenerDnsNode;
    std::string			m_runInfoDnsNode;
    std::string                 m_partitionName;
    int                         m_updateFrequency;
    int                         m_nrSubNodes;
    bool			m_sumPartition;

    int				m_nrSubFarms;
    std::vector<std::string>    m_subFarms;
    int                         m_nSrc;

    int				m_nrServices; 		// == nrSubNodes or nrSubFarms

    char			*m_allNames;
    int				m_allNamesSize;

    //Save all received values from all sources
    std::vector<std::vector<int64_t> >      	m_rBadLenPkt;
    std::vector<std::vector<int64_t> >      	m_rMisPkt;
    std::vector<std::vector<int64_t> >      	m_rBadPckFktPkt;
    std::vector<std::vector<int64_t> >      	m_rTruncPkt;
    std::vector<std::vector<int64_t> >      	m_rMultipleEvt;
    std::vector<std::vector<int64_t> >      	m_rRxOct;
    std::vector<std::vector<int64_t> >      	m_rRxPkt;
    std::vector<std::vector<int64_t> >  	m_rRxEvt;
    
    std::vector<int64_t>			m_rTotRxOct;
    std::vector<int64_t>			m_rTotRxPkt;
    std::vector<int64_t>			m_rIncEvt;
    std::vector<int64_t>			m_rTotBadMEP;
    std::vector<int64_t>			m_rTotMEPReq;
    std::vector<int64_t>			m_rTotMEPReqPkt;
    std::vector<int64_t>			m_rNumMEPRecvTimeouts;
    std::vector<int64_t>			m_rNotReqPkt;
    std::vector<int64_t>			m_rTotWrongPartID;    

    //Summed up vectors to send
    std::vector<int64_t>        m_badLenPkt;
    std::vector<int64_t>        m_misPkt;
    std::vector<int64_t>        m_badPckFktPkt;
    std::vector<int64_t>        m_truncPkt;
    std::vector<int64_t>        m_multipleEvt;
    std::vector<int64_t>        m_rxOct;
    std::vector<int64_t>       	m_rxPkt;
    std::vector<int64_t>        m_rxEvt; 

    //Summed up single counters to send
    int64_t			m_totRxOct;
    int64_t			m_totRxPkt;
    int64_t			m_incEvt;
    int64_t			m_totBadMEP;
    int64_t			m_totMEPReq;
    int64_t			m_totMEPReqPkt;
    int64_t			m_numMEPRecvTimeouts;
    int64_t			m_notReqPkt;
    int64_t			m_totWrongPartID;

    //Subscriptions
    std::vector<DimInfo*> 	m_subsBadLenPkt;
    std::vector<DimInfo*> 	m_subsMisPkt;       
    std::vector<DimInfo*> 	m_subsBadPckFktPkt;
    std::vector<DimInfo*> 	m_subsTruncPkt;
    std::vector<DimInfo*> 	m_subsMultipleEvt;
    std::vector<DimInfo*> 	m_subsRxOct;
    std::vector<DimInfo*> 	m_subsRxPkt;
    std::vector<DimInfo*> 	m_subsRxEvt;

    std::vector<DimInfo*>	m_subsTotRxOct;
    std::vector<DimInfo*>	m_subsTotRxPkt;
    std::vector<DimInfo*>	m_subsIncEvt;
    std::vector<DimInfo*>	m_subsTotBadMEP;
    std::vector<DimInfo*>	m_subsTotMEPReq;
    std::vector<DimInfo*>	m_subsTotMEPReqPkt;
    std::vector<DimInfo*>	m_subsNumMEPRecvTimeouts;
    std::vector<DimInfo*>	m_subsNotReqPkt;
    std::vector<DimInfo*>	m_subsTotWrongPartID;

    std::vector<DimInfo*>	m_subsSrcName;

    IIncidentSvc*		m_incidentSvc;
    IMonitorSvc*                m_monSvc;    
 
    void infoHandler();
    int setupCounters();
    bool ReceiveArrayService(DimInfo * curr, DimInfo * subs,  std::vector<int64_t> &rArray, std::vector<int64_t> &sArray);
    bool ReceiveSingleService(DimInfo * curr, DimInfo * subs, int64_t rValue, int64_t sValue);

    MsgStream                   m_log;
  };

}

#endif
