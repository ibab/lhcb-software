#ifndef HLTMONITORSVC_H
#define HLTMONITORSVC_H

// STD & STL
#include <string>
#include <map>
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

class ISvcLocator;
class IIncidentSvc;
class IDataProviderSvc;

/** @class HltMonitorSvc

 */

class HltMonitorSvc : public extends2<Service, IHltMonitorSvc, IIncidentListener> {
public:

   HltMonitorSvc(const std::string& name, ISvcLocator* sl);
   virtual ~HltMonitorSvc();

   // Service pure virtual member functions
   virtual StatusCode initialize();
   virtual StatusCode start();
   virtual StatusCode stop();
   virtual StatusCode finalize();

   // Count counter at time t
   virtual void count(const Gaudi::StringKey& id, double t);

   // Simple wrapper to facilitate counting
   virtual RateCounter& rateCounter(const std::string& identifier) const;

   // List of all counters
   virtual std::vector<std::string> counters() const;

   virtual void handle(const Incident& incident);

private:

   StatusCode updateConditions();

   void sendChunk(const Monitoring::Chunk& chunk);

   // properties
   std::string m_outCon;
   std::string m_decRepLoc;
   size_t m_chunkOverlap;
   size_t m_updateInterval;

   // data members
   IIncidentSvc* m_incidentSvc;
   IUpdateManagerSvc *m_updMgrSvc;
   IDataProviderSvc* m_evtSvc;
   Condition *m_runPars;

   zmq::context_t* m_context;
   zmq::socket_t* m_output;

   unsigned int m_run;
   unsigned int m_tck;
   double m_startOfRun;

   mutable std::unordered_map<Gaudi::StringKey, RateCounter*> m_counters;
   mutable std::unordered_map<Gaudi::StringKey, Monitoring::Chunk> m_chunks;

};

#endif // HLTMONITORSVC_H
