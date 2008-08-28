//  ============================================================
//
//  MEPErrorAdder.h
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : David Svantesson 
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
    std::string                 m_senderDnsNode;
    std::string			m_runInfoDnsNode;
    std::string                 m_partitionName;
    int                         m_updateFrequency;
    int                         m_nrSubNodes;
    int                         m_nrSubFarms;
    bool			m_sumPartition;


    std::vector<std::string>    m_subFarms;
    int                         m_nSrc;

    //Save all received values from all sources
    std::vector<std::vector<int> >      m_rBadLenPkt;
    std::vector<std::vector<int> >      m_rMisPkt;
    std::vector<std::vector<int> >      m_rBadPckFktPkt;
    std::vector<std::vector<int> >      m_rTruncPkt;
    std::vector<std::vector<int> >      m_rMultipleEvt;
    std::vector<std::vector<int> >      m_rRxOct;
    std::vector<std::vector<int> >      m_rRxPkt;
    std::vector<std::vector<int> >      m_rRxEvt;
    
    //Summed up vectors to send
    std::vector<int>            m_badLenPkt;
    std::vector<int>            m_misPkt;
    std::vector<int>            m_badPckFktPkt;
    std::vector<int>            m_truncPkt;
    std::vector<int>            m_multipleEvt;
    std::vector<int>            m_rxOct;
    std::vector<int>            m_rxPkt;
    std::vector<int>            m_rxEvt; 

    //Subscriptions
    std::vector<DimInfo*> m_subsBadLenPkt;
    std::vector<DimInfo*> m_subsMisPkt;       
    std::vector<DimInfo*> m_subsBadPckFktPkt;
    std::vector<DimInfo*> m_subsTruncPkt;
    std::vector<DimInfo*> m_subsMultipleEvt;
    std::vector<DimInfo*> m_subsRxOct;
    std::vector<DimInfo*> m_subsRxPkt;
    std::vector<DimInfo*> m_subsRxEvt;

    IIncidentSvc*		m_incidentSvc;
    IMonitorSvc*                m_monSvc;    
 
    void infoHandler();
    int setupCounters();

    MsgStream                   m_log;
  };

}

#endif
