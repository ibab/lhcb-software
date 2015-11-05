#ifndef HLTINFOSVC_H
#define HLTINFOSVC_H

// STD & STL
#include <string>
#include <map>
#include <array>
#include <set>
#include <unordered_map>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiKernel/IIncidentListener.h>
#include "GaudiKernel/IUpdateManagerSvc.h"

// Det
#include "DetDesc/Condition.h"

// HLT
#include <Kernel/IHltMonitorSvc.h>
#include <Kernel/RateCounter.h>
#include <Kernel/HltHistogram.h>

// ZeroMQ
#include <ZeroMQ/IZeroMQSvc.h>

class ISvcLocator;
class IIncidentSvc;
class IDataProviderSvc;

/** @class HltInfoSvc

 */

class HltInfoSvc : public Service {
public:

   HltInfoSvc(const std::string& name, ISvcLocator* sl);
   virtual ~HltInfoSvc();

   // Service pure virtual member functions
   virtual StatusCode initialize();
   virtual StatusCode start();
   virtual StatusCode stop();
   virtual StatusCode finalize();

private:

   IZeroMQSvc& zmq() const { return *m_zmqSvc; }

   StatusCode updateConditions();

   void sendInfo();

   // properties
   std::string m_infoCon;
   std::string m_lumiCond;
   std::string m_runCond;
   std::string m_decRepLoc;
   std::string m_tckCond;

   // data members
   IZeroMQSvc* m_zmqSvc;
   IDataProviderSvc* m_evtSvc;
   IUpdateManagerSvc* m_updMgrSvc;

   std::map<std::string, Condition*> m_conditions;

   zmq::socket_t* m_infoOut;

   Monitoring::RunInfo m_info;

   mutable std::vector<std::vector<zmq::message_t>> m_infoMessages;

};

#endif // HLTINFOSVC_H
