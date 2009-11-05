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
//  It also sums counters from the DiskWR process 
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
class IIncidentSvc;

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

    std::string                 m_listenerDnsNode;	// DNS node to listen for services. 
    int                         m_updateFrequency;	// How often to update

    int                         m_nrSubNodes;		// Number of nodes on a subfarm (subfarm mode)

    bool			m_sumPartition;		// TRUE: Run in partition sum mode
    std::string                 m_partitionName;	// Partition to sum for

    int				m_nrSubFarms;		// Number of subfarms in partition
    std::vector<std::string>    m_subFarms;		// List of all subfarms

    int                         m_nSrc;			// Number of TELL1 sources
    int				m_nrServices; 		// (internally used) == nrSubNodes or nrSubFarms

    char			*m_allNames;		// TELL1 source names from DIM service
    int				m_allNamesSize;

    longlong m_zero;

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
    std::vector<int64_t>			m_rTotRxEvt;
    std::vector<int64_t>			m_rIncEvt;
    std::vector<int64_t>			m_rTotBadMEP;
    std::vector<int64_t>			m_rTotMEPReq;
    std::vector<int64_t>			m_rTotMEPReqPkt;
    std::vector<int64_t>			m_rNumMEPRecvTimeouts;
    std::vector<int64_t>			m_rNotReqPkt;
    std::vector<int64_t>			m_rTotWrongPartID;    

    std::vector<int64_t>			m_rSentEvt;
    std::vector<int64_t>			m_rSentOct;
    std::vector<int64_t>			m_rSentEvtErr;

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
    int64_t			m_totRxEvt;
    int64_t			m_incEvt;
    int64_t			m_totBadMEP;
    int64_t			m_totMEPReq;
    int64_t			m_totMEPReqPkt;
    int64_t			m_numMEPRecvTimeouts;
    int64_t			m_notReqPkt;
    int64_t			m_totWrongPartID;

    int64_t			m_sentEvt;
    int64_t			m_sentOct;
    int64_t			m_sentEvtErr;


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
    std::vector<DimInfo*>	m_subsTotRxEvt;
    std::vector<DimInfo*>	m_subsIncEvt;
    std::vector<DimInfo*>	m_subsTotBadMEP;
    std::vector<DimInfo*>	m_subsTotMEPReq;
    std::vector<DimInfo*>	m_subsTotMEPReqPkt;
    std::vector<DimInfo*>	m_subsNumMEPRecvTimeouts;
    std::vector<DimInfo*>	m_subsNotReqPkt;
    std::vector<DimInfo*>	m_subsTotWrongPartID;

    std::vector<DimInfo*>	m_subsSentEvt;
    std::vector<DimInfo*>	m_subsSentOct;
    std::vector<DimInfo*>	m_subsSentEvtErr;

    std::vector<DimInfo*>	m_subsSrcName;

    DimInfo*			m_subsSubFarms;			//For partiton mode, look if partition setup changes

    IIncidentSvc*		m_incidentSvc;
    IMonitorSvc*                m_monSvc;    
 
    // Various functions
    void infoHandler();    
    int setupCounters();
    int resetAllCounters();
    int resetRem2DCounters();
    int resetRemSingleCounters();
    int publishSingleCounters();
    int publishArrayCounters();
    int unpublishArrayCounters();
    int resetPublishedServices(int nSrc);
    int changeSubFarms(DimInfo* subsSubFarms);
    int removeSubs();
    int readRunInfo(DimInfo*);
    int setupSubs();
    bool ReceiveArrayService(DimInfo * curr, DimInfo * subs,  std::vector<int64_t> &rArray, std::vector<int64_t> &sArray);
    bool ReceiveSingleService(DimInfo * curr, DimInfo * subs, int64_t &rValue, int64_t &sValue);

    MsgStream                   m_log;
  };

}

#endif
