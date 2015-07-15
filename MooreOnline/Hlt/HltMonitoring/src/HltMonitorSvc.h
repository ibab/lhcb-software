#ifndef HLTMONITORSVC_H
#define HLTMONITORSVC_H

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

   // List of all counters
   virtual std::vector<std::string> counters() const;

   // Simple wrapper to facilitate counting
   virtual RateCounter& rateCounter(const std::string& identifier) const;

   // Count counter at time t
   virtual void count(const Gaudi::StringKey& id, double t);

   // Simple wrapper to facilitate histogram filling
   virtual HltHistogram& histogram(const std::string& identifier,
                                   double left, double right,
                                   size_t bins) const;

   // List of all histograms
   virtual std::vector<std::string> histograms() const;

   // Fill histograms with value
   virtual void fill(const Gaudi::StringKey& id, size_t bin);

   virtual void handle(const Incident& incident);

private:

   template <class T, class C, class... Args>
   T& item(const std::string& identifier, C& container, Args&&... args) const {
      Gaudi::StringKey key{identifier};
      auto it = container.find(key);
      if (it == container.end()) {
         auto rc = new T(const_cast<HltMonitorSvc* const>(this), key, std::forward<Args>(args)...);
         auto r = container.emplace(key, rc);
         return *(r.first->second);
      } else {
         return *(it->second);
      }
   }

   StatusCode updateConditions();

   void sendChunks(bool all = true);

   void addInfo(Monitoring::HistId id, const std::string& type, const std::string& info) const;

   unsigned int tck() const;

   // properties
   std::string m_dataCon;
   std::string m_infoCon;
   std::string m_decRepLoc;
   double m_updateInterval;

   // data members
   IZeroMQSvc* m_zmqSvc;
   IIncidentSvc* m_incidentSvc;
   IUpdateManagerSvc *m_updMgrSvc;
   IDataProviderSvc* m_evtSvc;
   Condition *m_runPars;

   zmq::socket_t* m_dataOut;
   zmq::socket_t* m_infoOut;

   Monitoring::RunNumber m_run;
   mutable Monitoring::TCK m_tck;

   std::chrono::high_resolution_clock::time_point m_latestUpdate;
   int m_startOfRun;

   mutable std::vector<std::vector<zmq::message_t>> m_infoMessages;

   mutable std::unordered_map<Gaudi::StringKey, RateCounter*> m_counters;
   mutable std::unordered_map<Gaudi::StringKey, HltHistogram*> m_histograms;
   mutable std::unordered_map<Gaudi::StringKey, Monitoring::Chunk> m_chunks;

};

#endif // HLTMONITORSVC_H
